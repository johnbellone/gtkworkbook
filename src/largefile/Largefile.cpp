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
#include <gdk/gdkkeysyms.h>
#include <gtkworkbook/workbook.h>
#include <proactor/Proactor.hpp>
#include <proactor/Event.hpp>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "GotoDialog.hpp"
#include "Largefile.hpp"
#include "File.hpp"
#include "CsvParser.hpp"

using namespace largefile;

/* @description: This method creates a filename with the prefix supplied and
   uses the pid of the process as its suffix. 
   @pre: The prefix (should be a file path, obviously). */
static std::string
AppendProcessId (const gchar * pre) {
  std::stringstream s;
  s << pre << getppid();
  return s.str();
}

static void
GotoDialogResponseCallback (GtkWidget * gtkdialog, gint response, gpointer data) {
	GotoDialog * dialog = (GotoDialog *)data;
	
	if (response == GTK_RESPONSE_OK) {
		GList * children = gtk_container_get_children ( GTK_CONTAINER (GTK_DIALOG(gtkdialog)->vbox) );
		GtkWidget * entry = (GtkWidget *)g_list_nth_data (children, 1);
		const char * entry_value = gtk_entry_get_text ( GTK_ENTRY (entry) ); 
				
		if (entry_value && *entry_value != '\0') {
			long long value = atoll (entry_value);
						
			switch (dialog->active_index) {
				// byte offset
				case 0: {
					dialog->lf->Readoffset (dialog->lf->workbook()->focus_sheet,
													value,
													1000);
				}
				break;

				// absolute line
				case 1: {
					dialog->lf->Readline (dialog->lf->workbook()->focus_sheet,
												 value,
												 1000);
				}
				break;

				// relative percentage
				case 2: {
					long long perc_value = (long long) (atof ( entry_value ) * 10);
					
					dialog->lf->Readpercent (dialog->lf->workbook()->focus_sheet,
													 perc_value * 100,
													 1000);
				}
				break;
			}
		}

		gtk_entry_set_text (GTK_ENTRY (entry), "");
	}
	
	gtk_widget_hide_all (gtkdialog);
}

static void
GotoDialogRadioToggleCallback (GtkToggleButton * button, gpointer data) {
	GtkWidget * widget = GTK_WIDGET (button);
	GotoDialog * dialog = (GotoDialog *)data;

	if (GTK_TOGGLE_BUTTON (button)->active) {
		if (dialog->radio_byte.widget == widget) {
			dialog->active_index = 0;
		}
		else if (dialog->radio_line.widget == widget) {
			dialog->active_index = 1;
		}
		else if (dialog->radio_perc.widget == widget) {
			dialog->active_index = 2;
		}
	}
}

static void
CsvOpenDialogCallback (GtkWidget * w, gpointer data) {
	GtkWidget * open_dialog = NULL;
	Largefile * lf = (Largefile *)data;
	open_dialog = gtk_file_chooser_dialog_new ("Open CSV File",
															 GTK_WINDOW (lf->app()->gtkwindow()),
															 GTK_FILE_CHOOSER_ACTION_OPEN,
															 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
															 GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
															 NULL);  
  
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (open_dialog), TRUE);
				
	if (gtk_dialog_run (GTK_DIALOG (open_dialog)) == GTK_RESPONSE_ACCEPT) {
		gchar * filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (open_dialog));
		Sheet * sheet = lf->workbook()->add_new_sheet (lf->workbook(), filename, 1000, 20);

		if (sheet == NULL) {
			g_warning ("Failed adding new sheet because one already exists");
		}
		else {
			if (lf->OpenFile (sheet, filename) == true) {
				// STUB: Do something magical.
			}
			else {
				// STUB: The opening of the file failed. Do something meaningful here.
			}
		}
		
		g_free (filename);
	}

	gtk_widget_destroy (open_dialog);
}

