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
#include "Network.hpp"
#include <iostream>

#define IS_TERMINAL(c) ((*c == '\n') || (*c == '\r'))

namespace realtime {
	
	NetworkDispatcher::NetworkDispatcher (int e, proactor::Proactor * pro) {
		this->pro = pro;
		setEventId (e);
	}

	NetworkDispatcher::~NetworkDispatcher (void) {
	}

	void *
	NetworkDispatcher::run (void * null) {
		while (this->isRunning() == true) {
			this->inputQueue.lock();
      
			while (this->inputQueue.size() > 0) {
				if (this->isRunning() == false)
					break;

				this->pro->onReadComplete (this->inputQueue.pop());
			}

			this->inputQueue.unlock();

			concurrent::Thread::sleep(5);
		}

		return NULL;
	}
	
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
		if (this->socket != NULL) {
			this->socket->close();
			delete this->socket;
			this->socket = NULL;
		}
	}

	void *
	ConnectionThread::run (void * null) {
		int size = 0;
		size_t pos = 0;
		char * buf = new char[MAX_INPUT_SIZE];
		std::string packet;

		while (this->isRunning() == true) {
			packet.clear();

			while ((size = this->socket->receive (buf, MAX_INPUT_SIZE)) >= 0) {

				if ((this->isRunning() == false) || (size == 0)) {
					this->setRunning(false);
					break;
				}

				*(buf+size) = 0;
				packet.append(buf);

				while ((pos = packet.find_first_of('\n')) != std::string::npos) {
	  
					if (this->isRunning() == false)
						break;

					std::string str = packet.substr (0, pos);
					this->dispatcher->onReadComplete (str);
					packet = packet.substr (pos+1, packet.length());
				}
			}

			Thread::sleep (10);
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
		while (this->isRunning() == true) {
			int newfd = -1;
	
			if ((newfd = this->acceptor->acceptIncoming()) < 0) {
				break;
			}

			ConnectionThread * c = new ConnectionThread (this->dispatcher, newfd);
      
			if (this->dispatcher->addWorker (c) == false) {
				// Failed for some reason; cut out and quit for now.
				break;
			}
	
			Thread::sleep (10);
		}
		return NULL;
	}

} // end of namespace
