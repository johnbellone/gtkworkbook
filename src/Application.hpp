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
#ifndef HPP_APPLICATION
#define HPP_APPLICATION

#include <string>
#include <list>
#include <cstdio>
#include <cstdlib>
#include <libgtkworkbook/workbook.h>
#include "config.h"
#include "proactor/Proactor.hpp"
#include "AboutDialog.hpp"

class Plugin;
class RecordView;

class Application {
public:
	enum sigs {
		DESTROY_EVENT = 0,
		DELETE_EVENT,
		NOTEBOOK_SWITCHED,
		NOTEBOOK_REMOVED,
		NOTEBOOK_REORDERED,
		SHEET_CHANGED,
		/**/
		MAX_SIGNALS
	};

	GSourceFunc signals[MAX_SIGNALS];
private:
	typedef std::list<Workbook *> WorkbookList;
	typedef std::list<Plugin *> PluginList;
	
	Workbook * active_workbook;
	GtkWidget * gtk_window;
	GtkWidget * gtk_window_vbox;
	GtkWidget * gtk_menu;
	Config * cfg;
	proactor::Proactor pro;
	WorkbookList workbooks;
	PluginList plugins;
	AboutDialog about_dialog;
	gchar * absolute_path;
	
	void init (int argc, char *** argv);
public:
	Application (int argc, char *** argv);
	~Application (void);

	int run (void);
	void shutdown (void);
	Plugin * load_plugin (const std::string filename);
	void open_extension (const gchar * filename, gboolean absolute_path);

	RecordView * OpenRecordView (Sheet * sheet, int row, int col);

	GtkWidget * gtkwindow (void);
	GtkWidget * gtkvbox (void);
	GtkWidget * gtkmenu (void);
	AboutDialog * aboutdialog (void);
	Config * config (void);
	proactor::Proactor * proactor (void);
	Workbook * wb (void);
};

#endif
