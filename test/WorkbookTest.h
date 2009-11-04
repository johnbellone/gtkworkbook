/*
  The GTKWorkbook Project <http://gtkworkbook.sourceforge.net/>
  Copyright (C) 2009 John Bellone, Jr. <jvb4@njit.edu>

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
#ifndef WORKBOOK_TEST_H
#define WORKBOOK_TEST_H

#include <gtest/gtest.h>
#include <libgtkworkbook/workbook.h>
#include <gtk/gtk.h>

class WorkbookTest : public ::testing::Test {
protected:
	GtkWidget * gtk_window;
	Workbook * workbook;
public:
	virtual void SetUp (void) {
		this->gtk_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		this->workbook = workbook_open (this->gtk_window, "");
	}

	virtual void TearDown (void) {
		this->workbook->destroy (this->workbook);
		gtk_widget_destroy (this->gtk_window);
	}
};

#endif
