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
#include <libgtkworkbook/row.h>

static Row * row_object_init (gint);
static void row_object_free (Row *);
static void row_method_destroy (Row *);
static Cell * row_method_getcell (Row *, gint);

Row *
row_new (gint cells) {
	if (cells <= 0)
		return NULL;
	
	Row * r = row_object_init (cells);
	return r;
}

static Row *
row_object_init (gint cells) {
	Row * r = NEW (Row);

	/* Members */
	r->cells = (Cell **) g_malloc (cells * sizeof (Cell *));
	r->size = cells;

	/* Methods */
	r->destroy = row_method_destroy;
	r->get_cell = row_method_getcell;

	/* Initialize the cells. */
	for (gint ii = 0; ii < r->size; ii++)
		r->cells[ii] = cell_new();
	
	return r;
}

static void
row_object_free (Row * row) {
	
	/* Destroy the cell pointers. */
	for (gint ii = 0; ii < row->size; ii++)
		row->cells[ii]->destroy (row->cells[ii]);

	FREE (row);
}

static void
row_method_destroy (Row * row) {
	g_return_if_fail (row != NULL);
	
	row_object_free (row);
}

static Cell *
row_method_getcell (Row * row, gint column) {
	if (!row || row->size < column)
		return NULL;

	return row->cells[ column ];
}
