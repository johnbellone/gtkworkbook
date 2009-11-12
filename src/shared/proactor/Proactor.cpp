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
#include "Proactor.hpp"
#include <iostream>
#include <algorithm>

namespace proactor {

	Proactor::Proactor (void) {
	}

	Proactor::~Proactor (void) {
		// This is a hack right now:
		// We do the teardown for a thread inside of the Thread object. This is merely
		// replicating the feature because there is a problem with the lock inside of the
		// queue object, e.g., we need to implement a pthread wait condition.
		if (this->isRunning() == true) {
			this->setRunning (false);
			this->join();
		}
		
		// Remove all of the lists of event handlers.
		{
			EventMapType::iterator it = this->eventsToHandlers.begin();
			while (it != this->eventsToHandlers.end()) {
				WorkerListType * q = (it->second);
				delete q;
				it++;
			}
		}
 
		// Handle the dispatchers that have not been manually removed.
		{
			DispatcherList::iterator it = this->dispatchers.begin();
			while (it != this->dispatchers.end())
			{
				Dispatcher * d = (*it);
				delete d;
				it++;
			}
		}
 	}

	bool
	Proactor::addWorker (int e, Worker * job) {
		this->eventsToHandlers.lock();
		{
			EventMapType::iterator it = this->eventsToHandlers.find (e);
  
			if (it == this->eventsToHandlers.end())
				this->eventsToHandlers[e] = new WorkerListType;  
			this->eventsToHandlers[e]->push_back (job);
		}
		this->eventsToHandlers.unlock();

		job->dispatcher = this;
		
		return job->start();
	}

	void
	Proactor::onReadComplete (Event e) {
		this->events.push (e);
	}

	void 
	Proactor::onReadComplete (int e, const char * buf) {
		this->events.push ( Event (e, std::string (buf)) );   
	}

	bool
	Proactor::removeWorker (int e, Worker * job) {
		bool result = false;

		this->eventsToHandlers.lock();
		{
			WorkerListType::iterator it = 
				std::find (this->eventsToHandlers[e]->begin(),
							  this->eventsToHandlers[e]->end(),
							  job);
    
			if (it != this->eventsToHandlers[e]->end())
			{
				this->eventsToHandlers[e]->erase (it);
				result = true;
			}
		}
		this->eventsToHandlers.unlock();

		job->dispatcher = NULL;
		return result;
	}

	void
	Proactor::addDispatcher (Dispatcher * d) {
		d->pro = this;
		this->dispatchers.push_back (d);
	}

	bool
	Proactor::removeDispatcher (Dispatcher * d) {
		DispatcherList::iterator it = std::find (this->dispatchers.begin(),
															  this->dispatchers.end(),
															  d);

		if (it == this->dispatchers.end())
			return false;
  
		this->dispatchers.erase (it);

		d->pro = NULL;
		return true;
	}

	void *
	Proactor::run (void * null) {
		WorkerListType::iterator it;

		while (this->isRunning() == true) {
			while (this->events.size() == 0) {
				if (this->isRunning() == false)
					return NULL;
				Thread::sleep(1);
			}

			// STUB: Should probably copy the events queue into a local one to free up
			// the resource, e.g. the spinlock at the top might never let go if we continually
			// add items to the queue.
			Event e = this->events.pop();

			// We are throwing events with no handlers to catch them.
			if (this->eventsToHandlers.find (e.id) == 
				 this->eventsToHandlers.end())
				continue;
 
			it = this->eventsToHandlers[e.id]->begin();
	  
			while (it != this->eventsToHandlers[e.id]->end()) {
				Worker * j = (*it);
	      
				j->pushInputQueue (e.buf);
		      
				it++;
			}
		}
		return NULL;
	}

} // end of namespace
