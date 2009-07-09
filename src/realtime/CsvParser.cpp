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
#include "CsvParser.hpp"
#include <gtkworkbook/sheet.h>
#include <gtkworkbook/cell.h>
#include <iostream>

namespace realtime {

  struct csv_column {
    Sheet * sheet;
    Cell ** array;
    int row;
    int field;
    char * value;
  };

  /* This structure is due to the libcsv parser; it uses function pointers to
     do any work inside of an actual tuple. So the cb1 is called after a field
     is parsed and cb2 is called after a tuple/row is parsed. */
  static void 
  cb1 (void * s, size_t length, void * data) {
    struct csv_column * column = (struct csv_column *)data;
    Cell * cell = column->array[column->field];

    cell->set_row (cell, column->row);
    cell->set_column (cell, column->field++);
    cell->set_value_length (cell, s, length);
  }

  static void
  cb2 (int c, void * data) {
    struct csv_column * column = (struct csv_column *)data;
    column->row++;
    column->field = 0;
  }

  void *
  CsvParser::run (void * null) {
    this->running = true;
    struct csv_parser csv;
    struct csv_column column = {this->wb->sheet_first,
				this->cell, 
				0, 
				0, 
				new char[1024]};
    
    if (csv_init (&csv, CSV_STRICT) != 0) {
      std::cerr << "Failed initializing libcsv parser\n";
      return NULL;
    }

    while (this->running == true) {
      while (this->inputQueue.size() > 0) {

	if (this->running == false)
	  break;

	std::string buf = this->inputQueue.pop();
	size_t bytes = buf.length();
 
	// Parse the CSV input
	if ((bytes = csv_parse(&csv, 
			       buf.c_str(), 
			       bytes, 
			       cb1,
			       cb2,
			       &column)) == bytes) {
	  if (csv_error (&csv) == CSV_EPARSE)
	    std::cerr << "Parsing error on input: " << buf << "\n";
	}

	csv_fini (&csv, cb1, cb2, &column);

	this->wb->sheet_first->apply_array (this->wb->sheet_first,
					    this->cell,
					    10);

	if (column.row >= (column.sheet)->max_rows)
	  column.row = 0;
      }

      Thread::sleep(100);

    }

    csv_free (&csv);
    delete column.value;
    return NULL;
  }

} // end of namespace
