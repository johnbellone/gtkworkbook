/* 
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
#ifndef FILE_HPP
#define FILE_HPP

#include <proactor/InputDispatcher.hpp>
#include <proactor/Proactor.hpp>
#include <proactor/Worker.hpp>
#include <cstdio>
#include <vector>
#include <string>

namespace largefile {

	struct LineIndex {
		long int byte;
		long int line;
	};
	
	class FileDispatcher : public proactor::InputDispatcher {
	private:
		LineIndex marks[101];
		FILE * fp;
		std::string filename;
	public:
		FileDispatcher (int e, proactor::Proactor * pro);
		virtual ~FileDispatcher (void);

		bool open (const std::string & filename);
		bool close (void);
		void * run (void * null);

		void read (long int start, long int N);
		void index (void);

		inline bool isIndexed(void) const { return (this->marks[100].line != -1); }
	};

	class LineIndexer : public proactor::Worker {
	private:
		LineIndex * marks;
		FILE * fp;
	public:
		LineIndexer (proactor::InputDispatcher * d, 
						 FILE * fp,
						 LineIndex * marks);
		virtual ~LineIndexer (void);

		void * run (void * null);
	};

	class LineReader : public proactor::Worker {
	private:
		FILE * fp;
		long int numberOfLinesToRead;
		long int startOffset;
	public:
		LineReader (proactor::InputDispatcher * d, 
						FILE * fp, 
						long int start,
						long int N);
		virtual ~LineReader (void);

		void * run (void * null);
	};

} // end of namespace

#endif
