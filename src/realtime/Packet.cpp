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
#include "Packet.hpp"

#define WORD(d,buf,word) if (buf.length() > 0) {	\
		size_t index = buf.find_first_of (d, 0);		\
		if (index == std::string::npos) {				\
			word = buf;											\
			buf.clear();										\
		}															\
		else {													\
			word = buf.substr (0, index);					\
			buf = buf.substr (index+1, buf.length());	\
		}															\
	}

namespace realtime {

  Packet::Packet (void) {
    this->delimiter = '\0';
    this->type = -1;
  }

  Packet::~Packet (void) {
    this->time.clear();
    this->fields.clear();
  }

  gboolean
  Packet::parse (const gchar * buf) {
    this->fields.clear();

    if (IS_NULLSTR (buf)) 
      return FALSE;
  
    this->delimiter = buf[0];

    String line(&buf[1]), value;

    WORD (this->delimiter, line, this->time);
    WORD (this->delimiter, line, this->type);

    while (line.length() > 0) {
      WORD (this->delimiter, line, value);
      this->fields.push_back (value);
    }

    return TRUE;
  }

  Map<String,String>
  Packet::parseFormatString (const gchar * buf) {
    String fmt(buf);
    Map<String,String> pairs; pairs.clear();

    size_t pos = 0, length = fmt.length(), comma = 0, equal = 0;
    String key, val;

    while (pos < length) {
      if ((comma = fmt.find_first_of (',')) == String::npos)
	comma = length;
      
      if ((equal = fmt.find_first_of ('=')) == String::npos)
	break;

      key = fmt.substr (0, equal);
      val = fmt.substr (equal+1, comma-equal-1);

      pairs[key] = val;

      if (comma == length)
	break;

      fmt = fmt.substr (comma+1, length);
      length = fmt.length();
    }
    return pairs;
  }
}
