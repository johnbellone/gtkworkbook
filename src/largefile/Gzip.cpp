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
#include <proactor/Proactor.hpp>

using namespace largefile;

LookupTable *
GzipIndex::Add (off64_t byte, off64_t line, off64_t zin, int bits, unsigned int left, unsigned char * window) {
	LineOffset * x = NULL;

	this->lock();

	if (NULL == FileIndex::Add (byte, line)) return NULL;

	x = this->table->list + this->table->have;

	GzipBlockData * block = reinterpret_cast <GzipBlockData *> (malloc (sizeof (GzipBlockData))); 
	
	x->extra = static_cast<OffsetData *> (block);
	block->zin   = zin;
	block->zbits = bits;

	if (left)
		memcpy (block->window, window + GZIP_WINSIZE - left, left);
	else
		memcpy (block->window + left, window, GZIP_WINSIZE - left);

	this->unlock();
	
	return this->table;
}

GnuzipDispatcher::GnuzipDispatcher (int e)
	: AbstractFileDispatcher (e, new GzipIndex) {
}

GnuzipDispatcher::~GnuzipDispatcher (void) {
}

bool
GnuzipDispatcher::Openfile (const std::string & filename) {
	unsigned char gzheader[10];

	// Just make sure that we are dealing with a stable release. This was the tagged release that
	// ships with RHEL / CentOS 5.0; also happens to be the release with Ubuntu 9.10. 
	if (0 != strcmp (ZLIB_VERSION, "1.2.3")) {
		if (ZLIB_VERNUM < 0x1230) {
			std::cout << "Invalid ZLIB_VERSION ("<<ZLIB_VERSION<<") needs to be 1.2.3 or higher\n";
			return false;
		}
	}
	
	FILE * fp = NULL;
	if (NULL == (fp = FOPEN (filename.c_str(), "rb"))) {
		std::cerr << "Failed opening "<<filename<<" for binary gzip reading.\n";
		return false;
	}

	// Make sure that we are dealing with a gzip compressed file. If it is some other type of file
	// we should not attempt to open or index it. 
	fread (gzheader, 1, 10, fp);

	// (1) Check to make sure the file's identification bytes indicate that it is a gzip file.
	// (2) Be sure that the compression method used inside of the file is the DEFLATE so that we
	// know it can be uncompressed using zlib - this should never, ever, fail. But just in case.
	// From Gzip RFC: http://www.gzip.org/zlib/rfc-gzip.html
	if (0x1f != gzheader[0] || 0x8b != gzheader[1]) {
		std::cerr << "Input file "<<filename<<" is not gzip formatted.\n";
		return false;
	}

	if (8 != gzheader[2]) {
		std::cerr << "Input file "<<filename<<" uses compression method other than deflate.\n";
		return false;
	}

	// At this point we know that we have a gzip file that was compressed with the DEFLATE algorithm.
	
	
	this->filename = filename;
	return true;
}

bool
GnuzipDispatcher::Closefile (void) {
	// STUB: Write out the file index to the end of the Gzip file on disk.
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
	GnuzipLineIndexer * indexer = new GnuzipLineIndexer (this->filename, this->marks);
	this->addWorker (indexer);
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
	// data directly from disk and using the libraries inflate algorithm to decompress.
	if (NULL == (this->fp = FOPEN (this->filename.c_str(), "rb"))) {
		// STUB: Throw an error eventually to inform the user (in the GUI) of the problem.
		return false;
	}

	// We can forego any explicit checks here because we already performed that inside of the GnuzipDispatcher
	// Openfile method. 
	return true;
}

void *
GnuzipDispatcher::run (void * null) {
	this->Index();
	
	while (true == this->isRunning()) {
		while (0 == this->inputQueue.size()) {
			if (false == this->isRunning())
				return NULL;
			concurrent::Thread::sleep(1);
		}

		this->pro->onReadComplete (this->inputQueue.pop());
	}
	
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
	GzipIndex * index = static_cast <GzipIndex *> (this->marks);
	double ms;
	struct timeval start, end;
	int ret;
	off64_t total_in = 0, total_out = 0;
	off64_t last = 0; 
	LookupTable * table = NULL;
	z_stream zstrm;
	unsigned char input[GZIP_CHUNK];
	unsigned char window[GZIP_WINSIZE];
	
	if (false == GnuzipFileWorker::Openfile()) {
		std::cerr << "Failed opening file descriptor in gz line indexer\n";
		return NULL;
	}

	// Iniitalize the z_stream struct and inflate
	zstrm.zalloc = Z_NULL;
	zstrm.zfree = Z_NULL;
	zstrm.opaque = Z_NULL;
	zstrm.avail_in = 0;
	zstrm.avail_out = 0;
	zstrm.next_in = Z_NULL;

	if (Z_OK != (ret = inflateInit2 (&zstrm, 47)))
		return NULL;

	std::cout << "index starting...";
	
	gettimeofday (&start, NULL);

	do {
		while (false == this->marks->trylock())
			Thread::sleep(1);

		if (false == this->isRunning())
			return NULL;
		
		if (0 == (zstrm.avail_in = fread (input, 1, GZIP_CHUNK, this->fp))) {
			ret = Z_DATA_ERROR;
			goto build_index_error;
		}

		if (ferror (this->fp)) {
			ret = Z_ERRNO;
			goto build_index_error;
		}

		zstrm.next_in = input;

		// Process all of the data that was just read in from [this->fp]
		do {
			
			if (0 == zstrm.avail_out) {
				zstrm.avail_out = GZIP_WINSIZE;
				zstrm.next_out = window;
			}

			total_in += zstrm.avail_in;
			total_out += zstrm.avail_out;

			if (Z_NEED_DICT == (ret = inflate (&zstrm, Z_BLOCK))) {
				ret = Z_DATA_ERROR;
			}
			else if (Z_MEM_ERROR == ret || Z_DATA_ERROR == ret) {
				goto build_index_error;
			}
			else if (Z_STREAM_END == ret)
				break;

			total_in -= zstrm.avail_in;
			total_out -= zstrm.avail_out;

			if ((zstrm.data_type & 128) && !(zstrm.data_type & 64) &&
				 (0 == total_out || (total_out - last > GZIP_SPAN))) {
				// Add the point to our GzipFileIndex which will in turn do all the fancy things underneath.
				table = index->Add (total_out,
										  -1,
										  total_in,
										  zstrm.data_type & 7,
										  zstrm.avail_out,
										  window);
				if (NULL == table) {
					ret = Z_MEM_ERROR;
					goto build_index_error;
				}
				last = total_out;
			}
		} while (zstrm.avail_in != 0);

		this->marks->unlock();
		
	} while (ret != Z_STREAM_END);
	
	gettimeofday (&end, NULL);

	ms = ((((end.tv_sec-start.tv_sec) * 1000) + ((end.tv_usec-start.tv_usec)/1000.0)) + 0.5);
	std::cout<<"ready (ms:"<<ms<<")!\n"<<std::flush;

	std::cout << ((table->have * sizeof (GzipBlockData) * sizeof (LineOffset))) << "\n";
	
	inflateEnd (&zstrm);
	return NULL;
	
	build_index_error:
	std::cerr << "Failed indexing!\n";
	inflateEnd (&zstrm);
	this->marks->Free();
	return NULL;
}
