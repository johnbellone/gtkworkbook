/* 
   application.c - Application Object Source File

   The GTKWorkbook Project <http://gtkworkbook.sourceforge.net/>
   Copyright (C) 2008, 2009 John Bellone, Jr. <jvb4@njit.edu>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PRACTICAL PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301 USA
*/
#include "application.h"
#include "plugin.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <glib/gthread.h>

/* application.c (static) */
static int application_method_run (ApplicationState *);
static guint application_signal_delete_event (GtkWindow *,
					      GdkEvent *, 
					      gpointer);
static guint application_signal_destroy_event (GtkWidget *, gpointer);
static void application_method_close (ApplicationState *);
static ApplicationState* application_object_free (ApplicationState *);
static ApplicationState* application_object_init (void);
static Plugin* application_method_loadplugin (ApplicationState *, 
					      const gchar *);
static void application_method_openextension (ApplicationState *,
					      const gchar *,
					      gboolean);
static guint application_signal_gtknotebook_switchpage (GtkNotebook *,
							GtkNotebookPage *,
							gint,
							Workbook *);
static guint application_signal_gtknotebook_removed (GtkNotebook *,
						     GtkNotebookPage *,
						     gint,
						     Workbook *);
static guint application_signal_gtknotebook_reordered (GtkNotebook *,
						       GtkNotebookPage *,
						       gint,
						       Workbook *);
static guint application_signal_gtksheet_changed (GtkWidget *,
						  gint, gint,
						  Sheet *);

/* @description: This method takes the argument and clears the string of
   everything except for the directories. Therefore the return value would
   be a string sans anything before the last directory delimiter '/'. */
static gchar *
munchpath (gchar * path_)
{
  gchar * path = g_strdup(path_);
  path = g_strreverse (path);
  gchar * p = path;

  while (p && (*p != '\0')) { if (*p == '/') break; p++; }

  gchar * str = g_strdup (p);
  str = g_strreverse (str);
  FREE (path);
  return str;
}

/* @description: This method creates and returns a new initialized 
   ApplicationState object. This should only be performed once.
   @argc: The argument count (passed in from commandline).
   @argv: The argument pointer to string array. */
ApplicationState *
application_init (int * argc, char *** argv)
{
  if (!g_thread_supported ())
    {
      g_thread_init (NULL);
      gdk_threads_init ();
    }

  ApplicationState * appstate = application_object_init ();
  int c;

  appstate->absolute_path = munchpath (*argv[0]);

  /* This block parses the commandline for options. A better example on how
     this code works can be found on the GNU website at the following URI:
     http://gnu.org/software/libtool/manual/libc/Using-Getopt.html */
  while ((c = getopt (*argc, *argv, "c:")) != -1)
    {
      switch (c)
	{
	case 'c':
	  {
	    Config * cfg = config_new (optarg);
	    if (!cfg)
	      {
		g_critical ("Failed loading configuration file '%s';" 
			    " which was specified with -c argument\n", optarg);
		exit (0);
		break;
	      }
	    appstate->cfg = cfg;
	  }
	  break;

	case '?':
	  {
	    if (optopt == 'c')
	      {
		g_warning ("Option -c requires an argument in order to load"
			   " a configuration file\n");
	      }
	  }
	  break;
	}
    }

  gdk_threads_enter ();
  gtk_init (argc, argv);

  /* Create the window and connect two callback to the signals. */
  appstate->gtk_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect (GTK_OBJECT (appstate->gtk_window),
		      "destroy",
		      G_CALLBACK (appstate->signals[SIG_DESTROY_EVENT]),
		      (gpointer *)appstate);
  gtk_signal_connect (GTK_OBJECT (appstate->gtk_window),
		      "delete_event",
		      G_CALLBACK (appstate->signals[SIG_DELETE_EVENT]),
		      NULL);
  
  /* Set the initial size of the application; we could load this
     from a configuration file eventually. */
  gtk_widget_set_usize (appstate->gtk_window, 1024, 768);
  
  /* Attach the window box to the window and present to the screen. */
  GtkWidget * window_box = gtk_vbox_new (FALSE, 1);
  gtk_container_add (GTK_CONTAINER (appstate->gtk_window), window_box);
  appstate->gtk_window_vbox = window_box;
  
  gtk_widget_show_all (appstate->gtk_window);
 
  gdk_threads_leave ();
  return appstate;
}

/* @description: This method destroys the application state object.
   @appstate: The pointer to the object to destroy. */
static void
application_method_close (ApplicationState *appstate)
{
  ASSERT (appstate != NULL);

  *appstate->shutdown = TRUE;
  
  DESTROY (Workbook, appstate->workbook_first);

  /* This has to be set so that we have a sentinel variable in separate threads
     if there is a better way to do this than it should be changed inside of
     the plugins that use these pointers to check if the object is still
     allocated. */
  appstate->workbook_first = appstate->workbook_last = NULL;

  application_object_free (appstate);
}

static guint
application_signal_gtknotebook_removed (GtkNotebook * notebook,
					GtkNotebookPage * page,
					gint page_num,
					Workbook * wb)
{
  ASSERT (wb != NULL);

  return TRUE;
}

