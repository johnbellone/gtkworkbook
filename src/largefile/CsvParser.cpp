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
#include "CsvParser.hpp"
#include <gtkworkbook/sheet.h>
#include <queue>
#include <string>
#include <iostream>

namespace largefile {

	struct csv_column {
		Sheet * sheet;
		Cell ** array;
		int & array_max;
		int & array_size;
		int row;
		int field;
		char * value;
	};

	/* This structure is due to the libcsv parser; it uses function pointers to
		do any work inside of an actual tuple. So the cb1 is called after a field
		is parsed and cb2 is called after a tuple/row is parsed. */
	static void 
	cb1 (void * s, size_t length, void * data) {
		struct csv_column * column = (struct csv_column *) data;
		int & array_max = column->array_max;

		// Resize the cell array here.
		if (column->field >= array_max) {
			int max = (2 * array_max);
			(column->array) = (Cell **) g_realloc ((column->array),max*sizeof (Cell*));

			for (int ii = array_max; ii < max; ii++)
				(column->array)[ii] = NULL;
		}
	
		if ((column->array)[column->field] == NULL)
			(column->array)[column->field] = cell_new();

		Cell * cell = (column->array)[column->field];
		cell->set_row (cell, column->row);
		cell->set_column (cell, column->field++);
		cell->set_value_length (cell, s, length);
	}
  
	static void
	cb2 (int c, void * data) {
		struct csv_column * column = (struct csv_column *)data;
		column->row++;
		column->array_size = column->field - 1;
		column->field = 0;
	}

	CsvParser::CsvParser (Sheet * sheet,
								 FILE * log,
								 int verbosity,
								 int maxOfFields)
		: sheet(sheet), log (log), maxOfFields (maxOfFields) {
		this->wb = sheet->workbook;
		this->sizeOfFields = 0;
		this->fields = (Cell **) g_malloc (maxOfFields*sizeof (Cell*));

		for (int ii = 0; ii < this->maxOfFields; ii++)
			this->fields[ii] = NULL;
	}

	CsvParser::~CsvParser (void) {
		for (int ii = 0; ii < this->maxOfFields; ii++) {
			if (this->fields[ii])
				(this->fields[ii])->destroy (this->fields[ii]);
		}
    
		g_free (this->fields);
	}

	void *
	CsvParser::run (void * null) {
		this->running = true;
		std::queue<std::string> queue;
		struct csv_parser csv;
		struct csv_column column = {sheet,
											 this->fields,
											 this->maxOfFields,
											 this->sizeOfFields,
											 0,
											 0,
											 new char [1024]};
    
		if (csv_init (&csv, CSV_STRICT) != 0) {
			std::cerr << "Failed initializing libcsv parser library\n";
			return NULL;
		}

		while (this->running == true) {
			if (this->inputQueue.size() > 0) {
	
				// Lock, copy, clear, unlock. - Free this up.
				this->inputQueue.lock();
				this->inputQueue.copy (queue);
				this->inputQueue.clear();
				this->inputQueue.unlock();

				while (queue.size() > 0) {
					std::string buf = queue.front(); queue.pop();
					size_t bytes = buf.length();

					if (this->running == false)
						break;

					if ((bytes = csv_parse (&csv, buf.c_str(), bytes, cb1, cb2, &column)) == bytes) {
						if (csv_error (&csv) == CSV_EPARSE) {
							std::cerr << "Parsing error on input: "<<"\n";
							continue;
						}
					}

					csv_fini (&csv, cb1, cb2, &column);

					sheet->apply_row (sheet,
											this->fields,
											column.row - 1,
											this->sizeOfFields);

					if (column.row >= (column.sheet)->max_rows)
						column.row = 0;

					concurrent::Thread::sleep(5);
				}
				
			}	
			concurrent::Thread::sleep(5);
		}

		return NULL;
	}

} // end of namespace
