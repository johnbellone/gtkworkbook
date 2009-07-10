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
#ifndef H_APPLICATION
#define H_APPLICATION

typedef struct _ApplicationState ApplicationState;

#include <gtk/gtk.h>
#include <workbook/workbook.h>
#include <config/config.h>
#include <shared.h>
#include "plugin.h"

enum
	{
		SIG_DESTROY_EVENT = 0,
		SIG_DELETE_EVENT,
		SIG_NOTEBOOK_SWITCHED,
		SIG_NOTEBOOK_REMOVED,
		SIG_NOTEBOOK_REORDERED,
		SIG_SHEET_CHANGED,
		/**/
		MAX_SIGNALS
	};

struct _ApplicationState
{
	/* Members */
	GSourceFunc signals[MAX_SIGNALS];
	Config * cfg;
	Workbook * workbook_first, * workbook_last;
	Plugin * plugin_first, * plugin_last;
	GtkWidget * gtk_window;
	GtkWidget * gtk_menu;
	GtkWidget * gtk_window_vbox;
	gchar * absolute_path;
	gboolean * shutdown;

	/* Methods */
	int (*run) (ApplicationState *);
	void (*close) (ApplicationState *);
	void (*open_extension) (ApplicationState *, const gchar *, gboolean);
	Plugin *(*load_plugin) (ApplicationState *, const gchar *);
	void (*exit_application) (ApplicationState *);
};

/* application.c */
ApplicationState * application_init (int * argc, char *** argv);

#endif
