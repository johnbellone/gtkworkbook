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
#include "RecordView.hpp"

static void
RecordViewCallback (GtkWidget * w, gpointer data) {

}

RecordView::RecordView (Application * appstate) {
	this->appstate = appstate;

	this->gtk_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (this->gtk_window), "Record View");
	/*	gtk_window_set_model (GTK_WINDOW (this->gtk_window), FALSE);
		gtk_window_set_resizeable (GTK_WINDOW (this->gtk_window), TRUE); */
	gtk_window_set_destroy_with_parent (GTK_WINDOW (this->gtk_window), TRUE);
}

RecordView::~RecordView (void) {

}

GtkWidget *
RecordView::window (void) const {
	return this->gtk_window;
}

Application *
RecordView::app (void) const {
	return this->appstate;
}
