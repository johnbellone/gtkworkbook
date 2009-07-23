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

#define LINE_INDEX_MAX 1001
#define LINE_PRECISION 10000

namespace largefile {

	struct LineIndex {
		off64_t byte;
		off64_t line;
	};
	
	class FileDispatcher : public proactor::InputDispatcher {
	private:
		LineIndex marks[LINE_INDEX_MAX];
		FILE * fp;
		std::string filename;
	public:
		FileDispatcher (int e, proactor::Proactor * pro);
		virtual ~FileDispatcher (void);

		bool OpenFile (const std::string & filename);
		bool Close (void);
		void * run (void * null);

		bool Readline (off64_t start, off64_t N);
		bool Readoffset (off64_t offset, off64_t N);
		bool Readpercent (guint percent, off64_t N);
		
		void Index (void);

		inline bool isIndexed(void) const { return (this->marks[LINE_INDEX_MAX-1].line != -1); }
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

	class OffsetReader : public proactor::Worker {
	private:
		FILE * fp;
		off64_t numberOfLinesToRead;
		off64_t startOffset;
	public:
		OffsetReader (proactor::InputDispatcher * d,
						  FILE * fp,
						  off64_t offset,
						  off64_t N);
		virtual ~OffsetReader (void);

		void * run (void * null);
	};
	
	class LineReader : public proactor::Worker {
	private:
		FILE * fp;
		off64_t numberOfLinesToRead;
		off64_t startLine;
		LineIndex * marks;
	public:
		LineReader (proactor::InputDispatcher * d, 
						FILE * fp,
						LineIndex * marks,
						off64_t start,
						off64_t N);
		virtual ~LineReader (void);

		void * run (void * null);
	};

} // end of namespace

#endif
