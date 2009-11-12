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

#include "FileWorker.hpp"
#include "FileDispatcher.hpp"
#include <zlib.h>

namespace largefile {

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
		bool Readpercent (unsigned int percent, off64_t N);
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
		gzFile fp;
	public:
		GnuzipFileWorker (const std::string & filename, FileIndex * marks);
		virtual ~GnuzipFileWorker (void);

		bool Openfile (void);
		bool Closefile (void);
	};

	/***
	 * \class GnuzipLineIndexer
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class GnuzipLineIndexer : public GnuzipFileWorker {
	private:
	public:
		/// Constructor.
		GnuzipLineIndexer (const std::string & filename, FileIndex * marks);

		/// Destructor.
		virtual ~GnuzipLineIndexer (void);

		/// Method that acts as "main" for thread of execution.
		void * run (void * null);
	};
	
}

#endif
