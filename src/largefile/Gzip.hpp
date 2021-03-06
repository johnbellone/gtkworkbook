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
#ifndef GNUZIP_HPP
#define GNUZIP_HPP

#include "FileIndex.hpp"
#include "FileWorker.hpp"
#include "FileDispatcher.hpp"
#include <tr1/memory>
#include <zlib.h>

namespace largefile {

#define GZIP_SPAN 1048576L
#define GZIP_WINSIZE 32768U
#define GZIP_CHUNK 16384
	
	struct GzipBlockData : public OffsetData {
		off64_t zin;
		off64_t zbits;
		unsigned char window [GZIP_WINSIZE];
	};
	
	/***
	 * \class GzipIndex
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class GzipIndex : public FileIndex {
	public:
		LookupTable * Add (off64_t byte,
								 off64_t line,
								 off64_t zin,
								 int bits,
								 unsigned int left,
								 unsigned char * window);
		void Relax (void);
	};

	typedef std::tr1::shared_ptr<GzipIndex> GzipIndexPtr;
	
	/***
	 * \class GnuzipDispatcher
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class GnuzipDispatcher : public AbstractFileDispatcher {
	public:
		/// Constructor.
		GnuzipDispatcher (int e);

		/// Destructor.
		virtual ~GnuzipDispatcher (void);

		bool Openfile (const std::string & filename);
		bool Closefile (void);

		bool Readline (off64_t start, off64_t N);
		bool Readoffset (off64_t start, off64_t N);
		bool Readpercent (float percent, off64_t N);
		void Index (void);
		
		void * run (void * null);
	};
	
	/***
	 * \class GnuzipFileWorker
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class GnuzipFileWorker : public AbstractFileWorker {
	protected:
		FILE * fp;
	
		bool InflateBlockAtOffset (off64_t offset, char * buf, size_t size);
	public:
		GnuzipFileWorker (const std::string & filename, FileIndexPtr marks);
		GnuzipFileWorker (const std::string & filename);
		virtual ~GnuzipFileWorker (void);

		bool Openfile (void);
		bool Closefile (void);
	};

	/***
	 * \class GnuzipLineReader
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class GnuzipLineReader : public GnuzipFileWorker {
	private:
		off64_t numberOfLinesToRead;
		off64_t startLine;
	public:
		GnuzipLineReader (const std::string & filename, FileIndexPtr marks, off64_t start, off64_t N);

		virtual ~GnuzipLineReader (void);

		void * run (void * null);
	};
	
	/***
	 * \class GnuzipBlockIndexer
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class GnuzipBlockIndexer : public GnuzipFileWorker {
	public:
		/// Constructor.
		GnuzipBlockIndexer (const std::string & filename, FileIndexPtr marks);

		/// Destructor.
		virtual ~GnuzipBlockIndexer (void);

		/// Method that acts as "main" for thread of execution.
		void * run (void * null);
	};
	
}

#endif
