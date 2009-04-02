/* 
   PacketParser.cpp - PacketParser Object Header File

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
#ifndef HPP_PACKETHANDLER
#define HPP_PACKETHANDLER

#include "proactor/Job.hpp"
#include "Packet.hpp"
#include <workbook/cell.h>
#include <workbook/sheet.h>
#include <workbook/workbook.h>

class PacketParser : public proactor::Job {
private:
  Workbook * wb;
  FILE * pktlog;
  Cell * cell;
public:
  PacketParser (Workbook * wb, FILE * pktlog) { 
    this->wb = wb;
    this->pktlog = pktlog;
    this->cell = cell_new();
  }
  
  virtual ~PacketParser (void) {
    cell->destroy (cell);
  }

  void * run (void * null) {
    this->running = true;
    Packet packet;
    const char * p = NULL;

    while (this->running == true)
      {
	while (this->inputQueue.size() > 0)
	  {
	    std::string buf = this->inputQueue.pop();
	    p = buf.c_str();

	    if (packet.parse (p) == FALSE)
	      {
		g_warning ("Failed parsing packet '%s'", buf.c_str());
		::sleep (1);
		continue;
	      }

	  switch (packet.getType())
	    {
	    default:
	      g_warning ("Invaild packet line '%s'", buf.c_str());
	      break;
	    /* ^time^type^sheet_name^sheet_name^after */
	  case Packet::TYPE_MOVESHEET:
	    {
	      if (packet.size() != 3)
		{
		  g_warning ("Packet::TYPE_MOVESHEET: Wrong packet format: %s",
			     buf.c_str());
		  break;
		}

	      Sheet * sheet = wb->get_sheet (wb, packet[0]);
	      if (IS_NULL (sheet))
		{
		  g_warning ("Failed moving sheet '%s' to %s sheet '%s'",
			 packet[0],
			 (strcmp(packet[2], "0") == 0) ? "after" : "before",
			 packet[1]);
		  break;
		}

	      if (wb->move_sheet (wb, sheet,
				  packet[1], 
				  atoi (packet[2]))
		  == FALSE)
		{
		  g_warning ("Failed moving sheet '%s'", packet[0]);
		  break;
		}
	    }
	    break;
	    /* ^time^type^sheet_name^loadpath */
	  case Packet::TYPE_LOADSHEET:
	    {
	      if (packet.size() != 2)
		{
		  g_warning ("Packet::TYPE_LOADSHEET: Wrong packet format: %s",
			     buf.c_str());
		  break;
		}

	      Sheet * sheet = wb->get_sheet (wb, packet[0]);
	      if (IS_NULL (sheet))
		{
		  g_warning ("Unable to load geometry file; sheet '%s'"
			     " does not exist", packet[0]);
		  break;
		}
	      
	      sheet->load (sheet, packet[1]);
	    }
	    break;
	    /* ^time^type^sheet_name^savepath */
	  case Packet::TYPE_SAVESHEET:
	    {
	      if (packet.size() != 2)
		{
		  g_warning ("Packet::TYPE_SAVESHEET: Wrong packet format: %s",
			     buf.c_str());
		  break;
		}

	      Sheet * sheet = wb->get_sheet (wb, packet[0]);
	      if (IS_NULL (sheet))
		{
		  g_warning ("Failed saving sheet '%s'; does not exist",
			     packet[0]);
		  break;
		}
	      
	      if (sheet->save (sheet, packet[1]) == FALSE)
		{
		  g_warning ("Unable to save sheet.");
		  break;
		}
	    }
	    break;
	    /* ^time^type^sheet_name^position */
	  case Packet::TYPE_MOVESHEETINDEX:
	    {
	      if (packet.size() != 2)
		{
		  g_warning ("Packet::TYPE_MOVESHEETINDEX: Wrong packet "
			     "format: %s", buf.c_str());
		  break;
		}

	      Sheet * sheet = wb->get_sheet (wb, packet[0]);
	      if (IS_NULL (sheet))
		{
		  g_warning ("Failed moving sheet '%s' to %d:"
			     " Does not exist", packet[0], atoi(packet[1]));
		  break;
		}
	      
	      if (wb->move_sheet_index (wb, sheet, atoi(packet[1])) 
		  == FALSE)
		{
		  g_warning ("Failed moving sheet '%s' to %d:"
			     " Invaild index\n", packet[0], atoi(packet[1]));
		}
	    }
	    break;
	    /* ^time^type^sheet_name */
	  case Packet::TYPE_REMSHEET:
	    {
	      if (packet.size() != 1)
		{
		  g_warning ("Packet::TYPE_REMSHEET: Wrong packet format: %s",
			     buf.c_str());
		  break;
		}

	      Sheet * sheet = wb->get_sheet (wb, packet[0]);
	      if (IS_NULL (sheet))
		{
		  g_warning ("Failed removing sheet '%s':"
			     " Does not exist", packet[0]);
		  break;
		}
	      wb->remove_sheet (wb, sheet);
	      sheet->destroy (sheet);
	    }
	    break;
	    /* ^time^type^sheet_name^max_row^max_column */
	  case Packet::TYPE_ADDSHEET:
	    {
	      if (packet.size() != 3)
		{
		  g_warning ("Packet::TYPE_ADDSHEET: Wrong packet format: %s",
			     buf.c_str());
		  break;
		}

	      wb->add_new_sheet (wb, 
				 packet[0],
				 atoi (packet[1]),
				 atoi (packet[2]));
	    }
	    break;
	    /* ^time^type^sheet_name^row^column^format^data */
	  case Packet::TYPE_UPDATECELL:
	    {
	      if (packet.size() != 5)
		{
		  g_warning ("Packet::TYPE_UPDATESHEET: Wrong packet "
			     "format: %s", buf.c_str());
		  break;
		}

	      Sheet * sheet = wb->get_sheet (wb, packet[0]);

	      if (IS_NULL (sheet))
		{
		  g_warning ("Invaild sheet name '%s': Does not exist",
			     packet[0]);
		  break;
		}
	            
	      cell->set_row (cell, atoi (packet[1]) );
	      cell->set_column (cell, atoi (packet[2]) );
	      
	      if (strlen (packet[3]) > 0)
		{
		  Map<String,String> fmt = packet.parseFormatString (packet[3]);
		  
		  if (fmt["bgcolor"].length() > 0)
		    cell->set_bgcolor (cell, fmt["bgcolor"].c_str());
		  if (fmt["fgcolor"].length() > 0)
		    cell->set_fgcolor (cell, fmt["fgcolor"].c_str());
		  if (fmt["justification"].length() > 0)
		    cell->set_justification (cell, 
					     (GtkJustification)
					     atoi (fmt["justification"].c_str()));
		}
	      
	      cell->set_value (cell, packet[4]);
	      
	      sheet->apply_cell (sheet, cell);
	      
	      g_message ("Cell (%d,%d) updated", cell->row, cell->column);
	    }
	    break;
	  }
	
	  fprintf (pktlog, "%s\n", buf.c_str());
	  fflush (pktlog);
	  ::sleep (1);
	  }
	::sleep (1);
      }

    return NULL;
  }
};

#endif
