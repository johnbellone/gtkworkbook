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
GotoDialogResponseCallback (GtkWidget * dialog, gint response, gpointer data) {
	Largefile * lf = (Largefile *)data;
	
	if (response == GTK_RESPONSE_OK) {
		GList * children = gtk_container_get_children ( GTK_CONTAINER (GTK_DIALOG(dialog)->vbox) );
		GtkWidget * entry = (GtkWidget *)g_list_nth_data (children, 0);
		const gchar * value = gtk_entry_get_text ( GTK_ENTRY (entry) );

		if (value && *value != '\0') {
			lf->read (lf->workbook()->focus_sheet, atol (value) - 1, 1000);
		}

		gtk_entry_set_text (GTK_ENTRY (entry), "");
	}
	
	gtk_widget_hide_all (dialog);
}

static gint
GtkKeypressCallback (GtkWidget * window, GdkEventKey * event, gpointer data) {
	static GtkWidget * goto_dialog = NULL;
	Largefile * lf = (Largefile *)data;
	Workbook * wb = lf->workbook();
	Sheet * sheet = wb->focus_sheet;

	// Only create the dialog the first time we run this method. 
	if (goto_dialog == NULL) {
		gdk_threads_enter ();
		
		goto_dialog = gtk_dialog_new_with_buttons ("Goto position ", GTK_WINDOW (window),
																 (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_NO_SEPARATOR),
																 GTK_STOCK_OK,
																 GTK_RESPONSE_OK,
																 GTK_STOCK_CANCEL,
																 GTK_RESPONSE_CANCEL,
																 NULL);
		
		GtkWidget * box = GTK_DIALOG (goto_dialog)->vbox;
		GtkWidget * entry = gtk_entry_new_with_max_length (30);

		gtk_box_set_spacing (GTK_BOX (box), 18);
			
		gtk_box_pack_start (GTK_BOX (box), entry, TRUE, TRUE, 0);
				
		gtk_widget_show_all (box);

		gdk_threads_leave();
		
		g_signal_connect (G_OBJECT (goto_dialog), "response", G_CALLBACK (GotoDialogResponseCallback), lf);
		
		g_signal_connect (G_OBJECT (goto_dialog), "delete-event",
								G_CALLBACK (gtk_widget_hide_on_delete), NULL);
		
	}	
	
	//	int vposition = std::abs((int)gtksheet->vadjustment->value);
	static off64_t cursor = 0;
	//	float N = vposition, K = 24388, V = (N/K);
	
	switch (event->keyval) {
		case GDK_F1: {
			if (sheet != NULL) {
				gdk_threads_enter();
				gtk_widget_show_all (goto_dialog);
				gdk_threads_leave();
			}
		}
		break;
		
		case GDK_Page_Up: {
			lf->read(sheet, cursor, 100);
			cursor += 100;
		}
		return TRUE;

		case GDK_Page_Down: {
			if (cursor <= 100)
				cursor = 0;
			else
				cursor -= 100;
			
			lf->read(sheet, cursor, 100);
		}
		return TRUE;
	}
	return FALSE;
}

Largefile::Largefile (Application * appstate, Handle * platform)
	: Plugin (appstate, platform) {

	this->wb = workbook_open (appstate->gtkwindow(), "largefile");
	
	ConfigPair * logpath =
		appstate->config()->get_pair (appstate->config(), "largefile", "log", "path");

	if (IS_NULL (logpath)) {
		g_critical ("Failed loading log->path from configuration file. Exiting application.");
		exit(1);
	}
	
	std::string logname = std::string (logpath->value).append("/");
	logname.append (AppendProcessId("largefile.").append(".log"));
	
	if ((pktlog = fopen (logname.c_str(), "w")) == NULL) {
		g_critical ("Failed opening file '%s' for packet logging; exiting"
					" thread", logname.c_str());
		return;
    }
	
	gtk_signal_connect (GTK_OBJECT (this->wb->gtk_window), "key_press_event",
							  GTK_SIGNAL_FUNC (GtkKeypressCallback), this);
}

Largefile::~Largefile (void) {
}

bool
Largefile::read (Sheet * sheet, off64_t start, off64_t N) {
	this->lock();
	std::string key = sheet->name;
	
	FilenameMap::iterator it = this->mapping.find (key);
	if (it == this->mapping.end()) {
		this->unlock();
		return false;
	}

	FileDispatcher * fd = it->second;
	fd->read (start, N);
	this->unlock();
	return true;
}

bool
Largefile::open_file (Sheet * sheet, const std::string & filename) {
	this->lock();
	
	int fdEventId = proactor::Event::uniqueEventId();
	FileDispatcher * fd = new FileDispatcher (fdEventId, appstate->proactor());
	CsvParser * csv = new CsvParser (sheet, this->pktlog, 0, 20);

	if (appstate->proactor()->addWorker (fdEventId, csv) == false) {
		g_critical ("Failed starting CsvParser for file %s", filename.c_str());
		this->unlock();
		return false;
	}

	if (fd->open (filename) == false) {
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
Largefile::exit_file (const std::string & filename) {
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
