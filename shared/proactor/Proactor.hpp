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
#ifndef HPP_PROACTOR_PROACTOR
#define HPP_PROACTOR_PROACTOR

#include "../concurrent/Thread.hpp"
#include "../concurrent/Map.hpp"
#include "../concurrent/Queue.hpp"
#include "../concurrent/List.hpp"
#include "Worker.hpp"
#include "Event.hpp"
#include "Dispatcher.hpp"
#include "InputDispatcher.hpp"

namespace proactor {

	class Proactor : public Dispatcher {
	private:
		typedef concurrent::List<Dispatcher *> DispatcherList;
		typedef concurrent::Map<int, WorkerListType *> EventMapType;
		typedef concurrent::Queue<Event> EventQueueType;

		EventMapType eventsToHandlers;
		DispatcherList dispatchers;
		EventQueueType events;
	public:
		Proactor (void);
		virtual ~Proactor (void);

		bool addWorker (int e, Worker * w);
		bool removeWorker (int e, Worker * w);
		void addDispatcher (Dispatcher * d);
		bool removeDispatcher (Dispatcher * d);
    
		void * run (void * null);
 
		void onReadComplete (Event e);
		void onReadComplete (int e, const char * buf);

		inline const std::string & peekInputQueue (void) {
			return (this->events.front()).buf;
		}
	};

} // end of namespace

#endif
