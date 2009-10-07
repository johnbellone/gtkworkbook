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
#ifndef LIBGTKWORKBOOK_SHEET
#define LIBGTKWORKBOOK_SHEET

#include "header.h"
#include <gtk/gtk.h>
#include <gtkextra/gtksheet.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GEOMETRY_FILE_VERSION 0x000001

	typedef struct _Sheet Sheet;

#include "workbook.h"
#include "cell.h"
#include "row.h"
	
	/*
	  @description: This object abstracts away all of the calls to the native
	  GtkSheet methods. It is meant to be used with the Cell object(s).
    
	  Please keep the following in mind:
	  a. All calls to gtk_* methods should be performed in here and not inside of
	  a cell object. 
	  b. All calls to gtk_* methods should be performed inside of a lock, e.g. 
	  you should always use gdk_threads_enter and gtk_threads_exit. Any calls
	  outside of the window (main) thread will usually result in problems if
	  you do not get a mutex. 
	*/
	struct _Sheet
	{
		/* Members */
		Sheet * next;
		Sheet * prev;
		Cell *** cells;
		Row * column_titles;
		Row * row_titles;
		gchar * name;
		Workbook * workbook;
		GtkWidget * gtk_label;
		GtkWidget * gtk_sheet;
		GtkWidget * gtk_box;
		GtkWidget * gtk_scrolledwindow;
		gint x;
		gint page;
		gint attention;
		gint notices;
		gint max_rows;
		gint max_columns;
		gboolean has_focus;

		/* Methods */
		void (*destroy) (Sheet * sheet);
		void (*set_attention) (Sheet * sheet, gint attention);
		void (*apply_range) (Sheet * sheet, const GtkSheetRange * range, const CellAttributes * attrib);
		void (*apply_array) (Sheet * sheet, Cell ** array, gint size);
		void (*apply_cell) (Sheet * sheet, const Cell * cell);
		void (*apply_row) (Sheet * sheet, gint row);
		void (*set_cell) (Sheet * sheet, gint row, gint column, const gchar * value);
		void (*set_cell_value_length) (Sheet * sheet, gint row, gint column, void * value, size_t length);
		void (*range_set_background) (Sheet * sheet, const GtkSheetRange * range, const gchar * desc);
		void (*range_set_foreground) (Sheet * sheet, const GtkSheetRange * range, const gchar * desc);
		gboolean (*save) (Sheet * sheet, const gchar * filepath);
		gboolean (*load) (Sheet * sheet, const gchar * filepath);
		void (*get_row) (Sheet * sheet, gint row, Cell ** array, gint size);
		void (*set_column_title) (Sheet * sheet, gint column, const char * title);
		void (*set_row_title) (Sheet * sheet, gint row, const char * title);
	};

	/* sheet.c */
	Sheet *sheet_new (Workbook *, const gchar *, gint, gint);

#ifdef __cplusplus
}
#endif
#endif /*H_SHEET*/
