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
#ifndef HPP_PACKET
#define HPP_PACKET

#include <libgtkworkbook/cell.h>
#include <glib.h>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>

#define String std::string
#define Array  std::vector
#define Map    std::map

namespace realtime {

	class Packet {
	private:
		/* Members */
		String time;
		String type;
		Array<String> fields;
		gchar delimiter;
	public:
		const static int TYPE_UPDATECELL = 0;
		const static int TYPE_ADDSHEET = 1;
		const static int TYPE_REMSHEET = 2;
		const static int TYPE_MOVESHEETINDEX = 3;
		const static int TYPE_MOVESHEET = 4;
		const static int TYPE_SAVESHEET = 5;
		const static int TYPE_LOADSHEET = 6;
		const static int MAX_TYPES = 7;
		  
		Packet (void);
		~Packet (void);
  
		/* Methods */
		gboolean parse (const gchar *);
		Map<String,String> parseFormatString (const gchar *);

		inline gchar getDelimiter (void) const { return this->delimiter; }
		inline gint getType (void) const { return atoi (this->type.c_str()); }
		inline const gchar * getTime (void) const { return this->time.c_str(); }
		inline gint getFieldSize (void) const { return this->fields.size(); }
		inline const gchar * operator[] (gint index) const {
			return this->fields.at(index).c_str();
		}
		inline size_t size (void) const { return this->fields.size(); }
	};

} // end of namespace

#endif
