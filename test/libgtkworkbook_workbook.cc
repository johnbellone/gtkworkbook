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
#include "WorkbookTest.h"
#include <gtkextra/gtkextra.h>

// Basic sanity checks to make sure the underlying fixture is working and setup. 
TEST_F (WorkbookTest, FixtureIsWorking) {
	EXPECT_TRUE (gtk_window != NULL);
	EXPECT_TRUE (workbook != NULL);
}

// The Sheet object linked list should be empty because none have been added. This
//	is a mere sanity check. 
TEST_F (WorkbookTest, SheetListIsEmpty) {
	EXPECT_TRUE (workbook->sheet_first == NULL);
	EXPECT_TRUE (workbook->sheet_last == NULL);
	EXPECT_TRUE (workbook->focus_sheet == NULL);
}

// This test verifies that the "add_new_sheet" method correctly adds a sheet with
//	the proper columns, rows, and name. 
TEST_F (WorkbookTest, MethodAddSheetWorks) {
	Sheet * sheet = workbook->add_new_sheet (workbook, "One", 1, 1);
	ASSERT_TRUE (sheet != NULL);

	EXPECT_EQ (sheet, workbook->sheet_first);
	EXPECT_EQ (1, sheet->max_rows);
	EXPECT_EQ (1, sheet->max_columns);
	EXPECT_STREQ ("One", sheet->name);
}

// This test checks to make sure that the Workbook object's "get_sheet" method returns
//	the proper pointer to a sheet with more than one item in the linked list. 
TEST_F (WorkbookTest, MethodGetSheetWorks) {
	workbook->add_new_sheet (workbook, "one", 1, 1);
	workbook->add_new_sheet (workbook, "two", 2, 2);
	workbook->add_new_sheet (workbook, "three", 3, 3);
	
	Sheet * a = workbook->add_new_sheet (workbook, "test", 4, 4);
	Sheet * b = workbook->get_sheet (workbook, "test");
	
	EXPECT_TRUE (a == b);
	EXPECT_EQ (4, b->max_rows);
	EXPECT_EQ (4, a->max_columns);

	Sheet * c = workbook->get_sheet (workbook, "one");
	EXPECT_EQ (1, c->max_rows);
	EXPECT_EQ (1, c->max_columns);
}

// This test verifies that the Workbook object's "remove_sheet" method removes the
//	correct sheet when doing so, and restructures the linked list accordingly. 
TEST_F (WorkbookTest, MethodRemoveSheetWorks) {
	Sheet * a = workbook->add_new_sheet (workbook, "one", 1, 1);
	Sheet * b = workbook->add_new_sheet (workbook, "two", 1, 1);
	Sheet * c = workbook->add_new_sheet (workbook, "three", 1, 1);

	EXPECT_EQ (a, workbook->sheet_first);
	EXPECT_EQ (c, workbook->sheet_last);
	
	workbook->remove_sheet (workbook, c);

	EXPECT_EQ (a, workbook->sheet_first);
	EXPECT_EQ (b, workbook->sheet_last);

	workbook->remove_sheet (workbook, a);

	EXPECT_EQ (b, workbook->sheet_first);
	EXPECT_EQ (b, workbook->sheet_last);

	workbook->remove_sheet (workbook, b);

	EXPECT_EQ (NULL, workbook->sheet_first);
	EXPECT_EQ (NULL, workbook->sheet_last);
}

TEST_F (WorkbookTest, MethodMoveSheetWorks) {
	// TODO(jb): Need to check the order inside of the GtkNotebook widget to test that
	//	the method actually works. This does not change the order of the linkages inside
	//	of the Sheet linked list. 
}

TEST_F (WorkbookTest, MethodMoveSheetIndexWorks) {
	// TODO(jb): Need to check the order inside of the GtkNotebook widget to test that
	//	the method actually works. This does not change the order of the linkages inside
	//	of the Sheet linked list. 
}