static guint
application_signal_gtknotebook_reordered (GtkNotebook * notebook,
					  GtkNotebookPage * page,
					  gint page_num,
					  Workbook * wb)
{
  ASSERT (wb != NULL);

  return TRUE;
}

/* @description: This is the callback for the GtkNotebook 'switch-page' 
   signal. It is called every single time a user *clicks* on different
   tab. This method is called *before* the drawing takes place.

   This method iterates through the Sheets and updates where neccesary.

   @notebook: This is a pointer to the notebook object. The object is
   equal to the Workbook->gtk_notebook pointer.
   @page:
   @page_num: The number of the *new* page.
   @book: The Workbook object associated with the GtkNotebook.*/
static guint
application_signal_gtknotebook_switchpage (GtkNotebook * notebook,
					   GtkNotebookPage * page,
					   gint page_num,
					   Workbook * book)
{
  ASSERT (book != NULL);

  /* Perform the "unfocus" on the old notebook tab. */
  if (!IS_NULL (book->focus_sheet))
    {
      book->focus_sheet->has_focus = FALSE;
      book->focus_sheet->notices = 0;
    }

  GtkWidget * widget = gtk_notebook_get_nth_page (notebook, page_num);

  ITERATE_BEGIN (Sheet, book->sheet_first);
  {
    /* Once we find the right Sheet object we can perform what we need
       to in order to change the "focus." Finally, set book pointer. */
    if (it->gtk_box == widget)
      {
	it->page = page_num;
	it->has_focus = TRUE;
	it->notices = 0;
	
	/* Reset the label on the notebook tab to the object's name. */
	gtk_notebook_set_tab_label_text (notebook,
					 it->gtk_box,
					 it->name);
	book->focus_sheet = it;
	break;
      }
  }
  ITERATE_END ();
  return TRUE;
}

static guint
application_signal_gtksheet_changed (GtkWidget * gtksheet,
				     gint row, gint column,
				     Sheet * sheet)
{
  ASSERT (sheet != NULL);
  ASSERT (sheet->workbook != NULL);

  Workbook * wb = sheet->workbook;

  if (sheet->notices > 0)
    {
      gchar * label 
	= g_strdup_printf ("%s (%d)", sheet->name, sheet->notices);
      gtk_notebook_set_tab_label_text (GTK_NOTEBOOK (wb->gtk_notebook),
				       sheet->gtk_box,
				       label);
      FREE (label);
    }
  return FALSE;
}

/* @description: This method frees teh application state object.   
   @appstate: The application state object to free. */
static ApplicationState *
application_object_free (ApplicationState *appstate)
{
  ASSERT (appstate != NULL);

  FREE (appstate->shutdown);
  FREE (appstate->absolute_path);
  FREE (appstate);
  return appstate;
}

/* @description: This initializes a new application state object. This
   should only be called once. */
static ApplicationState *
application_object_init (void)
{
  ApplicationState * app = NEW (ApplicationState);

  /* Members */
  app->cfg = NULL;
  app->workbook_first = app->workbook_last = NULL;
  app->plugin_first = app->plugin_last = NULL;
  app->gtk_window = NULL;
  app->gtk_menu = NULL;
  app->gtk_window_vbox = NULL;
  app->shutdown = NEW (gboolean);
  *app->shutdown = FALSE;

  /* Set up the signals. */
  app->signals[SIG_NOTEBOOK_SWITCHED]
    = (GSourceFunc)application_signal_gtknotebook_switchpage;
  app->signals[SIG_NOTEBOOK_REORDERED]
    = (GSourceFunc)application_signal_gtknotebook_reordered;
  app->signals[SIG_NOTEBOOK_REMOVED]
    = (GSourceFunc)application_signal_gtknotebook_removed;
  app->signals[SIG_DESTROY_EVENT]
    = (GSourceFunc)application_signal_destroy_event;
  app->signals[SIG_DELETE_EVENT]
    = (GSourceFunc)application_signal_delete_event;
  app->signals[SIG_SHEET_CHANGED]
    = (GSourceFunc)application_signal_gtksheet_changed;

  /* Methods */
  app->run = application_method_run;
  app->close = application_method_close;
  app->load_plugin = application_method_loadplugin;
  app->open_extension = application_method_openextension;

  return app;
}

/* @description: This is called from main() when we want to start the
   application.It loads up the configuration file (if present) and starts
   gtk_main.
   @appstate: A pointer to the application state object. */
