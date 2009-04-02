#ifndef HPP_CONNECTION
#define HPP_CONNECTION

#include "network/Tcp.hpp"
#include "proactor/Dispatcher.hpp"
#include "proactor/Proactor.hpp"
#include <iostream>

#define MAX_INPUT_SIZE 4096

class Connection : public TcpSocket {
public:
  Connection (int sockfd); 
  virtual ~Connection (void);
};

class ConnectionThread : public proactor::Dispatcher {
private:
  Connection * socket;
public:
  ConnectionThread (proactor::Proactor * pro, int newfd); 
  virtual ~ConnectionThread (void);

  void * run (void * null);
};

#endif
