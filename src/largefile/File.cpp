/* 
   File.cpp

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
#include <concurrent/ScopedMemoryLock.hpp>
#include <cstdio>

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

  bool
  FileDispatcher::open (const std::string & filename) {
    if (filename.length() == 0)
      return false;

    if ((this->fp = fopen (filename.c_str(), "r")) == NULL) {
      // stub: throw an error somewhere
      return false;
    }

    concurrent::ScopedMemoryLock::addMemoryLock ((unsigned long int)this->fp);
    this->filename = filename;
    return true;
  }

  bool 
  FileDispatcher::close (void) {
    concurrent::ScopedMemoryLock mutex ((unsigned long int)this->fp);
    if (this->fp == NULL)
      return false;

    ::fclose (this->fp); this->fp = NULL;
    mutex.remove();
    return true;
  }

  void *
  FileDispatcher::run (void * null) {
    this->running = true;

    while (this->running == true) {

      if (this->fp == NULL) {
	this->running = false;
	break;
      }

      Thread::sleep (100);
    }

    return NULL;
  }

  LineIndexer::LineIndexer (proactor::InputDispatcher * d,
			    FILE * fp,
			    long int start,
			    long int N) {
    this->fp = fp;
    this->dispatcher = d;
    this->startOffset = start;
    this->numberOfLinesToRead = N;
  }

  LineIndexer::~LineIndexer (void) {

  }

  void *
  LineIndexer::run (void * null) {
    this->running = true;
    char buf[4096];

    concurrent::ScopedMemoryLock mutex ((unsigned long int)this->fp, true);

    // Record current position and seek to where we're going to start.
    long int cursor = ::ftell (this->fp);
    ::fseek (this->fp, this->startOffset, SEEK_SET);

    for (long int ii = 0; ii < this->numberOfLinesToRead; ii++) {
      
      if (fgets (buf, 4096, this->fp) == NULL) {
	
	break;
      }
      
      long int pos = ::ftell (this->fp);
    }

    // stub: Push up to our pappy.

    this->running = false;
    ::fseek (this->fp, cursor, SEEK_SET);
    return NULL;
  }

  LineReader::LineReader (proactor::InputDispatcher * d,
			  FILE * fp,
			  long int start,
			  long int N) {
    this->fp = fp;
    this->dispatcher = dispatcher;
    this->startOffset = start;
    this->numberOfLinesToRead = N;
  }

  LineReader::~LineReader (void) {
  
  }

  void *
  LineReader::run (void * null) {
    this->running = false;
    char buf[4096];

    concurrent::ScopedMemoryLock mutex ((unsigned long int)this->fp, true);

    // Record current position and seek to where we're going to start.
    long int cursor = ::ftell (this->fp);
    ::fseek (this->fp, this->startOffset, SEEK_SET);

    for (long int ii = 0; ii < this->numberOfLinesToRead; ii++) {
      
      if (fgets (buf, 4096, this->fp) == NULL) {
	
	break;
      }
      
      long int pos = ::ftell (this->fp);
      
      // stub: store index and buf
    }

    // stub: Push up to our pappy.

    this->running = false;
    ::fseek (this->fp, cursor, SEEK_SET);
    return NULL;
  }

} // end of namespace
