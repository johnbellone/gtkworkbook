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
#include <queue>
#include <string>
#include <iostream>

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
							 int maxOfFields,
							 int maxOfRows)
	: sheet(sheet), log (log), maxOfFields (maxOfFields), maxOfRows(maxOfRows) {
	this->wb = sheet->workbook;
	this->sizeOfFields = 0;
	
	this->fields = (Cell ***) g_malloc (this->maxOfRows * sizeof (Cell**) + sizeof(double));
	
	for (int jj = 0; jj < this->maxOfRows; jj++) {
		this->fields[jj] = (Cell **)g_malloc (maxOfFields * sizeof (Cell*) + sizeof(double));
		
		for (int ii = 0; ii < this->maxOfFields; ii++)
			this->fields[jj][ii] = NULL;
	}
}

CsvParser::~CsvParser (void) {
	for (int jj = 0; jj < this->maxOfRows; jj++) {
		for (int ii = 0; ii < this->maxOfFields; ii++) {
			if (this->fields[jj][ii])
				(this->fields[jj][ii])->destroy (this->fields[jj][ii]);
		}

		g_free (this->fields[jj]);
	}
	g_free (this->fields);
}

void *
CsvParser::run (void * null) {
	std::queue<std::string> queue;
	struct csv_parser csv;
	char buf[1024];
	    
	if (csv_init (&csv, CSV_STRICT) != 0) {
		std::cerr << "Failed initializing libcsv parser library\n";
		return NULL;
	}

	while (this->isRunning() == true) {
		if (this->inputQueue.size() > 0) {
			bool draw = false;
			struct csv_column column = {sheet,
												 this->fields[0],
												 this->maxOfFields,
												 this->sizeOfFields,
												 0,
												 0,
												 &buf[0]};

			
			// Lock, copy, clear, unlock. - Free this up.
			this->inputQueue.lock();
			this->inputQueue.copy (queue);
			this->inputQueue.clear();
			this->inputQueue.unlock();

			while (queue.size() > 0) {
				std::string str = queue.front(); queue.pop();
				size_t bytes = str.length();

				if (this->isRunning() == false)
					break;

				if ((bytes = csv_parse (&csv, str.c_str(), bytes, cb1, cb2, &column)) == bytes) {
					if (csv_error (&csv) == CSV_EPARSE) {
						std::cerr << "Parsing error on input: "<<"\n";
						continue;
					}
				}

				csv_fini (&csv, cb1, cb2, &column);
				
				if (column.row >= (column.sheet)->max_rows)
					column.row = 0;
				else {
					column.array = this->fields[column.row];
					draw = true;
				}
			}

			if (draw) {
				
				gdk_threads_enter();	
	
				for (int jj = 0; jj < this->maxOfRows; jj++) {
					sheet->apply_row (sheet, this->fields[jj], jj, this->maxOfFields-1);
				}
	
				gdk_threads_leave();
				
			}
		}	
		concurrent::Thread::sleep(1);
	}

	return NULL;
}
