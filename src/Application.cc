#include "Application.hpp"

static guint
signal_gtknotebook_removed (GtkNotebook * notebook,
									 GtkNotebookPage * page,
									 gint page_num,
									 Workbook * wb) {
	ASSERT (wb != NULL);
	return TRUE;
}

static guint
signal_gtknotebook_reordered (GtkNotebook * notebook,
										GtkNotebookPage * page,
										gint page_num,
										Workbook * wb) {
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
signal_gtknotebook_switchpage (GtkNotebook * notebook,
										 GtkNotebookPage * page,
										 gint page_num,
										 Workbook * book) {
	ASSERT (book != NULL);

	/* Perform the "unfocus" on the old notebook tab. */
	if (!IS_NULL (book->focus_sheet)) {
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
signal_gtksheet_changed (GtkWidget * gtksheet,
								 gint row, gint column,
								 Sheet * sheet) {
	ASSERT (sheet != NULL);
	ASSERT (sheet->workbook != NULL);

	Workbook * wb = sheet->workbook;

	if (sheet->notices > 0) {
      gchar * label 
			= g_strdup_printf ("%s (%d)", sheet->name, sheet->notices);
      gtk_notebook_set_tab_label_text (GTK_NOTEBOOK (wb->gtk_notebook),
													sheet->gtk_box,
													label);
      FREE (label);
	}
	return FALSE;
}

/* @description: This is the callback to GtkMain's "delete" event. This is
   called when we attempt to close the application safely. 
   @window: A pointer to the GtkWindow object.
   @event: A pointer to the associated GdkEvent information.
   @p: NULL */
static guint
signal_delete_event (GtkWindow * window, 
											GdkEvent * event,
											gpointer p) {
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
signal_destroy_event (GtkWidget *window, gpointer data)
{
	Application * app = (Application *)data;
	app->shutdown();
	return FALSE;
}

Application::Application (int argc, char ** argv) {
	this->init (argc, argv);
	this->cfg = NULL;
	this->gtk_window = NULL;
	this->gtk_menu = NULL;
	this->gtk_window_vbox = NULL;
	
	/* Set up the signals. */
	this->signals[NOTEBOOK_SWITCHED]
		= (GSourceFunc)signal_gtknotebook_switchpage;
	this->signals[NOTEBOOK_REORDERED]
		= (GSourceFunc)signal_gtknotebook_reordered;
	this->signals[NOTEBOOK_REMOVED]
		= (GSourceFunc)signal_gtknotebook_removed;
	this->signals[DESTROY_EVENT]
		= (GSourceFunc)signal_destroy_event;
	this->signals[DELETE_EVENT]
		= (GSourceFunc)signal_delete_event;
	this->signals[SHEET_CHANGED]
		= (GSourceFunc)signal_gtksheet_changed;
}

Application::~Application (void) {
	/* Because of a change to the Plugin architecture it will now yield until
		all threads that were instatiated are closed properly. We use the 
		appstate->workbook_first variable in order to test for NULL. */
	gdk_threads_leave();
	{
		PluginList::iterator it = this->plugins.begin();
		while (it != this->plugins.end()) {
			(*it)->destroy ( (*it) ); (*it) = NULL;
			it++;
		}
	}
	gdk_threads_enter();

	WorkbookList::iterator it = this->workbooks.begin();
	while (it != this->workbooks.end()) {
		(*it)->destroy ( (*it) ); (*it) = NULL;
		it++;
	}
	
	FREE (this->absolute_path);

	this->shutdown();
}

Plugin *
Application::load_plugin (const gchar * filename) {
	Plugin * plugin = plugin_open (filename);

	if (plugin) {
		this->plugins.push_back (plugin);
	}
	return plugin;
}

void
Application::shutdown(void) {
	// STUB: shutdown the proactor threads here.
	gtk_main_quit ();
}

void
Application::open_extension (const gchar * filename, gboolean absolute_path) {
	gchar * fname = NULL;

	if (absolute_path) {
		fname = g_strconcat (this->absolute_path, filename, NULL);
	}
	else {
		fname = g_strdup (filename);
	}

	Plugin * plugin = NULL;
	if ((plugin = this->load_plugin (fname)) != NULL) {
		typedef Workbook * (*Plugin_Main) (ApplicationState *, Plugin *);
      Plugin_Main plugin_main;
	  
      if ((plugin_main = plugin->method_register (plugin, "plugin_main")) == NULL) {
			g_critical ("Unable to register method with symbol 'plugin_main'");
			exit (1);
		}

      Workbook * wb = plugin_main (this, plugin);
      if (wb == NULL) {
			g_critical ("Plugin returned a NULL pointer instead of allocated"
							" workbook.");
			exit (1);
		} else {
			/* Attach all of the signals for the Workbook object. */
			gtk_signal_connect (GTK_OBJECT (wb->gtk_notebook),
									  "switch-page",
									  (GtkSignalFunc)this->signals[SIG_NOTEBOOK_SWITCHED], 
									  (gpointer)wb);
	  
			this->workbooks.push_back (wb);
		}
	}

	FREE (fname);
}

void
Application::init (int argc, char ** argv) {
	int c;
	
	if (!g_thread_supported ()) {
      g_thread_init (NULL);
      gdk_threads_init ();
	}

	this->absolute_path = munchpath (*argv[0]);
	
	/* This block parses the commandline for options. A better example on how
		this code works can be found on the GNU website at the following URI:
		http://gnu.org/software/libtool/manual/libc/Using-Getopt.html */
	while ((c = getopt (*argc, *argv, "c:")) != -1) {
      switch (c) {
			case 'c': {
				Config * cfg = config_new (optarg);
				if (!cfg)
				{
					g_critical ("Failed loading configuration file '%s';" 
									" which was specified with -c argument\n", optarg);
					exit (0);
					break;
				}
				this->cfg = cfg;
			}
			break;

			case '?': {
				if (optopt == 'c') {
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
	this->gtk_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_signal_connect (GTK_OBJECT (this->gtk_window),
							  "destroy",
							  G_CALLBACK (this->signals[DESTROY_EVENT]),
							  (gpointer *)this);
	gtk_signal_connect (GTK_OBJECT (this->gtk_window),
							  "delete_event",
							  G_CALLBACK (this->signals[DELETE_EVENT]),
							  NULL);
  
	/* Set the initial size of the application; we could load this
		from a configuration file eventually. */
	gtk_widget_set_usize (this->gtk_window, 1024, 768);
  
	/* Attach the window box to the window and present to the screen. */
	GtkWidget * window_box = gtk_vbox_new (FALSE, 1);
	gtk_container_add (GTK_CONTAINER (this->gtk_window), window_box);
	this->gtk_window_vbox = window_box;
  
	gtk_widget_show_all (this->gtk_window);
 
	gdk_threads_leave ();
	
	if (this->cfg) {
		Config * cfg = this->cfg;
      cfg->open (cfg);
     
      /* Load extensions */
      ConfigRow * load = cfg->get_row (cfg, "application", "load");

      /* Did a little bit of fixing here. We needed something to specify that
			certain extensions will start automatically when the application is
			started. */
      if (!IS_NULL (load)) {
			ConfigVector * ext = load->get_vector (load, "extensions");
			gchar * block = NULL;
			gint ii = 0;

			if (IS_NULL (ext)) {
				g_critical ("Config 'extensions' vector is NULL");
				return -1;
			}
      
			while ((block = ext->get(ext, ii)) != NULL) {
				ConfigPair * run = cfg->get_pair (cfg, block, "onLoad", "run");
	      
				if (run && (strcmp (run->value, "1") == 0)) {
					ConfigPair * filename 
						= cfg->get_pair (cfg, block, "linux", "filename");

					if (!IS_NULL (filename) && !IS_NULLSTR (filename->value)) {
						gboolean RelativePath 
							= (filename->value[0] == '/') ? FALSE : TRUE;

						this->open_extension (filename->value, RelativePath);
					}
				}
				ii++;
			}
		}
	}
}

int
Application::run (void) {
	if (proactor.start() == false) {
		g_critical ("Failed to start the proactor thread; exiting application.");
		return -1;
	}
	
	/* Start the GTK+ main loop; make sure it is surrounded in the 
		thread calls. GTK+ "releases" the current lock after every loop
		interation. This allows us to call gdk_threads_enter/leave inside
		of another thread. */
	gdk_threads_enter ();
	gtk_main();
	gdk_threads_leave ();
	return 0;
}

