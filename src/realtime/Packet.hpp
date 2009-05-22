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

#include <workbook/cell.h>
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
    enum {
      TYPE_UPDATECELL = 0,
      TYPE_ADDSHEET,
      TYPE_REMSHEET,
      TYPE_MOVESHEETINDEX,
      TYPE_MOVESHEET,
      TYPE_SAVESHEET,
      TYPE_LOADSHEET,
      /**/
      MAX_TYPES
    };
  
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