static gint
GtkKeypressCallback (GtkWidget * window, GdkEventKey * event, gpointer data) {
	gint result = FALSE;
	Largefile * lf = (Largefile *)data;
	GotoDialog * dialog = lf->gotodialog();
	Workbook * wb = lf->workbook();
	Sheet * sheet = wb->focus_sheet;

	// Only create the dialog the first time we run this method. 
	if (dialog->widget == NULL) {
		dialog->lf = lf;
		
		dialog->widget = gtk_dialog_new_with_buttons ("Goto position ", GTK_WINDOW (window),
																	(GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR),
																	GTK_STOCK_OK,
																	GTK_RESPONSE_OK,
																	GTK_STOCK_CANCEL,
																	GTK_RESPONSE_CANCEL,
																	NULL);

		GtkWidget * gtk_frame = gtk_frame_new ("Jump Options");
		GtkWidget * gtk_hbox = gtk_hbox_new (FALSE, 0);
		GtkWidget * gtk_radiobyte = gtk_radio_button_new_with_label (NULL, "Offset");
		GtkWidget * gtk_radioline = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gtk_radiobyte),
																						  "Line");
		GtkWidget * gtk_radioperc = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (gtk_radiobyte),
																						  "Percent");
		GtkWidget * box = GTK_DIALOG (dialog->widget)->vbox;
		GtkWidget * entry = gtk_entry_new_with_max_length (30);

		// Set the GotoDialog RadioButton objects to the proper pointers.
		dialog->radio_byte.widget = gtk_radiobyte;
		dialog->radio_line.widget = gtk_radioline;
		dialog->radio_perc.widget = gtk_radioperc;
		
		gtk_container_add (GTK_CONTAINER (gtk_hbox), gtk_radiobyte);
		gtk_container_add (GTK_CONTAINER (gtk_hbox), gtk_radioline);
		gtk_container_add (GTK_CONTAINER (gtk_hbox), gtk_radioperc);
		gtk_container_add (GTK_CONTAINER (gtk_frame), gtk_hbox);

		gtk_box_set_spacing (GTK_BOX (box), 18);
			
		gtk_box_pack_start (GTK_BOX (box), gtk_frame, TRUE, TRUE, 0);
		gtk_box_pack_end (GTK_BOX (box), entry, TRUE, TRUE, 0);
								
		gtk_widget_show_all (box);

		// Connect the signals to we can do fancy switching.
		g_signal_connect (G_OBJECT (gtk_radiobyte), "toggled",
								G_CALLBACK (GotoDialogRadioToggleCallback),
								dialog);
		g_signal_connect (G_OBJECT (gtk_radioline), "toggled",
								G_CALLBACK (GotoDialogRadioToggleCallback),
								dialog);
		g_signal_connect (G_OBJECT (gtk_radioperc), "toggled",
								G_CALLBACK (GotoDialogRadioToggleCallback),
								dialog);
		g_signal_connect (G_OBJECT (dialog->widget), "response",
								G_CALLBACK (GotoDialogResponseCallback), dialog);
		
		g_signal_connect (G_OBJECT (dialog->widget), "delete-event",
								G_CALLBACK (gtk_widget_hide_on_delete), NULL);
	}	
	
	switch (event->keyval) {
		case GDK_F1: {
			if (sheet != NULL) {
				gtk_widget_show_all (dialog->widget);
			}
		}
		break;
	}
	return result;
}

Largefile::Largefile (Application * appstate, Handle * platform)
	: Plugin (appstate, platform) {

	this->wb = workbook_open (appstate->gtkwindow(), "largefile");
	this->gtk_togglegroup = NULL;
	
	ConfigPair * logpath =
		appstate->config()->get_pair (appstate->config(), "largefile", "log", "path");

	if (IS_NULL (logpath)) {
		g_critical ("Failed loading log->path from configuration file. Exiting application.");
		exit(1);
	}
	
	std::string logname = std::string (logpath->value).append("/");
	logname.append (AppendProcessId("largefile.").append(".log"));
	
	if ((pktlog = fopen (logname.c_str(), "w")) == NULL) {
		g_critical ("Failed opening file '%s' for packet logging", logname.c_str());
    }
	
	gtk_signal_connect (GTK_OBJECT (this->wb->gtk_window), "key_press_event",
							  GTK_SIGNAL_FUNC (GtkKeypressCallback), this);
}

