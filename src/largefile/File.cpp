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
#include <glib.h>

namespace largefile {
	
	FileDispatcher::FileDispatcher (int e, proactor::Proactor * pro) {
		this->pro = pro;
		setEventId(e);
	}

	FileDispatcher::~FileDispatcher (void) {
	}

	bool
	FileDispatcher::Readline (off64_t start, off64_t N) {
		if (start > this->marks.get(LINE_INDEX_MAX-1).byte) return false;
		
		LineReader * reader = new LineReader (this, this->filename, &this->marks, start, N);
		this->addWorker (reader);
		return true;
	}

	bool
	FileDispatcher::Readoffset (off64_t offset, off64_t N) {
		if (offset > this->marks.get(LINE_INDEX_MAX-1).byte) return false;
		
		OffsetReader * reader = new OffsetReader (this, this->filename, offset, N);
		this->addWorker (reader);
		return true;
	}

	bool
	FileDispatcher::Readpercent (unsigned int percent, off64_t N) {
		if (percent > 100) return false;
		if (this->marks.get(percent * 10).byte == -1) return false;

		OffsetReader * reader = new OffsetReader (this, this->filename,
																this->marks.get(percent * 10).byte,
																N);
		this->addWorker (reader);
		return true;
	}

	void
	FileDispatcher::Index (void) {
		LineIndexer * indexer = new LineIndexer (this, this->filename, &this->marks);
		this->addWorker (indexer);
	}

	bool
	FileDispatcher::Openfile (const std::string & filename) {
		if (filename.length() == 0)
			return false;
		
		FILE * fp = NULL;
		if ((fp = fopen64 (filename.c_str(), "r")) == NULL) {
			// stub: throw an error somewhere
			return false;
		}

		// Take the relative byte position, e.g. .75 * byte_end, and we now have the a relative
		// line at that byte position for indexing at a later point in time.
		fseeko64 (fp, 0L, SEEK_END);
		off64_t byte_end = ftello64 (fp);

		this->marks.get(0).byte = 0;
		this->marks.get(0).line = 0;
		
		// Compute fuzzy relative position, and set line to -1 for indexing.
		for (int ii = 1; ii < LINE_INDEX_MAX; ii++) {
			double N = ii, K = LINE_PRECISION;
			this->marks.get(ii).byte = (off64_t)((N/K) * byte_end);
			this->marks.get(ii).line = -1;
		}

		fseeko64 (fp, 0L, SEEK_SET);

		fclose(fp);
		
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

	FileIndex::FileIndex (void) {
	}

	FileIndex::~FileIndex (void) {
	}
	
	FileWorker::FileWorker (const std::string & filename, FileIndex * marks)
		: marks (marks), filename (filename) {
		this->fp = NULL;
	}

	FileWorker::~FileWorker (void) {
		this->Closefile();
	}
	
	bool
	FileWorker::Openfile (void) {
		if (this->fp != NULL) return false;

		if ((this->fp = fopen64 (this->filename.c_str(), "r")) == NULL) {
			// stub: throw an error somewhere
			return false;
		}
		return true;
	}

	bool
	FileWorker::Closefile (void) {
		if (this->fp == NULL) return false;

		fclose (this->fp); this->fp = NULL;
		return true;
	}
	
	OffsetReader::OffsetReader (proactor::InputDispatcher * d,
										 const std::string & filename,
										 off64_t offset,
										 off64_t N)
		: FileWorker (filename, NULL) {
		this->startOffset = offset;
		this->numberOfLinesToRead = N;
		this->dispatcher = d;
	}

	OffsetReader::~OffsetReader (void) {
	}
	
	void *
	OffsetReader::run (void * null) {
		char buf[4096];
		int ch;

		if (FileWorker::Openfile () == false) {
			// STUB: throw some kind of error here; we failed opening the file.
			g_critical ("Failed opening file descriptor inside of OffsetReader.");
			return NULL;
		}
		
		off64_t start = ftello64 (this->fp);
		off64_t offset = 0;
		off64_t read_max = this->numberOfLinesToRead;

		fseeko64 (this->fp, this->startOffset, SEEK_SET);
		
		// We need to go to the beginning of the (next) line.
		while ((ch = fgetc (this->fp)) != EOF) {
			if (ch == '\n') {
				offset = ftello64 (this->fp);
				break;
			}
		}
			
		for (off64_t ii = 0; ii < read_max; ii++) {
			if (std::fgets (buf, 4096, this->fp) == NULL)		
				break;
      
			this->dispatcher->onReadComplete (buf);
		}

		fseeko64 (this->fp, start, SEEK_SET);
		
		this->dispatcher->removeWorker (this);
		this->Closefile();
		return NULL;
	}
		
	LineIndexer::LineIndexer (proactor::InputDispatcher * d,
									  const std::string & filename,
									  FileIndex * marks)
		: FileWorker (filename, marks) {
		this->dispatcher = d;
	}

	LineIndexer::~LineIndexer (void) {
	}

	void *
	LineIndexer::run (void * null) {
		int ch, index = 0;
		off64_t cursor = 0, count = 0, byte_beg = 0;
		struct timeval start, end;

		if (FileWorker::Openfile() == false) {
			// STUB: throw some kind of error here; we failed opening the file.
			g_critical ("Failed opening file descriptor in line indexer");
			return NULL;
		}
		
		std::cout<<"index start..."<<std::flush;
		
		gettimeofday (&start, NULL);
		
		// We need to get a absoltue line number from the relative position. We're not
		// going to get away from having to sequentially read this file in, but once we
		// have line numbers we can jump throughout the file pretty quickly.
		while ((ch = fgetc(this->fp)) != EOF) {
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

	LineReader::LineReader (proactor::InputDispatcher * d,
									const std::string & filename,
									FileIndex * marks,
									off64_t start,
									off64_t N)
		: FileWorker (filename, marks) {
		this->dispatcher = d;
		this->startLine = start;
		this->numberOfLinesToRead = N;
	}

	LineReader::~LineReader (void) {
	}

	void *
	LineReader::run (void * null) {
		char buf[4096];

		if (FileWorker::Openfile () == false) {
			// STUB: throw some kind of error here; we failed opening the file.
			g_critical ("Failed opening file descriptor in LineReader");
			return NULL;
		}
		
		off64_t start = ftello64 (this->fp);
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
		
		fseeko64 (this->fp, offset, SEEK_SET);

		// Munch lines to get to our starting point.
		while (delta > 0) {
			if (std::fgets (buf, 4096, this->fp) == NULL)		
				break;
			--delta;
      }
		
		for (off64_t ii = 0; ii < read_max; ii++) {
			if (std::fgets (buf, 4096, this->fp) == NULL)		
				break;
      
			this->dispatcher->onReadComplete (buf);
		}

		fseeko64 (this->fp, start, SEEK_SET);
		
		this->dispatcher->removeWorker (this);
		this->Closefile();
		return NULL;
	}

} // end of namespace
