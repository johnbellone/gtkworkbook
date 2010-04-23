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
#ifndef FILEWORKER_HPP
#define FILEWORKER_HPP

#include <proactor/Worker.hpp>
#include <string>
#include <cstdio>
#include "FileIndex.hpp"

namespace largefile {

	/***
	 * \class AbstractFileWorker
	 * \ingroup Largefile
	 *
	 * \brief Inheritable object specifically meant for dealing with files inside of the
	 * largefile plugin system. Any specific file filters will extend this worker. 
	 * 
	 * \author jb (jvb4@njit.edu)
	 */
	class AbstractFileWorker : public proactor::Worker {
	protected:
		FileIndexPtr marks;
		std::string filename;
	public:
		/// Constructor with the required filename and fileindex parameters.
		AbstractFileWorker (const std::string & filename, FileIndexPtr marks);
		AbstractFileWorker (const std::string & filename);
		
		/// Destructor needed for a clean teardown.
		virtual ~AbstractFileWorker (void);

		/// File method needed to handle opening a specific file type.
		virtual bool Openfile (void) = 0;

		/// File method needed to handle closing a specific file type.
		virtual bool Closefile (void) = 0;
	};

}

#endif
