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
#ifndef FILEINDEX_HPP
#define FILEINDEX_HPP

#include <concurrent/Mutex.hpp>
#include <fcntl.h>
#include <cstdio>

#define LINE_INDEX_MAX 1001
#define LINE_PRECISION 1000 

namespace largefile {

	/***
	 * \class LineIndex
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief Simple structure for containing byte and line values.
	 */
	struct LineIndex {
		off64_t byte;
		off64_t line;
	};
	
	/***
	 * \class FileIndex
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief Object that contains the lookup table for the project. Once populated it will
	 * contain the absolute line number (and byte location) for fast seeking. Until it is
	 * populated it contains a relative byte position (to the total file size).
	 */
	class FileIndex : public concurrent::RecursiveMutex {
	private:
		LineIndex marks [LINE_INDEX_MAX];
	public:
		/// Default constructor (and only) constructor for the object.
		FileIndex (void);

		/// Destructor for the object.
		virtual ~FileIndex (void);

		/// Method to obtain a specific value from the line index.
		inline LineIndex & get (int index) { return this->marks[index]; }
	};

}

#endif
