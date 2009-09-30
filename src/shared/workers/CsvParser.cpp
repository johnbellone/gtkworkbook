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
	
	column->sheet->set_cell_value_length (column->sheet, column->row, column->field++, s, length);
}
  
static void
cb2 (int c, void * data) {
	struct csv_column * column = (struct csv_column *)data;

	gdk_threads_enter();
	column->sheet->apply_row (column->sheet, column->row);
	gdk_threads_leave();
	
	column->row = (column->row >= column->sheet->max_rows) ? 0 : column->row + 1;
	column->field = 0;
}

CsvParser::CsvParser (Sheet * sheet,
							 FILE * log,
							 int verbosity)
	: sheet(sheet), log (log) {
	this->wb = sheet->workbook;
}

CsvParser::~CsvParser (void) {
}

void *
CsvParser::run (void * null) {
	std::queue<std::string> queue;
	struct csv_parser csv;
	char buf[1024];
	struct csv_column column = {sheet,0,0,&buf[0]};
	
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
			}
		}	
		concurrent::Thread::sleep(1);
	}

	return NULL;
}
