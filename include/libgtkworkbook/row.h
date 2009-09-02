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
#ifndef LIBGTKWORKBOOK_ROW
#define LIBGTKWORKBOOK_ROW

#include "header.h"
#include <gtkextra/gtkextra.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _Row Row;

#include "cell.h"

	/*
	  @description: This object as a container for N cell objects. It makes it much easier
	  than having to allocate blocks of cells manually in code. So, for the occasionally time
	  this code gets used its actually very helpful. If you have large row transformations
	  this may be the place to add additional methods.
	  
	  If you decide to manually set any of the members be sure:
	  a. For single cell changes row and column ranges must equal each other
	  because GtkSheet does not provide interfaces for all operations to
	  be done on single cell.
	  b. Keep in mind that this object was built with the ability to dump a whole
	  array of changes into a Sheet with a single call. This was built for 
	  simple brute forcing of cell additions (and changes).
	  c. If you find yourself adding a lot of code to this object then it may
	  be time to add a new object to libworkbook. It was meant to be simple.

	  If you decide to use gtk_sheet_* calls:
	  a. Do not add them to any methods inside of this object. This object does
	  not (and should not) contain any locking procedures. All the locking for
	  threading should be done in the Sheet object (where all gtk+ calls 
	  should be performed).
	*/
	struct _Row {
		/* Members */
		Cell ** cells;
		gint size;

		/* Methods */
		void (*destroy) (Row * row);
		Cell * (*get_cell) (Row * row, gint column);
	};

	/* row.c */
	Row * row_new (gint columns);
	
#ifdef __cplusplus
}
#endif
#endif /* H_ROW */
