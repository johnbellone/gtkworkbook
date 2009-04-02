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
