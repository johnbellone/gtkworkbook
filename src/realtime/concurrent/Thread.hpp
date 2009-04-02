#ifndef HPP_THREAD_THREAD
#define HPP_THREAD_THREAD

#include <shared.h>
#include <string>
#include <pthread.h>
#include <map>
#include "Runnable.hpp"

namespace concurrent {

class ThreadGroup;

class Thread : public IRunnable {
private:
  ThreadGroup * group;
  std::string name;
  IRunnable * runner;
  pthread_t thread;
  bool joinable;
  int priority;
public:
  Thread (ThreadGroup * group, 
	  IRunnable * runner, 
	  const std::string & name);
  Thread (IRunnable * runner,
	  const std::string & name);
  Thread (const std::string & name);
  Thread (void);
  virtual ~Thread (void);

  bool start (void);
  void * stop (void);
  void * join (void);
  void interrupt (void);
  void yield (void);
  
  virtual void * run (void *);

  inline const std::string & getName (void) const { return this->name; }
  inline int getPriority (void) const { return this->priority; }
  inline void setPriority (int priority) { this->priority = priority; }
  inline const ThreadGroup * getThreadGroup (void) const { 
    return this->group; 
  }

  static int sleep (unsigned long ms);
};  

}

#endif
