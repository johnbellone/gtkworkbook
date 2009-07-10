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
#include <proactor/Worker.hpp>
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
append_pidname (const gchar * pre) {
  std::stringstream s;
  s << pre << getppid();
  return s.str();
}

static gint
key_press_callback (GtkWidget * window, GdkEventKey * event, gpointer data) {
	std::vector<gpointer> * arguments = (std::vector<gpointer> *)data;
	FileDispatcher * fd = (FileDispatcher *)arguments->at(0);
	Workbook * wb = (Workbook *)arguments->at(1);
	GtkSheet * gtksheet = GTK_SHEET (wb->sheet_first->gtk_sheet);

	//	int vposition = std::abs((int)gtksheet->vadjustment->value);
	static off64_t cursor = 0;
	//	float N = vposition, K = 24388, V = (N/K);
	
	switch (event->keyval) {
		case GDK_F1: {
			fd->read(1012121,1000);
		}
		break;
		
		case GDK_Page_Up: {
			fd->read(cursor, 100);
			cursor += 100;
		}
		return TRUE;

		case GDK_Page_Down: {
			if (cursor <= 100)
				cursor = 0;
			else
				cursor -= 100;
			
			fd->read(cursor, 100);
		}
		return TRUE;
	}
	return FALSE;
}

Largefile::Largefile (Application * appstate, Handle * platform)
	: Plugin (appstate, platform) {

	ConfigPair * logpath =
		appstate->config()->get_pair (appstate->config(), "largefile", "log", "path");

	if (IS_NULL (logpath)) {
		g_critical ("Failed loading log->path from configuration file. Exiting application.");
		exit(1);
	}
	
	std::string logname = std::string (logpath->value).append("/");
	logname.append (append_pidname("largefile.").append(".log"));
	
	if ((pktlog = fopen (logname.c_str(), "w")) == NULL) {
		g_critical ("Failed opening file '%s' for packet logging; exiting"
					" thread", logname.c_str());
		return;
    }
}

bool
Largefile::open_file (const std::string & filename) {
	int fdEventId = proactor::Event::uniqueEventId();
	FileDispatcher * fd = new FileDispatcher (fdEventId, appstate->proactor());
	CsvParser * csv = new CsvParser (this->wb, this->pktlog, 0, 20);

	if (appstate->proactor()->addWorker (fdEventId, csv) == false) {
		g_critical ("Failed starting CsvParser for file %s", filename.c_str());
		return false;
	}

	if (fd->open (filename.c_str()) == false) {
		g_critical ("Failed opening %s", filename.c_str());
		return false;
	}

	if (fd->start() == false) {
		g_critical ("Failed starting file dispatcher for file %s", filename.c_str());
		return false;
	}

	this->mapping.insert (std::make_pair (filename, fdEventId));
	return true;
}

bool
Largefile::exit_file (const std::string & filename) {
	FilenameMap::iterator it = this->mapping.find(filename);
	if (it == this->mapping.end()) {
		// STUB: something meaningful here to mention that the file does not exist inside
		// of the map. They're trying to exit from a file that seemingly has not been opened.
		return false;
	}
	// STUB: procedure for shutting down a file dispatcher and CsvParser.
	this->mapping.erase (it);
	return true;
}
