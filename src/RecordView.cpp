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
#include <libgtkworkbook/row.h>
#include <gtkextra/gtksheet.h>

static guint
DeleteEventCallback (GtkWindow * window, GdkEvent * event, gpointer data) {
	RecordView * view = (RecordView *) data;

	// The RecordView object needs to be deleted. At this point we are destroying the
	// window object inside of GTK. We can use the delete_event as a closure invocation.
	delete view;
	
	return FALSE;
}

RecordView::RecordView (Application * appstate) {
	this->appstate = appstate;
	this->gtk_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	this->gtk_vbox = gtk_vbox_new (FALSE, 0);
	this->wb = workbook_open (this->gtk_window, "recordview");
	
	gtk_window_set_title (GTK_WINDOW (this->gtk_window), "Record View");
	gtk_window_set_destroy_with_parent (GTK_WINDOW (this->gtk_window), TRUE);

	gtk_box_pack_start (GTK_BOX (this->gtk_vbox), this->wb->gtk_notebook, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (this->gtk_window), this->gtk_vbox);
	
	// Take care of the object once we close the dialog.
	g_signal_connect (G_OBJECT (this->gtk_window), "delete_event",
							G_CALLBACK (DeleteEventCallback), this);
}

RecordView::~RecordView (void) {
	this->wb->destroy (this->wb);
	gtk_widget_destroy (this->gtk_window);
}

void
RecordView::AddSheetRecord (Sheet * sheet) {
	ASSERT (sheet != NULL);
	GtkSheet * gtksheet = GTK_SHEET (sheet->gtk_sheet);
	gint row = gtksheet->active_cell.row;
	
	Sheet * record_sheet = this->wb->add_new_sheet (this->wb,sheet->name,gtksheet->maxcol,1);

	Row * tuple = row_new (gtksheet->maxcol);
		
	sheet->get_row (sheet, row, tuple->cells, tuple->size);

	for (int ii = 0; ii < tuple->size; ii++) {
		
		record_sheet->set_cell (record_sheet,
										ii,
										0,
										tuple->cells[ii]->value->str);
	}

	tuple->destroy (tuple);
}

GtkWidget *
RecordView::window (void) const {
	return this->gtk_window;
}

Application *
RecordView::app (void) const {
	return this->appstate;
}
