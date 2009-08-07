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
#ifndef HPP_OPENSTREAMDIALOG
#define HPP_OPENSTREAMDIALOG

#include <gtk/gtk.h>

namespace realtime {

	class Realtime;

	struct OpenStreamDialog {
		OpenStreamDialog (void) {
			rt = NULL;
			widget = NULL;
			host_entry = NULL;
			port_entry = NULL;
		}

		Realtime * rt;
		GtkWidget * widget;
		GtkWidget * host_entry;
		GtkWidget * port_entry;
	};
}

#endif
