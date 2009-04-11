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

Connection::Connection (int sockfd) {
  this->sockfd = sockfd;
}

Connection::~Connection (void) {
}

/*****************************************************************************/
NetworkDispatcher::NetworkDispatcher (proactor::Proactor * pro) {
  this->pro = pro;
}

NetworkDispatcher::~NetworkDispatcher (void) {
}

void *
NetworkDispatcher::run (void * null) {
  this->running = true;

  while (this->running == true)
    {
      // Dispatch all of the input items on the queue.
      this->inputQueue.lock();

      while (this->inputQueue.size() > 0)
	{
	  // For right now all we're doing is pushing up the chain.
	  this->pro->onReadComplete ( this->inputQueue.pop() );
	}

      this->inputQueue.unlock();

      Thread::sleep(100);
    }
  
  return NULL; 
}

/*****************************************************************************/

ConnectionThread::ConnectionThread (proactor::Dispatcher * d, int newfd) {
  this->socket = new Connection (newfd);
  this->dispatcher = d;
}

ConnectionThread::~ConnectionThread (void) {
  delete socket;
}

void *
ConnectionThread::run (void * null) {
  this->running = true;
  int size = 0;
  char buf[MAX_INPUT_SIZE];
  char * p = NULL, * q = NULL;

  while (this->running == true) 
    {
      if ((size = this->socket->receive (&buf[0], MAX_INPUT_SIZE)) <= 0)
	{
	  // 
	  break;
	}

      buf[size] = 0;
      
      q = p = &buf[0];

      while (p && (*p != '\0'))
	{
	  if (IS_TERMINAL (p))
	    {
	      *p = 0;

	      if (*(p+1) == '\n')
		p++;

	      this->dispatcher->onReadComplete (q);
	      
	      q = (++p);
	      continue;
	    }
	  p++;
	}
      
      Thread::sleep (100);
    }

  this->dispatcher->removeWorker (this);
  return NULL;
}

/*****************************************************************************/

AcceptThread::AcceptThread (TcpServerSocket::Acceptor * acceptor,
			    proactor::Dispatcher * dispatcher) {
  this->dispatcher = dispatcher;
  this->acceptor = acceptor;
}

AcceptThread::~AcceptThread (void) {
  delete this->acceptor;
}

void *
AcceptThread::run (void * null) {
  this->running = true;
  
  while (this->running == true)
    {
      int newfd = -1;
	
      if ((newfd = this->acceptor->acceptIncoming()) < 0)
	{
	  this->running = false;
	  break;
	}

      ConnectionThread * c = new ConnectionThread (this->dispatcher, newfd);
      
      if (c->start() == false)
	{
	  // Failed for some reason; cut out and quit for now.
	  break;
	}
	
	this->dispatcher->addWorker ( c );
	Thread::sleep (100);
      }

    return NULL;
}
