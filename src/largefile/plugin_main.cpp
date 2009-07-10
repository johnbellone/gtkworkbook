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
#include <gtkworkbook/workbook.h>
#include <concurrent/ThreadArgs.hpp>
#include <gtk/gtk.h>
#include "Largefile.hpp"
#include "../config.h"
#include "../Application.hpp"

/* Prototypes */
extern void thread_main (ThreadArgs *);

static void
open_csv_file (GtkWidget * w, gpointer data) {
  Largefile * lf = (Largefile *)data;
  
  GtkWidget * dialog = gtk_file_chooser_dialog_new ("Open File",
																	 GTK_WINDOW (lf->app()->gtkwindow()),
																	 GTK_FILE_CHOOSER_ACTION_OPEN,
																	 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
																	 GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
																	 NULL);  
  
  gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
  
  if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
    gchar * filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

	 if (lf->open_file (filename) == true) {
		 std::cout << filename << "\n";
	 }
	 else {
		 // STUB: The opening of the file failed. Do something meaningful here.
	 }
	 
    g_free (filename);
  }

  gtk_widget_destroy (dialog);
}

static GtkWidget *
largefile_mainmenu_new (Application * appstate, GtkWidget * window) {
	GtkWidget * lfmenu = gtk_menu_new();
	GtkWidget * lfmenu_item = gtk_menu_item_new_with_label ("Largefile");
	GtkWidget * lfmenu_open = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (lfmenu), lfmenu_open);

	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lfmenu_item), lfmenu);

	gtk_widget_show_all (lfmenu);
	return lfmenu;
}

static GtkWidget *
build_layout (Application * app, Largefile * lf, Workbook * wb) {
	GtkWidget * gtk_menu = app->gtkmenu();
	GtkWidget * box = gtk_vbox_new (FALSE, 0);
	
	GtkWidget * largefile_menu = largefile_mainmenu_new (app, app->gtkwindow());
	gtk_menu_shell_append (GTK_MENU_SHELL (gtk_menu), largefile_menu);

	gtk_box_pack_end (GTK_BOX (box), wb->gtk_notebook, FALSE, FALSE, 0);

	wb->signals[SIG_WORKBOOK_CHANGED] = app->signals[Application::SHEET_CHANGED];
	wb->gtk_box = box;

	gtk_box_pack_start (GTK_BOX (app->gtkvbox()), box, FALSE, FALSE, 0);
	return box;
}

extern "C" {
  Plugin *
  plugin_main (Application * appstate, Handle * platform) {
    ASSERT (appstate != NULL);
    ASSERT (platform != NULL);
	 Largefile * lf = new Largefile (appstate, platform);
	 Workbook * wb = lf->workbook();
	 
    if ((wb = workbook_open (appstate->gtkwindow(), "largefile")) == NULL) {
      g_critical ("Failed opening workbook; exiting largefile plugin");
      return NULL;
    }
	 
	 GtkWidget * box = build_layout (appstate, lf, wb);
	 gtk_widget_show (box);	 
    return lf;
  }
} 
