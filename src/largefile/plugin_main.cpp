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
#include "../config.h"
#include "../Application.hpp"
#include "../plugin.h"

/* Prototypes */
extern void thread_main (ThreadArgs *);

static void
open_csv_file (GtkWidget * w, gpointer data) {
  Application * app = (Application *)data;
  Config * cfg = app->config();
  
  GtkWidget * dialog = gtk_file_chooser_dialog_new ("Open File",
																	 GTK_WINDOW (app->gtkwindow()),
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
largefile_mainmenu_new (Application * appstate, GtkWidget * window) {
  GtkItemFactoryEntry menu_items[] = {
    {"/_File",         NULL,		NULL, 		0, 	"<Branch>"},
    {"/File/_Open",    "<CTRL>O",	(GtkItemFactoryCallback)open_csv_file,	0,	"<StockItem>", GTK_STOCK_OPEN},
    {"/File/_Close",   "<CTRL>C",	NULL,		0,	"<Item>"},
    {"/File/sep1",     NULL,		NULL,		0,	"<Separator>"},
    {"/File/_Quit",    "<CTRL>Q", 	gtk_main_quit,	0,	"<StockItem>", GTK_STOCK_QUIT},
  };
  
  gint nmenu_items = sizeof (menu_items) / sizeof (menu_items[0]);
  GtkAccelGroup * accel 
    = gtk_accel_group_new ();
	
  GtkItemFactory * item_factory 
    = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel);
  
  gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, appstate);

  gtk_window_add_accel_group (GTK_WINDOW (window), accel);

  GtkWidget * menu = gtk_item_factory_get_widget (item_factory, "<main>");
  gtk_widget_show (menu);

  return menu;
}

extern "C" {

  Workbook *
  plugin_main (Application * appstate, Plugin * plugin) {
    ASSERT (appstate != NULL);
    ASSERT (plugin != NULL);
	
    GtkWidget * box = gtk_vbox_new (FALSE, 0);
    GtkWidget * mainmenu = largefile_mainmenu_new (appstate, appstate->gtkwindow());
    gtk_box_pack_start (GTK_BOX (box), mainmenu, FALSE, FALSE, 0);

    Workbook * wb = NULL;
    if ((wb = workbook_open (appstate->gtkwindow(), "largefile")) == NULL) {
      g_critical ("Failed opening workbook; exiting largefile plugin");
      return NULL;
    }

    gtk_box_pack_end (GTK_BOX (box), wb->gtk_notebook, FALSE, FALSE, 0);

    wb->signals[SIG_WORKBOOK_CHANGED] = appstate->signals[Application::SHEET_CHANGED];
        
    wb->gtk_box = box;
    
    wb->add_new_sheet (wb, "sheet0", 1000, 20);

    ThreadArgs args;
    args.push_back ( (void *)wb );
    args.push_back ( (void *)appstate->config() );

    if (plugin->create_thread (plugin,
			       (GThreadFunc)thread_main,
			       (gpointer)new ThreadArgs (args)) == NULL) {
      g_critical ("Failed creating thread; exiting 'largefile' plugin");
      return NULL;
    }

    gtk_box_pack_start (GTK_BOX (appstate->gtkvbox()), box, FALSE, FALSE, 0);
    gtk_widget_show (box);	
    return wb;
  }

} 
