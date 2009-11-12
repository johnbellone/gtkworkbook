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
#ifndef HPP_NETWORK
#define HPP_NETWORK

#include <network/Tcp.hpp>
#include <proactor/Worker.hpp>
#include <proactor/Proactor.hpp>
#include <iostream>
#include <memory>

#define MAX_INPUT_SIZE 1024

namespace realtime {

	class NetworkDispatcher : public proactor::InputDispatcher {
	public:
		typedef std::auto_ptr <NetworkDispatcher> AutoPtr;
	public:
		NetworkDispatcher (int e);
		virtual ~NetworkDispatcher (void);

		void * run (void * null);
	};
	
	class ConnectionThread : public proactor::Worker {
	private:
		bool purge_socket;
		network::TcpSocket * socket;
	public:
		ConnectionThread (int newfd);
		ConnectionThread (network::TcpSocket * s);
		virtual ~ConnectionThread (void);
  
		void * run (void * null);
	};

	class AcceptThread : public proactor::Worker {
	public:
		typedef std::auto_ptr <AcceptThread> AutoPtr;
	private:
		network::TcpServerSocket::Acceptor * acceptor;
	public:
		AcceptThread (network::TcpServerSocket::Acceptor * acceptor);
		virtual ~AcceptThread (void);

		void * run (void * null);
	};
} // end of namespace

#endif
