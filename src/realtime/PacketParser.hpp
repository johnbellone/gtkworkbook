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
#ifndef HPP_PACKETHANDLER
#define HPP_PACKETHANDLER

#include <proactor/Worker.hpp>
#include "Packet.hpp"
#include <libgtkworkbook/cell.h>
#include <libgtkworkbook/sheet.h>
#include <libgtkworkbook/workbook.h>
#include <string>
#include <queue>
#include <ctime>
#include <memory>

namespace realtime {

	class PacketParser : public proactor::Worker {
	public:
		typedef std::auto_ptr <PacketParser> AutoPtr;
	private:
		Workbook * wb;
		FILE * pktlog;
		Cell * cell;
		int verbosity;
	public:
		PacketParser (Workbook * wb, FILE * pktlog, int verbosity);
		virtual ~PacketParser (void);

		void * run (void * null);
	};
	
} // end of namespace

#endif
