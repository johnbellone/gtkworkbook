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
#include "CellTest.h"

TEST_F (CellTest, FixtureIsWorking) {
	EXPECT_TRUE (workbook != NULL);
	EXPECT_TRUE (sheet != NULL);
	EXPECT_TRUE (cell != NULL);
}

TEST_F (CellTest, MethodSetWorks) {
	EXPECT_STREQ ("", cell->value->str);

	cell->set(cell, 0, 1, "2");

	EXPECT_EQ (0,   cell->row);
	EXPECT_EQ (1,   cell->column);
	EXPECT_STREQ ("2", cell->value->str);
}

TEST_F (CellTest, MethodSetValueWorks) {
	EXPECT_STREQ ("", cell->value->str);

	cell->set_value (cell, "testing");

	EXPECT_STREQ ("testing", cell->value->str);
}

TEST_F (CellTest, MethodSetColumnWorks) {
	cell->column = 0;

	EXPECT_EQ (0, cell->column);

	cell->set_column (cell, 2);

	EXPECT_EQ (2, cell->column);
}

TEST_F (CellTest, MethodSetRowWorks) {
	cell->row = 0;

	EXPECT_EQ (0, cell->row);

	cell->set_row (cell, 2);

	EXPECT_EQ (2, cell->row);
}

TEST_F (CellTest, MethodSetRangeWorks) {
	GtkSheetRange range = {1,2,3,4};
	
	cell->range.row0 = cell->range.rowi
		= cell->range.col0 = cell->range.coli = 0;

	EXPECT_EQ (0, cell->range.row0);
	EXPECT_EQ (0, cell->range.rowi);
	EXPECT_EQ (0, cell->range.col0);
	EXPECT_EQ (0, cell->range.coli);

	cell->set_range (cell, &range);

	EXPECT_EQ (1, cell->range.row0);
	EXPECT_EQ (3, cell->range.rowi);
	EXPECT_EQ (2, cell->range.col0);
	EXPECT_EQ (4, cell->range.coli);
}

TEST_F (CellTest, MethodSetJustificationWorks) {
	EXPECT_EQ (GTK_JUSTIFY_LEFT, cell->attributes.justification);

	cell->set_justification (cell, GTK_JUSTIFY_RIGHT);

	EXPECT_EQ (GTK_JUSTIFY_RIGHT, cell->attributes.justification);
}

TEST_F (CellTest, MethodSetFgColorWorks) {
	EXPECT_STREQ ("", (cell->attributes.fgcolor)->str);

	cell->set_fgcolor (cell, "black");

	EXPECT_STREQ ("black", (cell->attributes.fgcolor)->str);
}

TEST_F (CellTest, MethodSetBgColorWorks) {
	EXPECT_STREQ ("", (cell->attributes.bgcolor)->str);

	cell->set_bgcolor (cell, "white");

	EXPECT_STREQ ("white", (cell->attributes.bgcolor)->str);
}

TEST_F (CellTest, MethodSetEditableWorks) {
	cell->attributes.editable = FALSE;
	EXPECT_EQ (FALSE, cell->attributes.editable);

	cell->set_editable (cell, TRUE);
	EXPECT_EQ (TRUE, cell->attributes.editable);
}

TEST_F (CellTest, MethodSetHighlightedWorks) {
	cell->attributes.highlighted = FALSE;
	EXPECT_EQ (FALSE, cell->attributes.highlighted);

	cell->set_highlighted (cell, TRUE);
	EXPECT_EQ (TRUE, cell->attributes.highlighted);
}

TEST_F (CellTest, MethodSetAttributesWorks) {
	CellAttributes attrib;
	attrib.bgcolor = g_string_new_len ("black", 1024);
	attrib.fgcolor = g_string_new_len ("white", 1024);
	attrib.editable = FALSE;
	attrib.highlighted = TRUE;
	attrib.justification = GTK_JUSTIFY_FILL;
	
	EXPECT_STREQ ("", cell->attributes.fgcolor->str);
	EXPECT_STREQ ("", cell->attributes.bgcolor->str);
	EXPECT_EQ (TRUE, cell->attributes.editable);
	EXPECT_EQ (FALSE, cell->attributes.highlighted);
	EXPECT_EQ (GTK_JUSTIFY_LEFT, cell->attributes.justification);
	
	cell->set_attributes (cell, &attrib);

	EXPECT_STREQ ("white", cell->attributes.fgcolor->str);
	EXPECT_STREQ ("black", cell->attributes.bgcolor->str);
	EXPECT_EQ (FALSE, cell->attributes.editable);
	EXPECT_EQ (TRUE, cell->attributes.highlighted);
	EXPECT_EQ (GTK_JUSTIFY_FILL, cell->attributes.justification);
	
	g_string_free (attrib.fgcolor, TRUE);
	g_string_free (attrib.bgcolor, TRUE);
}