Largefile::~Largefile (void) {
	FCLOSE (pktlog);
}

GtkWidget *
Largefile::CreateMainMenu (void) {
	GtkWidget * lfmenu = gtk_menu_new();
	GtkWidget * lfmenu_item = gtk_menu_item_new_with_label ("Largefile");
	GtkWidget * lfmenu_open = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, NULL);
	gtk_menu_shell_append (GTK_MENU_SHELL (lfmenu), lfmenu_open);

	g_signal_connect (G_OBJECT (lfmenu_open), "activate",
							G_CALLBACK (CsvOpenDialogCallback), this);
	
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (lfmenu_item), lfmenu);
	return lfmenu_item;
}

GtkWidget *
Largefile::BuildLayout (void) {
	GtkWidget * gtk_menu = this->app()->gtkmenu();
	GtkWidget * box = gtk_vbox_new (FALSE, 0);
	GtkWidget * largefile_menu = this->CreateMainMenu();
	
	gtk_menu_shell_append (GTK_MENU_SHELL (gtk_menu), largefile_menu);

	wb->signals[SIG_WORKBOOK_CHANGED] = this->app()->signals[Application::SHEET_CHANGED];
	wb->gtk_box = box;

	gtk_box_pack_start (GTK_BOX (box), wb->gtk_notebook, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (this->app()->gtkvbox()), box, FALSE, FALSE, 0);
	return box;
}

bool
Largefile::Readline (Sheet * sheet, off64_t start, off64_t N) {
	this->lock();
	std::string key = sheet->name;
	
	FilenameMap::iterator it = this->mapping.find (key);
	if (it == this->mapping.end()) {
		this->unlock();
		return false;
	}

	FileDispatcher * fd = it->second;
	bool result = fd->Readline (start, N);
	this->unlock();
	return result;
}

bool
Largefile::Readoffset (Sheet * sheet, off64_t offset, off64_t N) {
	this->lock();
	std::string key = sheet->name;
	
	FilenameMap::iterator it = this->mapping.find (key);
	if (it == this->mapping.end()) {
		this->unlock();
		return false;
	}

	FileDispatcher * fd = it->second;
	bool result = fd->Readoffset (offset, N);
	this->unlock();
	return result;
}

bool
Largefile::Readpercent (Sheet * sheet, guint percent, off64_t N) {
	this->lock();
	std::string key = sheet->name;
	
	FilenameMap::iterator it = this->mapping.find (key);
	if (it == this->mapping.end()) {
		this->unlock();
		return false;
	}

	FileDispatcher * fd = it->second;
	bool result = fd->Readpercent (percent, N);
	this->unlock();
	return result;
}

bool
Largefile::OpenFile (Sheet * sheet, const std::string & filename) {
	this->lock();
	
	int fdEventId = proactor::Event::uniqueEventId();
	FileDispatcher * fd = new FileDispatcher (fdEventId, appstate->proactor());
	CsvParser * csv = new CsvParser (sheet, this->pktlog, 0, 20);

	if (appstate->proactor()->addWorker (fdEventId, csv) == false) {
		g_critical ("Failed starting CsvParser for file %s", filename.c_str());
		this->unlock();
		return false;
	}

	if (fd->Openfile (filename) == false) {
		g_critical ("Failed opening %s", filename.c_str());
		this->unlock();
		return false;
	}

	if (fd->start() == false) {
		g_critical ("Failed starting file dispatcher for file %s", filename.c_str());
		this->unlock();
		return false;
	}

	this->mapping.insert (std::make_pair (std::string(filename), fd));
	
	this->unlock();
	return true;
}

bool
Largefile::CloseFile (const std::string & filename) {
	this->lock();
	
	FilenameMap::iterator it = this->mapping.find(filename);
	if (it == this->mapping.end()) {
		// STUB: something meaningful here to mention that the file does not exist inside
		// of the map. They're trying to exit from a file that seemingly has not been opened.
		this->unlock();
		return false;
	}
	// STUB: procedure for shutting down a file dispatcher and CsvParser.
	this->mapping.erase (it);

	this->unlock();
	return true;
}
