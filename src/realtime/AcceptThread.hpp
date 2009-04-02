/* 
   AcceptThread.hpp - Acceptor Thread Header File

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
#ifndef HPP_ACCEPTTHREAD
#define HPP_ACCEPTTHREAD

#include "network/Tcp.hpp"
#include "proactor/Dispatcher.hpp"
#include "Connection.hpp"
#include <iostream>

/* @description: This object runs as a separate thread accepting incoming
   connections from a TcpServerSocket object.
   @author: John `jb Bellone (jvb4@njit.edu) */
class AcceptThread : public proactor::Dispatcher {
private:
  TcpServerSocket::Acceptor * acceptor;
public:
  AcceptThread (TcpServerSocket::Acceptor * acceptor, 
		proactor::Proactor * pro) {
    this->pro = pro;
    this->acceptor = acceptor;
  }

  virtual ~AcceptThread (void) {
      delete acceptor;
  }

  void * run (void * null) {
    this->running = true;
  
    while (this->running == true)
      {
	int newfd = -1;
	
	if ((newfd = this->acceptor->acceptIncoming()) < 0)
	  {
	    this->running = false;
	    break;
	  }

	ConnectionThread * c = new ConnectionThread (this->pro, newfd);
	c->setEventId ( this->getEventId() );

	if (c->start() == false)
	  {
	    // Failed for some reason; cut out and quit for now.
	    break;
	  }
	
	this->pro->addDispatcher ( c );
	Thread::sleep (100);
      }

    return NULL;
  }
};

#endif
