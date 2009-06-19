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
#include "File.hpp"
#include <iostream>
#include <concurrent/ThreadPool.hpp>
#include <concurrent/ScopedMemoryLock.hpp>
#include <cstdio>
#include <sys/time.h>
#include <unistd.h>

namespace largefile {
	
	FileDispatcher::FileDispatcher (int e, proactor::Proactor * pro) {
		this->fp = NULL;
		this->pro = pro;
		setEventId(e);
	}

	FileDispatcher::~FileDispatcher (void) {
		if (this->fp != NULL)
			this->close();
	}

	void
	FileDispatcher::read (long int start, long int N) {
		LineReader * reader = new LineReader (this, this->fp, this->marks, start, N);
		this->addWorker (reader);
	}

	void
	FileDispatcher::index (void) {
		LineIndexer * indexer = new LineIndexer (this, this->fp, this->marks);
		this->addWorker (indexer);
	}

	bool
	FileDispatcher::open (const std::string & filename) {
		if (filename.length() == 0)
			return false;

		if ((this->fp = std::fopen (filename.c_str(), "r")) == NULL) {
			// stub: throw an error somewhere
			return false;
		}

		// Take the relative byte position, e.g. .75 * byte_end, and we now have the a relative
		// line at that byte position for indexing at a later point in time.
		std::fseek (this->fp, 0L, SEEK_END);
		long int byte_end = std::ftell (this->fp);

		this->marks[0].byte = 0;
		this->marks[0].line = 0;
		
		// Compute fuzzy relative position, and set line to -1 for indexing.
		for (int ii = 1; ii < 101; ii++) {
			float N = ii, K = 10000;
			this->marks[ii].byte = (long int)((N/K) * byte_end);
			this->marks[ii].line = -1;
		}

		std::fseek (this->fp, 0L, SEEK_SET);
		
		concurrent::ScopedMemoryLock::addMemoryLock ((unsigned long int)this->fp);
		this->filename = filename;
		return true;
	}

	bool 
	FileDispatcher::close (void) {
		concurrent::ScopedMemoryLock mutex ((unsigned long int)this->fp);
		if (this->fp == NULL)
			return false;

		std::fclose (this->fp); this->fp = NULL;
		mutex.remove();
		return true;
	}
  
	void *
	FileDispatcher::run (void * null) {
		this->running = true;

		this->index();
		
		while (this->running == true) {
			if (this->fp == NULL) {
				this->running = false;
				break;
			}

			this->inputQueue.lock();
      
			while (this->inputQueue.size() > 0) {
				if (this->running == false)
					break;

				this->pro->onReadComplete (this->inputQueue.pop());
			}

			this->inputQueue.unlock();

			concurrent::Thread::sleep(5);
		}

		return NULL;
	}
  
	LineIndexer::LineIndexer (proactor::InputDispatcher * d,
									  FILE * fp,
									  LineIndex * marks) {
		this->fp = fp;
		this->dispatcher = d;
		this->marks = marks;
	}

	LineIndexer::~LineIndexer (void) {
		std::cout<<"index finished\n"<<std::flush;
	}

	void *
	LineIndexer::run (void * null) {
		this->running = true;
		int ch, index = 0;
		long long int cursor = 0, count = 0, byte_beg = 0;

		struct timeval start, end;
		
		concurrent::ScopedMemoryLock mutex ((unsigned long int)this->fp, true);

		gettimeofday(&start, NULL);
		
		// We need to get a absoltue line number from the relative position. We're not
		// going to get away from having to sequentially read this file in, but once we
		// have line numbers we can jump throughout the file pretty quickly.
		while ((ch = std::fgetc(this->fp)) != EOF) {
			cursor++;
			
			if (ch=='\n') count++;
						
			if (this->marks[index].byte <= cursor) {
				this->marks[index].line = count;
				this->marks[index].byte = byte_beg;

				index++;
				
				if (index == 101)
					break;
			}

			if (ch=='\n') byte_beg = cursor + 1;
		}

		gettimeofday(&end, NULL);

		std::cout<<"ms: "<<((((end.tv_sec-start.tv_sec) * 1000) + ((end.tv_usec-start.tv_usec)/1000.0)) + 0.5)<<"\n";
		
		this->running = false;
		this->dispatcher->removeWorker (this);
		return NULL;
	}

	LineReader::LineReader (proactor::InputDispatcher * d,
									FILE * fp,
									LineIndex * marks,
									long int start,
									long int N) {
		this->fp = fp;
		this->dispatcher = d;
		this->startLine = start;
		this->numberOfLinesToRead = N;
		this->marks = marks;
	}

	LineReader::~LineReader (void) {
	}

	void *
	LineReader::run (void * null) {
		this->running = false;
		char buf[4096];

		concurrent::ScopedMemoryLock mutex ((unsigned long int)this->fp, true);
		long int start = std::ftell(this->fp);
		long int offset = 0, delta = 0;
		long int & read_max = this->numberOfLinesToRead;
		
		for (int index = 1; index < 101; index++) {
			if ((this->startLine + read_max) < this->marks[index].line) {
				delta = std::abs(this->marks[index-1].line - this->startLine + 1);
				offset = this->marks[index-1].byte;
				break;
			}
		}
		
		std::fseek (this->fp, offset, SEEK_SET);

		// Munch lines to get to our starting point.
		while (delta-- > 0) {
			if (std::fgets (buf, 4096, this->fp) == NULL)		
				break;
      }
		
		for (long int ii = 0; ii < read_max; ii++) {
			if (std::fgets (buf, 4096, this->fp) == NULL)		
				break;
      
			this->dispatcher->onReadComplete (std::string (buf));
		}

		std::fseek (this->fp, start, SEEK_SET);
		
		this->running = false;
		this->dispatcher->removeWorker (this);
		return NULL;
	}

} // end of namespace
