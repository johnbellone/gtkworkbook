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
#include "FileIndex.hpp"

using namespace largefile;

FileIndex::FileIndex (void) {
	this->table = NULL;
}

FileIndex::~FileIndex (void) {
	Free();
}

void
FileIndex::Free (void) {
	this->lock();
	
	if (NULL != this->table) {
		// Deal with the "extra" offset param field if it exists.
		if (NULL != this->table->list->extra) {
			for (int ii = 0; ii < this->table->have; ii++) {
				if (NULL != (this->table->list + ii)->extra)
					free ((this->table->list + ii)->extra);
			}
		}
		
		free (this->table->list);
		free (this->table);
	}
	this->table = NULL;

	this->unlock();
}

LookupTable * 
FileIndex::Add (off64_t byte, off64_t line) {
	this->lock();
	
	LineOffset * next;
	
	// If the index was not allocated we're going to do it this first time around.
	// Most of this procedure was taken from Mark Adler's zran.c example inside of the
	// zlib distribution. 
	if (NULL == this->table) {
		// Allocate the lookup table.
		if (NULL == (this->table = (LookupTable *)malloc (sizeof (LookupTable)))) return NULL;

		// Pre-allocate the first eight entries inside of the list.
		if (NULL == (this->table->list = (LineOffset *)malloc (sizeof (LineOffset) << 3))) return NULL;
	
		this->table->have = 1;
		this->table->size = 8;

		// Initialize the first entry to the beginning of the file.
		this->table->list->byte = 0;
		this->table->list->line = 0;
		this->table->list->extra = NULL;
	}
	// The list is currently full: we need to realloc some more space for additional entries.
	else if (this->table->have == this->table->size) {
		this->table->size <<= 1;

		if (NULL == (next = (LineOffset *)realloc (this->table->list, sizeof (LineOffset) * this->table->size))) {
			Free();
			return NULL;
		}

		this->table->list = next;
	}

	next = this->table->list + this->table->have;
	next->byte = byte;
	next->line = line;
	next->extra = NULL;

	this->table->have++;

	this->unlock();
	return this->table;
}

LineOffset *
FileIndex::get (int ii) {
	return this->table->list + ii;
}
