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
#include "Gzip.hpp"
#include <sys/time.h>

using namespace largefile;

GnuzipDispatcher::GnuzipDispatcher (int e)
	:  AbstractFileDispatcher (e) {
}

GnuzipDispatcher::~GnuzipDispatcher (void) {
}

bool
GnuzipDispatcher::Openfile (const std::string & filename) {
	if (0 != strcmp (ZLIB_VERSION, "1.2.3")) {
		if (ZLIB_VERNUM < 0x1230) {
			std::cout << "Invalid ZLIB_VERSION ("<<ZLIB_VERSION<<") needs to be 1.2.3 or higher\n";
			return false;
		}
	}

	gzFile fp = NULL;
	if (NULL == (fp = gzopen (filename.c_str(), "rb"))) {
		return false;
	}

	gzseek (fp, 0L, SEEK_END);
	z_off_t byte_end = gztell (fp);
	
	std::cout << byte_end << "\n";
	
	this->marks.get(0).byte = 0;
	this->marks.get(0).line = 0;

	// Compute fuzzy relative position, and set line to -1 for indexing.
	for (int ii = 1; ii < LINE_INDEX_MAX; ii++) {
		double N = ii, K = LINE_PRECISION;
		this->marks.get(ii).byte = (off64_t)((N/K) * byte_end);
		this->marks.get(ii).line = -1;
	}

	gzclose (fp); fp = NULL;
	this->filename = filename;
	return true;
}

bool
GnuzipDispatcher::Closefile (void) {
	return true;
}

bool
GnuzipDispatcher::Readline (off64_t start, off64_t N) {
	return true;
}

bool
GnuzipDispatcher::Readoffset (off64_t start, off64_t N) {
	return true;
}

bool
GnuzipDispatcher::Readpercent (unsigned int percent, off64_t N) {
	return true;
}

void
GnuzipDispatcher::Index (void) {
}

GnuzipFileWorker::GnuzipFileWorker (const std::string & filename, FileIndex * marks)
	: AbstractFileWorker (filename, marks) {
	this->fp = NULL;
}

GnuzipFileWorker::~GnuzipFileWorker (void) {
}

bool
GnuzipFileWorker::Openfile (void) {
	if (NULL != this->fp) {
		// STUB: We need to throw an error here explaining that the file has already been opened. For
		// whatever reason someone attempted to call this method twice. That doesn't work.
		return false;
	}

	// A .gz file needs to be opened as "read-only binary" because we are going to be reading the compressed
	// data directly. The reader (and indexer) will handle decompression before it does its work.
	if (NULL == (this->fp = gzopen (this->filename.c_str(), "rb"))) {
		// STUB: Throw an error eventually to inform the user (in the GUI) of the problem.
		return false;
	}

	return true;
}

void *
GnuzipDispatcher::run (void * null) {
	return NULL;
}

bool
GnuzipFileWorker::Closefile (void) {
	// This is important for compressed files: we must write our index at this point when we know that we are
	// explicitly "closing" the file from our system. There can be no additional file handles open to the file
	// on disk.
	return false;
}

GnuzipLineIndexer::GnuzipLineIndexer (const std::string & filename, FileIndex * marks)
	: GnuzipFileWorker (filename, marks) {
}

GnuzipLineIndexer::~GnuzipLineIndexer (void) {
}

void *
GnuzipLineIndexer::run (void * null) {
	int ch, index = 0;
	z_off_t cursor = 0, count = 0, byte_beg = 0;
	struct timeval start, end;
	
	if (false == GnuzipFileWorker::Openfile()) {
		std::cerr << "Failed opening file descriptor in gz line indexer\n";
		return NULL;
	}

	gettimeofday (&start, NULL);
}
