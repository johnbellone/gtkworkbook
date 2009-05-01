/* 
   plugin_main.cpp - Plugin DLL Entry Point

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
#include <iostream>
#include <glib/gthread.h>
#include <workbook/workbook.h>
#include <concurrent/ThreadArgs.hpp>
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
    args.push_back( (void *)app->shutdown );

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
