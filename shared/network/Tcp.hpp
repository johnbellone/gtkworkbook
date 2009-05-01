/* 
   Tcp.hpp - Tcp Implementation Header File

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
#ifndef HPP_NETWORK_TCP
#define HPP_NETWORK_TCP

#include "Socket.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

namespace network {

  class TcpSocket : public ISocket {
  protected:
    struct sockaddr_in sockaddr;
  public:
    TcpSocket (void);
    virtual ~TcpSocket (void);

    void close (void);
    int send (const char * bytes, size_t length);
    int receive (char * bytes, size_t size);
  };

  class TcpServerSocket : public TcpSocket {
  private:
    int port;
  public:
    class Acceptor {
    private:
      TcpServerSocket * socket;
      int sockfd;
    public:
      Acceptor (TcpServerSocket * server, int sockfd);

      int acceptIncoming (void);
    };

    TcpServerSocket (int port);
    virtual ~TcpServerSocket (void);
    
    bool start (int backlog);
    void close (void);
    Acceptor * newAcceptor (void);
  };
  
  class TcpClientSocket : public TcpSocket {
  private:
    struct hostent * hp;
  public:
    TcpClientSocket (void);
    TcpClientSocket (int newfd);
    virtual ~TcpClientSocket (void);

    bool connect (const char * host, int port);
    void close (void);
  };

} // end of namespace

#endif
