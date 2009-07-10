/* 
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
#include <concurrent/ThreadArgs.hpp>
#include <gtk/gtk.h>
#include "../application.h"
#include "../plugin.h"

/* Prototypes */
extern void thread_main (ThreadArgs *);

static void
exit_application (GtkWidget * w, gpointer data) {
	ApplicationState * app = (ApplicationState *)data;
	app->exit_application(app);
}

static void
open_csv_file (GtkWidget * w, gpointer data) {
  ApplicationState * app = (ApplicationState *)data;
   
  GtkWidget * dialog = gtk_file_chooser_dialog_new ("Open File",
						    GTK_WINDOW (app->gtk_window),
						    GTK_FILE_CHOOSER_ACTION_OPEN,
						    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						    GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						    NULL);  
  
  gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
    gchar * filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

	 
	 
    g_free (filename);
  }

  gtk_widget_destroy (dialog);
}

static GtkWidget *
largefile_mainmenu_new (ApplicationState * appstate, GtkWidget * window) {
	GtkWidget * menubar = gtk_menu_bar_new();
	//GtkWidget * lfmenu = gtk_menu_new (); 
	GtkWidget * lfmenu_item = gtk_menu_item_new_with_label ("Largefile");
	GtkWidget * lfmenu_open = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, NULL);
	GtkWidget * lfmenu_exit = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, NULL);

	gtk_menu_shell_append (GTK_MENU_SHELL (menubar), lfmenu_open);
	gtk_menu_shell_append (GTK_MENU_SHELL (menubar), lfmenu_exit);
	
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lfmenu_item), menubar);
	
	g_signal_connect (G_OBJECT (lfmenu_open), "activate",
							G_CALLBACK (open_csv_file), (gpointer)appstate);
	
	g_signal_connect (G_OBJECT (lfmenu_exit), "activate",
							G_CALLBACK (exit_application), (gpointer)appstate);

	//gtk_container_add (GTK_CONTAINER (menubar), lfmenu);

	gtk_widget_show_all (menubar);
	return menubar;
}

static GtkWidget *
build_layout (ApplicationState * appstate, GtkWidget * gtk_window) {
	GtkWidget * box = gtk_vbox_new (FALSE, 0);
	GtkWidget * mainmenu = largefile_mainmenu_new (appstate, gtk_window);
	gtk_box_pack_start (GTK_BOX (box), mainmenu, FALSE, FALSE, 0);
	return box;
}

extern "C" {

  Workbook *
  plugin_main (ApplicationState * appstate, Plugin * plugin) {
    ASSERT (appstate != NULL);
    ASSERT (plugin != NULL);
	
    GtkWidget * box = build_layout (appstate, appstate->gtk_window);
	 Workbook * wb = NULL;
    if ((wb = workbook_open (appstate->gtk_window, "largefile")) == NULL) {
      g_critical ("Failed opening workbook; exiting largefile plugin");
      return NULL;
    }

    gtk_box_pack_end (GTK_BOX (box), wb->gtk_notebook, FALSE, FALSE, 0);

    wb->signals[SIG_WORKBOOK_CHANGED] = appstate->signals[SIG_SHEET_CHANGED];
        
    wb->gtk_box = box;
    
    wb->add_new_sheet (wb, "sheet0", 1000, 20);

    ThreadArgs args;
    args.push_back ( (void *)wb );
    args.push_back ( (void *)appstate->cfg );
    args.push_back ( (void *)appstate->shutdown );

    if (plugin->create_thread (plugin,
			       (GThreadFunc)thread_main,
			       (gpointer)new ThreadArgs (args)) == NULL) {
      g_critical ("Failed creating thread; exiting 'largefile' plugin");
      return NULL;
    }

    gtk_box_pack_start (GTK_BOX (appstate->gtk_window_vbox), box, FALSE, FALSE, 0);
    gtk_widget_show_all (box);	
    return wb;
  }

} 
