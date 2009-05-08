/* 
   File.cpp

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
#include "File.hpp"
#include <cstdio>

namespace largefile {

  Line::Line (int startByte, FILE * fp) {
    this->position = startByte;
    this->fp = fp;
    
    this->read();
  }

  Line::~Line (void) {
    this->line.clear();
  }

  void
  Line::read (void) {
    char buf[4096];

    if (this->line.length() > 0)
      return;

    std::fgets (buf, 4096, this->fp);

    this->line = buf;
  }

  File::File (FILE * fp) {
    this->fp = fp;
    
    reindex();
  }

  File::File (const std::string & path) {
    this->fp = std::fopen (path.c_str(), "r");

    reindex();
  }

  File::~File (void) {
  }

  void 
  File::reindex (void) {
    char buf[4096];
    long int pos = 0;

    this->lineIndex.clear();

    std::fseek (this->fp, 0, SEEK_SET);
    pos = std::ftell (this->fp);
    
    while (std::fgets (buf, 4096, this->fp) != NULL) {
	  //   this->lineIndex.push_back (pos);
      pos = std::ftell (this->fp);
    }

	//    this->lineIndex.push_back (pos);
  
    std::fseek (this->fp, 0, SEEK_SET);
  }

  Lines &
  File::getLines (long int S, long int T, Lines & V) {
    V.clear();

    if (S < 0) S = 0;
    
    if ((S < T) && (S < (signed)this->lineIndex.size())) {

      // Make sure we're not going out of bounds of the array.
      if (T > (signed)this->lineIndex.size()) T = this->lineIndex.size();
      
      for (long int ii = S; ii < T; ii++)
	V.push_back ( Line (this->lineIndex[ ii ], this->fp) );
    }
   
    return V;
  }

}
