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

/* This structure is due to the libcsv parser; it uses function pointers to
	do any work inside of an actual tuple. So the cb1 is called after a field
	is parsed and cb2 is called after a tuple/row is parsed. */
static void 
cb1 (void * s, size_t length, void * data) {
	struct csv_column * column = (struct csv_column *)data;

	Cell * cell = (column->array)[column->row][column->field];
	cell->set_row (cell, column->row);
	cell->set_column (cell, column->field++);
	cell->set_value_length (cell, s, length);
}
  
static void
cb2 (int c, void * data) {
	struct csv_column * column = (struct csv_column *)data;

	gdk_threads_enter();
	column->sheet->apply_row (column->sheet,
									  (column->array)[column->row],
									  column->row,
									  column->field);
	gdk_threads_leave();
	
	column->row++;
	column->field = 0;
}

CsvParser::CsvParser (Sheet * sheet,
							 FILE * log,
							 int verbosity)
	: sheet(sheet), log (log) {
	this->wb = sheet->workbook;
	this->maxOfFields = GTK_SHEET(sheet->gtk_sheet)->maxcol + 1;
	this->maxOfRows = GTK_SHEET(sheet->gtk_sheet)->maxrow + 1;
	this->fields = (Cell ***) g_malloc (this->maxOfRows * sizeof (Cell**));

	for (int ii = 0; ii < this->maxOfRows; ii++) {
		this->fields[ii] = (Cell **) g_malloc (this->maxOfFields * sizeof (Cell*));
		
		for (int jj = 0; jj < this->maxOfFields; jj++) {
			this->fields[ii][jj] = cell_new();

			this->fields[ii][jj]->row = ii;
			this->fields[ii][jj]->column = jj;
		}
	}
}

CsvParser::~CsvParser (void) {
	for (int ii = 0; ii < this->maxOfRows; ii++) {
		for (int jj = 0; jj < this->maxOfFields; jj++) {
			if (this->fields[ii][jj])
				this->fields[ii][jj]->destroy (this->fields[ii][jj]);
		}
		FREE (this->fields[ii]);
	}
	FREE (this->fields);
}

void *
CsvParser::run (void * null) {
	std::queue<std::string> queue;
	struct csv_parser csv;
	char buf[1024];
	struct csv_column column = {sheet,
										 this->fields,
										 0,
										 0,
										 &buf[0]};
	
	if (csv_init (&csv, CSV_STRICT) != 0) {
		std::cerr << "Failed initializing libcsv parser library\n";
		return NULL;
	}

	while (this->isRunning() == true) {
		if (this->inputQueue.size() > 0) {
			
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
			
				if (column.row >= sheet->max_rows) {
					column.row = 0;
				}
			}
		}	
		concurrent::Thread::sleep(1);
	}

	return NULL;
}
