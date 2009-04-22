/* 
   Tcp.hpp - Tcp Implementation Source File

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
#include "Tcp.hpp"
#include <iostream>

namespace network {

  TcpSocket::TcpSocket (void) {
    this->sockfd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
  }

  TcpSocket::~TcpSocket (void) {
    this->close();
  }

  void
  TcpSocket::close (void) {
    if (this->sockfd > 0)
      ::close (this->sockfd);
  }
	
  int
  TcpSocket::send (const char * bytes, size_t length) {
    return ::send (this->sockfd, bytes, length, 0);
  }

  int
  TcpSocket::receive (char * bytes, size_t size) {
    return ::recv (this->sockfd, bytes, size, 0);
  }

  TcpServerSocket::TcpServerSocket (int port) : TcpSocket() {
    this->port = port;

    memset (&(this->sockaddr), 0, sizeof (this->sockaddr));
    this->sockaddr.sin_family = AF_INET;
    this->sockaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    this->sockaddr.sin_port = htons (this->port);
  }

  TcpServerSocket::~TcpServerSocket (void) {
    this->close();
  }

  bool
  TcpServerSocket::start (int backlog = 5) {
    int opt = 1;

    // This is to prevent conflicts with major services' ports.
    if (this->port < 1024)
      return false;

    if (::setsockopt (this->sockfd, 
		      SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) < 0)
      return false;

    if (::bind (this->sockfd,
		(struct sockaddr *)&(this->sockaddr),
		sizeof (this->sockaddr)) < 0)
      return false;

    if (::listen (this->sockfd, backlog) < 0)
      return false;

    return true;
  }

  void
  TcpServerSocket::close (void) {
    TcpSocket::close();
  }

  TcpServerSocket::Acceptor *
  TcpServerSocket::newAcceptor (void) {
    return new TcpServerSocket::Acceptor (this, this->sockfd);
  }

  TcpServerSocket::Acceptor::Acceptor (TcpServerSocket * server, int sockfd) {
    this->socket = server;
    this->sockfd = sockfd;
  }

  int
  TcpServerSocket::Acceptor::acceptIncoming (void) {
    int newfd = -1;
    static struct sockaddr_in clientaddr;
    unsigned int x = sizeof (clientaddr);

    if ((newfd = ::accept (this->sockfd, 
			   (struct sockaddr *)&clientaddr,
			   &x)) < 0)
      return -1;
    return newfd;
  }

  TcpClientSocket::TcpClientSocket (void) : TcpSocket() {
    memset (&(this->sockaddr), 0, sizeof (this->sockaddr));
    this->sockaddr.sin_family = AF_INET;
    this->sockaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  }

  TcpClientSocket::TcpClientSocket (int newfd) {
    this->sockfd = newfd;
  }
				
  TcpClientSocket::~TcpClientSocket (void) {
  }

  bool
  TcpClientSocket::connect (const char * host, int port) {
    if (!host || (*host == '\0'))
      return false;

    if ((this->hp = ::gethostbyname (host)) == NULL)
      return false;
 
    // Copy over the hostname address.
    memset (&(this->sockaddr),0, sizeof (this->sockaddr));
    this->sockaddr.sin_family = AF_INET;
    this->sockaddr.sin_addr.s_addr 
      = ((struct in_addr *)(this->hp->h_addr))->s_addr;
    this->sockaddr.sin_port = htons (port);

    if (::connect (this->sockfd,
		   (struct sockaddr *)&(this->sockaddr),
		   sizeof (struct sockaddr)) < 0)
      return false;
    
    return true;
  }

  void
  TcpClientSocket::close (void) {
    TcpSocket::close();
  }

} // end of namespace
