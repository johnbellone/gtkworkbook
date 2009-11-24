/* 
   The GTKWorkbook Project <http://gtkworkbook.sourceforge.net/>
   Copyright (C) 2008,2009 John Bellone, Jr. <jvb4@njit.edu>

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
#include <iostream>
#include <cstdio>
#include <sys/time.h>
#include <unistd.h>
#include <glib.h>
#include <header.h>
#include "Plaintext.hpp"

using namespace largefile;

PlaintextFileWorker::PlaintextFileWorker (const std::string & filename, FileIndex * marks) 
	: AbstractFileWorker (filename, marks) {
}

PlaintextFileWorker::~PlaintextFileWorker (void) {
}

bool
PlaintextFileWorker::Openfile (void) {
	if (NULL != this->fp) {
		// STUB: We need to throw an error here explaining that the file has already been opened. For
		// whatever reason someone attempted to call this method twice. That doesn't work.
		return false;
	}
			
	if (NULL == (this->fp = FOPEN (this->filename.c_str(), "r"))) {
		// STUB: Throw an error eventually to inform the user (in the GUI) of the problem.
		return false;
	}
		
	return true;
}

bool
PlaintextFileWorker::Closefile (void) {
	if (NULL == this->fp) {
		// STUB: Why was this called a second time? This is more of a warning than anything.
		// Any additional calls are unneeded.
		return false;
	}
	else {
		fclose (this->fp);
		this->fp = NULL;
	}
	return true;
}

PlaintextOffsetReader::PlaintextOffsetReader (const std::string & filename, off64_t offset, off64_t N)
	: PlaintextFileWorker (filename, NULL) {
	this->startOffset = offset;
	this->numberOfLinesToRead = N;
}

PlaintextOffsetReader::~PlaintextOffsetReader (void) {
}
	
void *
PlaintextOffsetReader::run (void * null) {
	char buf[4096];
	int ch;

	if (PlaintextFileWorker::Openfile () == false) {
		// STUB: throw some kind of error here; we failed opening the file.
		g_critical ("Failed opening file descriptor inside of PlaintextOffsetReader.");
		return NULL;
	}
		
	off64_t offset = 0;
	off64_t read_max = this->numberOfLinesToRead;

	FSEEK (this->fp, this->startOffset);
						
	// We need to go to the beginning of the (next) line.
	while (EOF != (ch = fgetc (this->fp))) {
		if (ch == '\n') {
			offset = FTELL (fp);
			break;
		}
	}
			
	for (off64_t ii = 0; ii < read_max; ii++) {
		if (NULL == (std::fgets (buf, 4096, this->fp)))
			break;
						
		((proactor::InputDispatcher *)this->dispatcher)->onReadComplete (buf);
	}
		
	this->dispatcher->removeWorker (this);
	this->Closefile();
	return NULL;
}
		
PlaintextLineIndexer::PlaintextLineIndexer (const std::string & filename, FileIndex * marks)
	: PlaintextFileWorker (filename, marks) {
}

PlaintextLineIndexer::~PlaintextLineIndexer (void) {
}

void *
PlaintextLineIndexer::run (void * null) {
	int ch, index = 0;
	off64_t cursor = 0, count = 0, byte_beg = 0;
	struct timeval start, end;

	if (PlaintextFileWorker::Openfile() == false) {
		// STUB: throw some kind of error here; we failed opening the file.
		g_critical ("Failed opening file descriptor in line indexer");
		return NULL;
	}
		
	std::cout<<"index start..."<<std::flush;
		
	gettimeofday (&start, NULL);
		
	// We need to get a absoltue line number from the relative position. We're not
	// going to get away from having to sequentially read this file in, but once we
	// have line numbers we can jump throughout the file pretty quickly.
	while (EOF != (ch = fgetc (this->fp))) {
		if (ch=='\n') {
			byte_beg = cursor;
			count++;
		}
						
		if (this->marks->get(index).byte == cursor++) {
			this->marks->get(index).line = count;
			this->marks->get(index).byte = byte_beg;
			
			index++;

			this->marks->unlock();
				
			if (index == LINE_INDEX_MAX) {
				break;
			}
			else {
				Thread::sleep(1);								  
			}
				
			// Crude implementation of a spinlock. Wait while another thread is doing
			// some reading before we begin indexing again.
			while (this->marks->trylock() == false)
				Thread::sleep(1);
		}
	}

	this->marks->unlock();
		
	gettimeofday (&end, NULL);

	double ms = ((((end.tv_sec-start.tv_sec) * 1000) + ((end.tv_usec-start.tv_usec)/1000.0)) + 0.5);
	std::cout<<"ready (ms:"<<ms<<")!\n"<<std::flush;
	this->dispatcher->removeWorker (this);
	this->Closefile();
	return NULL;
}

PlaintextLineReader::PlaintextLineReader (const std::string & filename, FileIndex * marks, off64_t start, off64_t N)
	: PlaintextFileWorker (filename, marks) {
	this->startLine = start;
	this->numberOfLinesToRead = N;
}

PlaintextLineReader::~PlaintextLineReader (void) {
}

void *
PlaintextLineReader::run (void * null) {
	char buf[4096];

	if (false == PlaintextFileWorker::Openfile ()) {
		// STUB: throw some kind of error here; we failed opening the file.
		g_critical ("Failed opening file descriptor in PlaintextLineReader");
		return NULL;
	}
		
	off64_t offset = 0, delta = 0;
	off64_t read_max = this->numberOfLinesToRead, line_max = this->startLine + read_max;

	for (off64_t index = 1; index < LINE_INDEX_MAX; index++) {
		// Crude implementation of a spinlock. Wait while another thread is doing
		// some reading before we begin indexing again.
		while (this->marks->trylock() == false)
			Thread::sleep(1);
			
		if (line_max < this->marks->get(index).line) {
			delta = this->startLine - this->marks->get(index-1).line;
			offset = this->marks->get(index-1).byte;
			this->marks->unlock();
			break;
		}
		this->marks->unlock();
	}

	FSEEK (this->fp, offset);
		
	// Munch lines to get to our starting point.
	while (delta > 0) {
		if (NULL == std::fgets (buf, 4096, this->fp))
			break;
						
		--delta;
	}
		
	for (off64_t ii = 0; ii < read_max; ii++) {
		if (NULL == std::fgets (buf, 4096, this->fp))
			break;
						      
		((proactor::InputDispatcher *)this->dispatcher)->onReadComplete (buf);
	}
		
	this->dispatcher->removeWorker (this);
	this->Closefile();
	return NULL;
}