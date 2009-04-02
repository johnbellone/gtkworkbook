#ifndef HPP_NETWORK_SOCKET
#define HPP_NETWORK_SOCKET

#include <cstring>

class ISocket {
protected:
  int sockfd;
public:
  virtual ~ISocket (void) { }

  virtual void close (void) = 0;
  virtual int send (const char * bytes, size_t length) = 0;
  virtual int receive (char * btyes, size_t size) = 0;
};

#endif
