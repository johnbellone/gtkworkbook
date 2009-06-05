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
#include <workbook/cell.h>

/* cell.c (static) */
static Cell *cell_object_init (void);
static void cell_object_free (Cell *);
static void cell_method_set_value (Cell *, const gchar *);
static void cell_method_set_value_length (Cell *, void *, size_t);
static void cell_method_set_column (Cell *, gint);
static void cell_method_set_row (Cell *, gint);
static void cell_method_set_all (Cell *, gint, gint, const gchar *);
static void cell_method_set_justification (Cell *, GtkJustification);
static void cell_method_destroy (Cell *);
static void cell_method_set_bgcolor (Cell *, const gchar *);
static void cell_method_set_fgcolor (Cell *, const gchar *);
static void cell_method_set_attributes (Cell *, const CellAttributes *);
static void cell_method_set_range (Cell *, const GtkSheetRange *);

/* @description: The function returns a pointer to a Cell object. */
Cell *
cell_new (void)
{
  Cell * cell = cell_object_init ();
  return cell;
}

/* @description: This function is the Cell object's constructor. */
static Cell *
cell_object_init (void)
{
  Cell * cell = NEW (Cell);

  cell->value = g_string_new_len ("", 4096);
  cell->attributes.bgcolor = g_string_new_len ("", 1024);
  cell->attributes.fgcolor = g_string_new_len ("", 1024);

  /* Methods */
  cell->set_fgcolor = cell_method_set_fgcolor;
  cell->set_bgcolor = cell_method_set_bgcolor;
  cell->set_attributes = cell_method_set_attributes;
  cell->set_range = cell_method_set_range;
  cell->set_value = cell_method_set_value;
  cell->set_value_length = cell_method_set_value_length;
  cell->set_column = cell_method_set_column;
  cell->set_row = cell_method_set_row;
  cell->set = cell_method_set_all;
  cell->set_justification = cell_method_set_justification;
  cell->destroy = cell_method_destroy;

  return cell;
}

/* @description: This object frees the memory created by the Cell object.
   @cell: The pointer to the object to free. */
static void
cell_object_free (Cell * cell)
{
  ASSERT (cell != NULL);

  g_string_free (cell->value, TRUE);
  g_string_free (cell->attributes.fgcolor, TRUE);
  g_string_free (cell->attributes.bgcolor, TRUE);
  FREE (cell);
}

/* @description: This method sets a cell in the GtkSheet with the attributes
   of a Cell object.
   @row: An integer value of the row to set.
   @column: An integer value fo the column to set.
   @value: A string value to what should be set in the Cell. */
static void
cell_method_set_all (Cell * cell, 
		     gint row, gint column, 
		     const gchar * value)
{
  ASSERT (cell != NULL);

  g_string_assign (cell->value, value);
  cell->row = row;
  cell->column = column;  
}

/* @description: This method sets the justification of the specified Cell
   object.
   @justification: The GtkJustification value for the cell. */
static void
cell_method_set_justification (Cell * cell, 
			       GtkJustification justification)
{
  ASSERT (cell != NULL);
  cell->attributes.justification = justification;
}

/* @description: The method sets the row of the Cell object.
   @row: Integer value of the row. */
static void
cell_method_set_row (Cell * cell, 
		     gint row)
{
  ASSERT (cell != NULL);
  cell->row = row;
  cell->range.row0 = cell->range.rowi = row;
}

/* @description: The method sets the column of the Cell object.
   @column: Integer value fo the column. */
static void
cell_method_set_column (Cell * cell, 
			gint column)
{
  ASSERT (cell != NULL);
  cell->column = column;
  cell->range.col0 = cell->range.coli = column;
}

/* @description: This method sets the bgcolor of the Cell object.
   @color: String value of the color, e.g. white, blue, red. */
static void
cell_method_set_bgcolor (Cell * cell, 
			 const gchar * color)
{
  ASSERT (cell != NULL);

  g_string_assign (cell->attributes.bgcolor, color);
}

/* @description: This method sets the fgcolor of the Cell object.
   @color: String value of the color, e.g. white, blue, red. */
static void 
cell_method_set_fgcolor (Cell * cell, 
			 const gchar *color)
{
  ASSERT (cell != NULL);
  
  g_string_assign (cell->attributes.fgcolor, color);
}

/* @description: This method sets the range of the Cell object.
   @range: A pointer to a GtkSheetRange object. */
static void
cell_method_set_range (Cell * cell, 
		       const GtkSheetRange * range)
{
  ASSERT (cell != NULL); ASSERT (range != NULL);
  
  cell->range.row0 = range->row0;
  cell->range.col0 = range->col0;
  cell->range.rowi = range->rowi;
  cell->range.coli = range->coli;
}

/* @description: This method sets the attributes of the Cell object.
   @attrib: A pointer to the Cell Attributes object. */
static void
cell_method_set_attributes (Cell * cell, 
			    const CellAttributes * attrib)
{
  ASSERT (cell != NULL); ASSERT (attrib != NULL);

  cell->attributes.justification = attrib->justification;
 
  g_string_assign (cell->attributes.fgcolor, attrib->fgcolor->str);
  g_string_assign (cell->attributes.bgcolor, attrib->bgcolor->str);
}

/* @description: This method sets the text value of the Cell object.
   @value: This is a string pointer to the value. */
static void
cell_method_set_value (Cell * cell, 
		       const gchar * value)
{
  ASSERT (cell != NULL);
  
  g_string_assign (cell->value, value);
}

static void
cell_method_set_value_length (Cell * cell, void * s, size_t length) {
  ASSERT (cell != NULL);

  g_string_assign (cell->value, "");
  g_string_append_len (cell->value, (const char *)s, length);
}

static void
cell_method_destroy (Cell * cell)
{
  g_return_if_fail (cell != NULL);

  cell_object_free (cell);
}
