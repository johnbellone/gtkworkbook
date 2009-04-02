#include <iostream>
#include <glib/gthread.h>
#include <workbook/workbook.h>
#include "concurrent/ThreadArgs.hpp"
#include "../application.h"
#include "../plugin.h"

/* Prototypes */
extern void thread_main (ThreadArgs *);

extern "C"
{
  Workbook *
  plugin_main (ApplicationState * app, Plugin * plugin)
  {
    ASSERT (app != NULL);
    ASSERT (plugin != NULL);

    Workbook * wb = NULL;
    GtkWidget * hbox = gtk_handle_box_new ();
    
    if ((wb = workbook_open (app->gtk_window, "realtime")) == NULL)
      {
	g_critical ("Failed opening workbook; exiting plugin");
	return NULL;
      }

    wb->signals[SIG_WORKBOOK_CHANGED] = app->signals[SIG_SHEET_CHANGED];

    gtk_container_add (GTK_CONTAINER (hbox), wb->gtk_notebook);
 
    wb->gtk_box = hbox;
    
    wb->add_new_sheet (wb, "sheet0", 100, 11);

    ThreadArgs args;
    args.push_back( (void *)wb );
    args.push_back( (void *)app->cfg );

    if (plugin->create_thread (plugin, 
			       (GThreadFunc)thread_main,
			       (gpointer)new ThreadArgs (args)
			       ) == NULL)
      {
	g_critical ("Failed creating thread; exiting plugin");
	return NULL;
      }
   
    gtk_widget_show (hbox);
    return wb;
  }
}
