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
#ifndef PLAINTEXT_HPP
#define PLAINTEXT_HPP

#include <proactor/Proactor.hpp>
#include <proactor/InputDispatcher.hpp>
#include "FileDispatcher.hpp"
#include "FileWorker.hpp"

namespace largefile {

	const int LINE_INDEX_MAX = 1001;
	const int LINE_PRECISION = 1000;
	
	/***
	 * \class PlaintextDispatcher
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class PlaintextDispatcher : public AbstractFileDispatcher {
	public:
		/// Constructor.
		PlaintextDispatcher (int e);

		/// Destructor.
		virtual ~PlaintextDispatcher (void);

		bool Openfile (const std::string & filename);
		bool Closefile (void);

		bool Readline (off64_t start, off64_t N);
		bool Readoffset (off64_t start, off64_t N);
		bool Readpercent (unsigned int percent, off64_t N);
		void Index (void);
		
		void * run (void * null);
	};
	
	/***
	 * \class PlaintextFileWorker
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class PlaintextFileWorker : public AbstractFileWorker {
	protected:
		FILE * fp;
	public:
		PlaintextFileWorker (const std::string & filename, FileIndex * marks);
		virtual ~PlaintextFileWorker (void);

		bool Openfile (void);
		bool Closefile (void);
	};
	
	/***
	 * \class PlaintextLineIndexer
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class PlaintextLineIndexer : public PlaintextFileWorker {
	public:
		/// Constructor.
		PlaintextLineIndexer (const std::string & filename, FileIndex * marks);

		/// Destructor.
		virtual ~PlaintextLineIndexer (void);

		/// Method that acts as "main" for thread of execution.
		void * run (void * null);
	};

	/***
	 * \class PlaintextOffsetReader
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class PlaintextOffsetReader : public PlaintextFileWorker {
	private:
		off64_t numberOfLinesToRead;
		off64_t startOffset;
	public:
		/// Constructor.
		PlaintextOffsetReader (const std::string & filename, off64_t offset, off64_t N);

		/// Destructor.
		virtual ~PlaintextOffsetReader (void);

		/// Method that acts as "main" for thread of execution.
		void * run (void * null);
	};

	/***
	 * \class PlaintextLineReader
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief
	 */
	class PlaintextLineReader : public PlaintextFileWorker {
	private:
		off64_t numberOfLinesToRead;
		off64_t startLine;
	public:
		/// Constructor.
		PlaintextLineReader (const std::string & filename, FileIndex * marks, off64_t start, off64_t N);

		/// Destructor.
		virtual ~PlaintextLineReader (void);

		/// Method that acts as "main" for thread of execution.
		void * run (void * null);
	};
	
}

#endif
