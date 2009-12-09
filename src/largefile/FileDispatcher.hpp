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
#ifndef FILEDISPATCHER_HPP
#define FILEDISPATCHER_HPP

#include <proactor/InputDispatcher.hpp>
#include <string>
#include <header.h>
#include "FileIndex.hpp"

namespace largefile {

	/***
	 * \class AbstractFileDispatcher
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief Front end interface to all of the file operations. This dispatcher will spawn separate
	 * threads for dealing with indexing and reading of the supported file types.
	 */
	class AbstractFileDispatcher : public proactor::InputDispatcher {
	protected:
		FileIndex * marks;
		std::string filename;
	public:
		static AbstractFileDispatcher * CreateFromExtension (const std::string & filename, int e);
		
		/// Constructor.
		AbstractFileDispatcher (int e);
		AbstractFileDispatcher (int e, FileIndex * marks);
		
		/// Destructor.
		virtual ~AbstractFileDispatcher (void);

		virtual bool Openfile (const std::string & filename) = 0;
		virtual bool Closefile (void) = 0;

		virtual bool Readline (off64_t start, off64_t N) = 0;
		virtual bool Readoffset (off64_t start, off64_t N) = 0;
		virtual bool Readpercent (float percent, off64_t N) = 0;
		virtual void Index (void) = 0;
	};
	
}

#endif
