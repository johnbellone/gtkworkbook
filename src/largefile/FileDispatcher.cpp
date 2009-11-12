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
#include "FileDispatcher.hpp"
#include "Plaintext.hpp"
#include <proactor/Proactor.hpp>
#include <cstdio>

using namespace largefile;

FileDispatcher::FileDispatcher (int e) {
	setEventId (e);
}

FileDispatcher::~FileDispatcher (void) {
}

bool
FileDispatcher::Readline (off64_t start, off64_t N) {
	if (start > this->marks.get(LINE_INDEX_MAX-1).byte) return false;
		
	PlaintextLineReader * reader = new PlaintextLineReader (this->filename, &this->marks, start, N);
	this->addWorker (reader);
	return true;
}

bool
FileDispatcher::Readoffset (off64_t offset, off64_t N) {
	if (offset > this->marks.get(LINE_INDEX_MAX-1).byte) return false;
		
	PlaintextOffsetReader * reader = new PlaintextOffsetReader (this->filename, offset, N);
	this->addWorker (reader);
	return true;
}

bool
FileDispatcher::Readpercent (unsigned int percent, off64_t N) {
	if (percent > 100) return false;
	if (this->marks.get(percent * 10).byte == -1) return false;

	PlaintextOffsetReader * reader = new PlaintextOffsetReader (this->filename,
																					this->marks.get(percent * 10).byte,
																					N);
	this->addWorker (reader);
	return true;
}

void
FileDispatcher::Index (void) {
	PlaintextLineIndexer * indexer = new PlaintextLineIndexer (this->filename, &this->marks);
	this->addWorker (indexer);
}

bool
FileDispatcher::Openfile (const std::string & filename) {
	if (filename.length() == 0)
		return false;

	FILE * fp = NULL;
	if (NULL == (fp = FOPEN (filename.c_str(), "r"))) {
		// STUB: Throw an exception, error, or something to tell the caller that we need
		// to warn the user inside of the GUI. The "false" boolean value may not be enough
		// information.
		return false;
	}

	// Take the relative byte position, e.g. .75 * byte_end, and we now have the a relative
	// line at that byte position for indexing at a later point in time.
	FSEEK_END (fp);
	off64_t byte_end = FTELL (fp);
				
	this->marks.get(0).byte = 0;
	this->marks.get(0).line = 0;
		
	// Compute fuzzy relative position, and set line to -1 for indexing.
	for (int ii = 1; ii < LINE_INDEX_MAX; ii++) {
		double N = ii, K = LINE_PRECISION;
		this->marks.get(ii).byte = (off64_t)((N/K) * byte_end);
		this->marks.get(ii).line = -1;
	}

	FCLOSE (fp);
		
	this->filename = filename;
	return true;
}
  
void *
FileDispatcher::run (void * data) {
	this->Readline(0,1000);
	this->Index();
		
	while (this->isRunning() == true) {
		while (this->inputQueue.size() == 0) {
			if (this->isRunning() == false)
				return NULL;
			concurrent::Thread::sleep(1);
		}
						
		this->pro->onReadComplete (this->inputQueue.pop());
	}

	return NULL;
}
