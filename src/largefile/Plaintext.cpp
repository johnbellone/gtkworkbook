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

PlaintextDispatcher::PlaintextDispatcher (int e)
	: AbstractFileDispatcher (e) {
}

PlaintextDispatcher::~PlaintextDispatcher (void) {
}

bool
PlaintextDispatcher::Readline (off64_t start, off64_t N) {
	// If the user is requesting to read a line that we have not indexed yet then it
	// is not possible to do so. So just return false and they will get prompted with
	// some sort of GUI notification that their request has failed.
	/*
	this->marks.lock();
	
	if (start > this->marks.get(this->marks.size())->line) {
		this->marks.unlock();
		return false;
	}

	this->marks.unlock();
	*/
	
	PlaintextLineReader * reader = new PlaintextLineReader (this->filename, this->marks, start, N);
	this->addWorker (reader);
	return true;
}

bool
PlaintextDispatcher::Readoffset (off64_t offset, off64_t N) {
	// If the user is requesting to read an offset that is larger than the total size
	// of the file then we obviously can't do that. Return false and have the GUI inform
	// them of their wrong choice to do so.
	this->marks->lock();
	
	if (offset > this->marks->get(this->marks->size())->byte) {
		this->marks->unlock();
		return false;
	}

	this->marks->unlock();
	
	PlaintextOffsetReader * reader = new PlaintextOffsetReader (this->filename, offset, N);
	this->addWorker (reader);
	return true;
}

bool
PlaintextDispatcher::Readpercent (float percent, off64_t N) {
	if (percent > 100.0f) return false;

	this->marks->lock();

	int index = (this->marks->size() * (percent / 100));
	off64_t byte = this->marks->get(index)->byte;

	this->marks->unlock();

	PlaintextOffsetReader * reader = new PlaintextOffsetReader (this->filename, byte, N);
	this->addWorker (reader);
	return true;
}

void
PlaintextDispatcher::Index (void) {
	PlaintextLineIndexer * indexer = new PlaintextLineIndexer (this->filename, this->marks);
	this->addWorker (indexer);
}

bool
PlaintextDispatcher::Openfile (const std::string & filename) {
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
	off64_t byte;
			
	// Compute fuzzy relative position, and set line to -1 for indexing.
	for (int ii = 1; ii < LINE_INDEX_MAX; ii++) {
		double N = ii, K = LINE_PRECISION;
		byte = (off64_t)((N/K) * byte_end);

		this->marks->Add (byte, -1);
	}

	FCLOSE (fp);
		
	this->filename = filename;
	return true;
}

bool
PlaintextDispatcher::Closefile (void) {
	return true;
}

void *
PlaintextDispatcher::run (void * null) {
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

PlaintextFileWorker::PlaintextFileWorker (const std::string & filename, FileIndexPtr marks) 
	: AbstractFileWorker (filename, marks) {
	this->fp = NULL;
}

PlaintextFileWorker::PlaintextFileWorker (const std::string & filename)
	: AbstractFileWorker (filename) {
	this->fp = NULL;
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
	: PlaintextFileWorker (filename) {
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
		
PlaintextLineIndexer::PlaintextLineIndexer (const std::string & filename, FileIndexPtr marks)
	: PlaintextFileWorker (filename, marks) {
}

PlaintextLineIndexer::~PlaintextLineIndexer (void) {
}

void *
PlaintextLineIndexer::run (void * null) {
	int index = 0;
	off64_t cursor = 0, count = 0, byte_beg = 0;
	struct timeval start, end;
	const int CHUNK = 16384;
	unsigned char * ch;
	LineOffset * x = NULL;
	unsigned char input[CHUNK+1];
	double ms = 0.0f;
	input[CHUNK] = 0;
	
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
	while (0 != fread (input, 1, CHUNK, this->fp)) {
		if (ferror (this->fp) || false == this->isRunning())
			goto thread_teardown;
		
		ch = input;
		
		while (*ch) {
			while (false == this->marks->trylock()) {
				if (false == this->isRunning())
					goto thread_teardown;
				Thread::sleep(1);
			}

			if (NULL == (x = this->marks->get(index))) {
				this->marks->unlock();
				goto thread_teardown;
			}
					
			if (*ch=='\n') {
				byte_beg = cursor;
				count++;
			}
										
			if (x->byte == cursor++) {
				x->line = count;
				x->byte = byte_beg;
			
				index++;
								
				if (index == this->marks->size()) {
					break;
				}
			}

			this->marks->unlock();
		}
	}
		
	gettimeofday (&end, NULL);

	ms = ((((end.tv_sec-start.tv_sec) * 1000) + ((end.tv_usec-start.tv_usec)/1000.0)) + 0.5);
	std::cout<<"ready (ms:"<<ms<<")!\n"<<std::flush;
	this->dispatcher->removeWorker (this);

 thread_teardown:
	this->Closefile();
	return NULL;
}

PlaintextLineReader::PlaintextLineReader (const std::string & filename,
														FileIndexPtr marks,
														off64_t start,
														off64_t N)
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

	this->marks->lock();
	
	for (off64_t index = 1; index < LINE_INDEX_MAX; index++) {
		if (line_max < this->marks->get(index)->line) {
			delta = this->startLine - this->marks->get(index-1)->line;
			offset = this->marks->get(index-1)->byte;
			break;
		}
	}

	this->marks->unlock();
	
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

 thread_teardown:
	this->Closefile();
	return NULL;
}
