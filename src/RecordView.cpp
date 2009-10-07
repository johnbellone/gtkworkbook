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
#include <iostream>

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
	gtk_window_set_default_size (GTK_WINDOW (this->gtk_window), 400, 600);
	
	gtk_box_pack_start (GTK_BOX (this->gtk_vbox), this->wb->gtk_notebook, TRUE, TRUE, 0);

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
	GtkSheetRange range = {gtksheet->range.row0, 0, gtksheet->range.rowi, 0};
	Row * tuple = row_new (gtksheet->maxcol + 1);
	gint sheet_rows = 1;
	
	// More than one row is selected; we must transpose them all into the RV sheet.
	if (range.row0 != range.rowi) {
		sheet_rows = range.rowi - range.row0 + 1;
	}
			
	Sheet * record_sheet = this->wb->add_new_sheet (this->wb,sheet->name,gtksheet->maxcol + 1,sheet_rows);
	int column = 0, row = 0;

	do {
		sheet->get_row (sheet, range.row0, tuple->cells, tuple->size);

		// Change the titles of the columns to the row titles. This is only going to work if the
		// row titles have been explicitly set somewhere inside of the plugin.
		record_sheet->set_column_title (record_sheet,
												  row,
												  sheet->row_titles->cells[range.row0]->value->str);
		
		for (int ii = 0; ii < tuple->size; ii++) {

			// We only need to change the row titles once. This can happen on our last iteration.
			if (range.row0 == range.rowi) {
				record_sheet->set_row_title (record_sheet,
													  ii,
													  sheet->column_titles->cells[ii]->value->str);

			}
			
			record_sheet->set_cell (record_sheet,
											ii,
											column,
											tuple->cells[ii]->value->str);

		}
		
		range.row0++; column++; row++;
	} while (range.row0 <= range.rowi);

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
