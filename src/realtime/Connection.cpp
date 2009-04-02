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
#include "Connection.hpp"
#include <iostream>

#define IS_TERMINAL(c) ((*c == '\n') || (*c == '\r'))

Connection::Connection (int sockfd) {
  this->sockfd = sockfd;
}

Connection::~Connection (void) {
}

/*****************************************************************************/

ConnectionThread::ConnectionThread (proactor::Proactor * pro, int newfd) {
  this->socket = new Connection (newfd);
  this->pro = pro;
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
	  this->running = false;
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

	      this->pro->onReadComplete (this->getEventId(), q);
	      
	      q = (++p);
	      continue;
	    }
	  p++;
	}
      
      Thread::sleep (100);
    }

  this->pro->removeDispatcher (this);
  return NULL;
}
