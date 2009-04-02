#ifndef HPP_NETWORK_TCP
#define HPP_NETWORK_TCP

#include "Socket.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>

class TcpSocket : public ISocket {
public:
  TcpSocket (void);
  virtual ~TcpSocket (void);

  void close (void);
  int send (const char * bytes, size_t length);
  int receive (char * bytes, size_t size);
};

class TcpServerSocket : public TcpSocket {
private:
  struct sockaddr_in sockaddr;
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
  struct sockaddr_in sockaddr;
public:
  TcpClientSocket (void);
  virtual ~TcpClientSocket (void);

  bool connect (const char * host, int port);
  void close (void);
};

#endif
