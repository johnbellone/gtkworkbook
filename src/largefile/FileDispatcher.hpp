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
	 * \class FileDispatcher
	 * \ingroup Largefile
	 * \author jb (jvb4@njit.edu)
	 * \brief Front end interface to all of the file operations. This dispatcher will spawn separate
	 * threads for dealing with indexing and reading of the supported file types.
	 */
	class FileDispatcher : public proactor::InputDispatcher {
	private:
		FileIndex marks;
		std::string filename;
	public:
		/// Constructor.
		FileDispatcher (int e);

		/// Destructor.
		virtual ~FileDispatcher (void);

		/// Method which spawns the appropriate file opener as per the type.
		bool Openfile (const std::string & filename);

		/// Method which closes the file down inside of the system.
		bool Close (void);

		/// Method that is the "main loop" of the FileDispatcher's execution thread.
		void * run (void * null);

		/// Method that spawns a line reader with the appropriate start and number of lines.
		bool Readline (off64_t start, off64_t N);

		/// Method that spawns a offset reader with the appropriate start and number of lines.
		bool Readoffset (off64_t start, off64_t N);

		/// Method that spawns a percentage reader with the appropriate percent and number of lines.
		bool Readpercent (unsigned int percent, off64_t N);

		/// Method that starts the "index" operation for the file in question.
		void Index (void);
	};
	
}

#endif
