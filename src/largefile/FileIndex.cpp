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
	this->index = NULL;
}

FileIndex::~FileIndex (void) {
	Free();
}

void
FileIndex::Free (void) {
	this->lock();
	
	if (NULL != this->index) {
		free (this->index->list);
		free (this->index);
	}
	this->index = NULL;

	this->unlock();
}

LookupTable * 
FileIndex::Add (off64_t byte, off64_t line) {
	this->lock();
	
	LineIndex * next;
	
	// If the index was not allocated we're going to do it this first time around.
	// Most of this procedure was taken from Mark Adler's zran.c example inside of the
	// zlib distribution. 
	if (NULL == this->index) {
		// Allocate the lookup table.
		if (NULL == (this->index = (LookupTable *)malloc (sizeof (LookupTable)))) return NULL;

		// Pre-allocate the first eight entries inside of the list.
		if (NULL == (this->index->list = (LineIndex *)malloc (sizeof (LineIndex) << 3))) return NULL;
	
		this->index->have = 0;
		this->index->size = 8;
	}
	// The list is currently full: we need to realloc some more space for additional entries.
	else if (this->index->have == this->index->size) {
		index->size <<= 1;

		if (NULL == (next = (LineIndex *)realloc (this->index->list, sizeof (LineIndex) * index->size))) {
			Free();
			return NULL;
		}

		this->index->list = next;
	}

	next = this->index->list + this->index->have;
	next->zbits = -1;
	next->zin = -1;
	next->byte = byte;
	next->line = line;

	this->index->have++;

	this->unlock();
	return this->index;
}

LookupTable *
FileIndex::Add (off64_t byte, off64_t line, off64_t zin, int bits) {
	LineIndex * x = NULL;

	this->lock();

	if (NULL == Add (byte, line)) return NULL;

	x = this->index->list + this->index->have;
	x->zin   = zin;
	x->zbits = bits;

	this->unlock();
	
	return this->index;
}