static int
application_method_run (ApplicationState *appstate)
{
  if (appstate->cfg)
    {
      Config * cfg = appstate->cfg;
      cfg->open (cfg);
     
      /* Load extensions */
      ConfigRow * load = cfg->get_row (cfg, "application", "load");

      /* Did a little bit of fixing here. We needed something to specify that
	 certain extensions will start automatically when the application is
	 started. */
      if (!IS_NULL (load))
	{
	  ConfigVector * ext = load->get_vector (load, "extensions");
	  gchar * block = NULL;
	  gint ii = 0;

	  if (IS_NULL (ext))
	    {
	      g_critical ("Config 'extensions' vector is NULL");
	      return -1;
	    }
      
	  while ((block = ext->get(ext, ii)) != NULL)
	    {
	      ConfigPair * run = cfg->get_pair (cfg, block, "onLoad", "run");
	      
	      if (run && (strcmp (run->value, "1") == 0))
		{
		  ConfigPair * filename 
		    = cfg->get_pair (cfg, block, "linux", "filename");

		  if (!IS_NULL (filename) && !IS_NULLSTR (filename->value))
		    {
		      gboolean RelativePath 
			= (filename->value[0] == '/') ? FALSE : TRUE;

		      appstate->open_extension (appstate, 
						filename->value, 
						RelativePath);
		    }
		}
	      ii++;
	    }
	}
    }

  /* Start the GTK+ main loop; make sure it is surrounded in the 
     thread calls. GTK+ "releases" the current lock after every loop
     interation. This allows us to call gdk_threads_enter/leave inside
     of another thread. */
  gdk_threads_enter ();
  gtk_main ();
  gdk_threads_leave ();
  return 0;
}

/* @description: This method loads a shared library (plugin) from disk.
   @appstate: A pointer to the application state object.
   @filename: The string to the file that we're looking to load.*/
static Plugin *
application_method_loadplugin (ApplicationState * appstate, 
			       const gchar * filename)
{
  ASSERT (appstate != NULL);
  
  Plugin * plugin = plugin_open (filename);
  
  if (plugin == NULL)
    return NULL;

  LINK_OBJECT (appstate->plugin_first, 
	       appstate->plugin_last, 
	       plugin);

  return plugin;
}

/* @description: This is the callback to GtkMain's "delete" event. This is
   called when we attempt to close the application safely. 
   @window: A pointer to the GtkWindow object.
   @event: A pointer to the associated GdkEvent information.
   @p: NULL */
static guint
application_signal_delete_event (GtkWindow * window, 
				 GdkEvent * event,
				 gpointer p)
{
  GtkWidget * dialog 
    = gtk_message_dialog_new (window, GTK_DIALOG_MODAL,
			      GTK_MESSAGE_QUESTION,
			      GTK_BUTTONS_YES_NO,
			      "Are you sure that you want to quit?");
  gtk_window_set_title (GTK_WINDOW (dialog), "Close Application");

  gint result = gtk_dialog_run (GTK_DIALOG (dialog));
  
  gtk_widget_destroy (dialog);

  return (result == GTK_RESPONSE_YES) ? FALSE : TRUE;
}

/* @description: This is the callback to the "destroy" signal that is
   emitted from GtkMain. Any cleanup should be done here.
   @window: A pointer to the GtkWindow object.
   @data: NULL */
static guint
application_signal_destroy_event (GtkWidget *window, gpointer data)
{
  ApplicationState * appstate = (ApplicationState *)data;
  *appstate->shutdown = TRUE;

  /* Because of a change to the Plugin architecture it will now yield until
     all threads that were instatiated are closed properly. We use the 
     appstate->workbook_first variable in order to test for NULL. */
  gdk_threads_leave();
  DESTROY (Plugin, appstate->plugin_first);
  gdk_threads_enter();

  appstate->plugin_first = appstate->plugin_last = NULL;

  gtk_main_quit ();
  return FALSE;
}

/* @description: This method loads an extension from the filename
   provided. It also registers the plugin's method and calls it.
   @app: A pointer to the application state object.
   @filename: The string that we should load the library from.
   @absolute_path: Is the string an absolute or relative path? */
static void
application_method_openextension (ApplicationState * app, 
				  const gchar * filename,
				  gboolean absolute_path)
{
  ASSERT (app != NULL);
  gchar * fname = NULL;
  
  if (absolute_path)
    {
      fname = g_strconcat (app->absolute_path, 
			   filename,
			   NULL);
    }
  else
    {
      fname = g_strdup (filename);
    }

  Plugin * plugin = NULL;
  if ((plugin = app->load_plugin (app, fname)) != NULL)
    {
      typedef Workbook * (*Plugin_Main) (ApplicationState *, Plugin *);
      Plugin_Main plugin_main;
	  
      if ((plugin_main 
	   = plugin->method_register (plugin, "plugin_main")) == NULL)
	{
	  g_critical ("Unable to register method with symbol 'plugin_main'");
	  exit (1);
	}

      Workbook * wb = plugin_main (app, plugin);
      if (wb == NULL)
	{
	  g_critical ("Plugin returned a NULL pointer instead of allocated"
		      " workbook.");
	  exit (1);
	}
      else
	{
	  gtk_box_pack_end (GTK_BOX (app->gtk_window_vbox),
			    wb->gtk_box, 1,1,1);

	  /* Attach all of the signals for the Workbook object. */
	  gtk_signal_connect (GTK_OBJECT (wb->gtk_notebook),
			      "switch-page",
			   (GtkSignalFunc)app->signals[SIG_NOTEBOOK_SWITCHED], 
			      (gpointer)wb);
	  
	  LINK_OBJECT (app->workbook_first, app->workbook_last, wb);
	}
    }

  if (absolute_path)
    FREE (fname);
}

