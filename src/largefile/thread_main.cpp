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
#include <workbook/workbook.h>
#include <config/config.h>
#include <concurrent/Thread.hpp>
#include <concurrent/ThreadArgs.hpp>
#include <concurrent/ScopedMemoryLock.hpp>
#include <proactor/Proactor.hpp>
#include <proactor/Event.hpp>
#include <gtkextra/gtksheet.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
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

	int vposition = std::abs((int)gtksheet->vadjustment->value);
	static long int cursor = 0;
	float N = vposition, K = 24388, V = (N/K);
	
	switch (event->keyval) {
		case GDK_F1: {
			fd->read(102583,1000);
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
	
void
thread_main (ThreadArgs * args) {
	Workbook * wb = (Workbook *)args->at(0);
	Config * cfg  = (Config *)args->at(1);
	gboolean * SHUTDOWN = (gboolean *)args->at(2);
	ConfigPair * logpath = cfg->get_pair (cfg, "largefile", "log", "path");
	if (IS_NULL (logpath)) {
		g_critical ("Failed loading log->path from configuration file; "
					"exiting thread");
		return;
	}

	FILE * pktlog = NULL;
	std::string logname = std::string (logpath->value).append("/");
	logname.append (append_pidname("largefile.").append(".log"));

	if ((pktlog = fopen (logname.c_str(), "w")) == NULL) {
		g_critical ("Failed opening file '%s' for packet logging; exiting"
					" thread", logname.c_str());
		return;
    }

	int fdEventId = proactor::Event::uniqueEventId(); 
	proactor::Proactor proactor;
	FileDispatcher fdispatcher (fdEventId, &proactor);
	CsvParser csv_parser (wb, pktlog, 0, 20);

	if (proactor.start() == false) {
		g_critical ("Failed starting Proactor; exiting thread.");
		return;
	}

	if (proactor.addWorker (fdEventId, &csv_parser) == false) {
		g_critical ("Failed starting CsvParser; exiting thread.");
		return;
	}

	if (fdispatcher.open ("/home/johnb/largefile.csv") == false) {
		g_critical ("Failed opening /home/johnb/largefile.csv");
		return;
	}

	if (fdispatcher.start() == false) {
		g_critical ("Failed starting file dispatcher; exiting thread.");
		return;
	}
	
	std::vector<gpointer> signal_arguments;
	signal_arguments.push_back( (gpointer)&fdispatcher );
	signal_arguments.push_back( (gpointer)wb );
	
	gtk_signal_connect (GTK_OBJECT (wb->gtk_window), "key_press_event",
							  GTK_SIGNAL_FUNC (key_press_callback),
							  (gpointer)&signal_arguments);

	while (*SHUTDOWN == FALSE) {
		concurrent::Thread::sleep (100);
	}

	csv_parser.stop();
  
	FCLOSE (pktlog);
	delete args;
}
