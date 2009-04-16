/*
   Connection.cpp - Connection Object Source File

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
#include "Network.hpp"
#include <iostream>

#define IS_TERMINAL(c) ((*c == '\n') || (*c == '\r'))

namespace realtime {
  
  ConnectionThread::ConnectionThread (proactor::InputDispatcher * d, 
				      int newfd) {
    this->socket = new network::TcpClientSocket (newfd);
    this->dispatcher = d;
  }

  ConnectionThread::ConnectionThread (proactor::InputDispatcher * d,
				      network::TcpSocket * s) {
    this->socket = s;
    this->dispatcher = d;
  }

  ConnectionThread::~ConnectionThread (void) {
    delete socket;
  }

  void *
  ConnectionThread::run (void * null) {
    this->running = true;
    int size = 0;
    size_t pos = 0;
    char * buf = new char[MAX_INPUT_SIZE];
    std::string packet;

    while (this->running == true) {
      packet.clear();

      while ((size = this->socket->receive (buf, MAX_INPUT_SIZE)) > 0) {

	if (size == -1) {
	  this->running = false;
	  break;
	}

	*(buf+size) = 0;
	packet.append(buf);

	while ((pos = packet.find_first_of('\n')) != std::string::npos) {
	  this->dispatcher->onReadComplete (packet.substr (0, pos));
	  packet = packet.substr (pos+1, packet.length());
	}
      }

      Thread::sleep (100);
    }

    this->dispatcher->removeWorker (this);
    delete buf;
    return NULL;
  }

  AcceptThread::AcceptThread (network::TcpServerSocket::Acceptor * acceptor,
			      proactor::InputDispatcher * dispatcher) {
    this->dispatcher = dispatcher;
    this->acceptor = acceptor;
  }

  AcceptThread::~AcceptThread (void) {
    delete this->acceptor;
  }

  void *
  AcceptThread::run (void * null) {
    this->running = true;
    
    while (this->running == true) {
      int newfd = -1;
	
      if ((newfd = this->acceptor->acceptIncoming()) < 0) {
	this->running = false;
	break;
      }

      ConnectionThread * c = new ConnectionThread (this->dispatcher, newfd);
      
      if (c->start() == false) {
	// Failed for some reason; cut out and quit for now.
	break;
      }
	
      this->dispatcher->addWorker ( c );
      Thread::sleep (100);
    }

    return NULL;
  }

} // end of namespace
