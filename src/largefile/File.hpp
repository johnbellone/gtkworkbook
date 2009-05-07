/* 
   File.hpp - File Object handlers

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
#include <proactor/Worker.hpp>
#include <cstdio>
#include <vector>
#include <string>

namespace largefile {

  class Line {
  private:
    FILE * fp;
    int position;
    std::string line;
  public:
    Line (int startByte, FILE * fp);
    ~Line (void);
    
    void read (void);

    int getByteStart (void) const { return this->position; }
    std::string getLine (void) const { return this->line; }
  };

  typedef std::vector<Line> Lines;

  class File {
  private:
    typedef std::vector<long int> LineIndexMap;
    
    FILE * fp;
    LineIndexMap lineIndex;
  public:
    File (FILE * fp);
    File (const std::string & path);
    ~File (void);

    void reindex (void);
    Lines & getLines (long int S, long int T, Lines & V);
  };

} // end of namespace

#endif
