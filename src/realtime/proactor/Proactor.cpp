/* 
   Proactor.cpp - Proactor Object Source File

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
using namespace proactor;

Proactor::Proactor (void) {
}

Proactor::~Proactor (void) {
  // Remove all of the lists of event handlers.
  {
    EventMapType::iterator it = this->eventsToHandlers.begin();
    while (it != this->eventsToHandlers.end())
      {
	EventHandlers * q = (it->second);
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

void
Proactor::registerHandler (int e, Job * job) {
  this->eventsToHandlers.lock();
  {
    EventMapType::iterator it = this->eventsToHandlers.find (e);
  
    if (it == this->eventsToHandlers.end())
      this->eventsToHandlers[e] = new EventHandlers;  
    this->eventsToHandlers[e]->push_back (job);
  }
  this->eventsToHandlers.unlock();
}

void 
Proactor::onReadComplete (int e, const char * buf) {
  this->inputQueue.push ( Event (e, std::string (buf)) );   
}

bool
Proactor::unregisterHandler (int e, Job * job) {
  bool result = false;

  this->eventsToHandlers.lock();
  {
    EventHandlers::iterator it = 
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
   
  return result;
}

void
Proactor::addDispatcher (Dispatcher * d) {
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
  return true;
}

void *
Proactor::run (void * null) {
  this->running = true;
  
  EventHandlers::iterator it;

  while (this->running == true)
    {
      this->inputQueue.lock();
      while (this->inputQueue.size() > 0)
	{
	  Event e = this->inputQueue.pop();

	  // We are throwing events with no handlers to catch them.
	  if (this->eventsToHandlers.find (e.id) == 
	      this->eventsToHandlers.end())
	    continue;
 
	  it = this->eventsToHandlers[e.id]->begin();
	  
	  while (it != this->eventsToHandlers[e.id]->end())
	    {
	      Job * j = (*it);
	      
	      j->pushInputQueue (e.buf);
		      
	      it++;
	    }
	}
      this->inputQueue.unlock();
      
      Thread::sleep(100);
    }
  return NULL;
}
