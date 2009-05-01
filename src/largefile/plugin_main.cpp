/* 
   plugin_main.cpp - Largefile Plugin DLL Entry Point

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
#include <workbook/workbook.h>
#include <glib/gthread.h>
#include "../application.h"
#include "../plugin.h"

extern "C" {

  Workbook *
  plugin_main (ApplicationState * appstate, Plugin * plugin) {
    ASSERT (appstate != NULL);
    ASSERT (plugin != NULL);

    Workbook * wb = NULL;
    GtkWidget * hbox = gtk_handle_box_new();
    
    if ((wb = workbook_open (appstate->gtk_window, "largefile")) == NULL) {
      g_critical ("Failed opening workbook; exiting largefile plugin");
      return NULL;
    }

    wb->signals[SIG_WORKBOOK_CHANGED] = appstate->signals[SIG_SHEET_CHANGED];
    
    gtk_container_add (GTK_CONTAINER (hbox), wb->gtk_notebook);
    
    wb->gtk_box = hbox;
    
    wb->add_new_sheet (wb, "sheet0", 100, 15);

    gtk_widget_show (hbox);
    return wb;
  }

}
