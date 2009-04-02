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
