#include "Tcp.hpp"
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

/*****************************************************************************/

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

/*****************************************************************************/

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
  // This is to prevent conflicts with major services' ports.
  if (this->port < 50)
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

/*****************************************************************************/

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

/*****************************************************************************/

TcpClientSocket::TcpClientSocket (void) : TcpSocket() {
  memset (&(this->sockaddr), 0, sizeof (this->sockaddr));
  this->sockaddr.sin_family = AF_INET;
  this->sockaddr.sin_addr.s_addr = htonl (INADDR_ANY);
}
				
TcpClientSocket::~TcpClientSocket (void) {
}

bool
TcpClientSocket::connect (const char * host, int port) {
  struct sockaddr_in servAddr;
  struct hostent * h;

  if (!host || (*host == '\0'))
    return false;

  if (this->sockfd < 0)
    return false;

  if ((h = ::gethostbyname (host)) == NULL)
    return false;

  servAddr.sin_family = h->h_addrtype;
  memcpy ((char *)&(servAddr.sin_addr.s_addr),
	  h->h_addr_list[0],
	  h->h_length);
  servAddr.sin_port = htons (port);
  
  if (::bind (this->sockfd,
	      (struct sockaddr *)&(this->sockaddr),
	      sizeof (this->sockaddr)) < 0)
    return false;

  if (::connect (this->sockfd,
		 (struct sockaddr *)&(this->sockaddr),
		 sizeof (this->sockaddr)) < 0)
    return false;
  return true;  
}

void
TcpClientSocket::close (void) {
  TcpSocket::close();
}
