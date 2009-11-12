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
#ifndef HPP_PROACTOR_EVENT
#define HPP_PROACTOR_EVENT

#include "Dispatcher.hpp"
#include <string>

namespace proactor {

	class Proactor;
	
	class Event {
	private:
		static int numEvents;
	public:
		int id;
		std::string buf;

		Event (int id, const std::string & buf);

		static int uniqueEventId (void);
	};

	class EventDispatcher : public Dispatcher {
	private:
		int eventId;
	public:
		virtual ~EventDispatcher (void);

		inline void setEventId (int e) { 
			this->eventId = e;
		}

		inline int getEventId (void) { 
			return this->eventId;
		}
	};

}

#endif
