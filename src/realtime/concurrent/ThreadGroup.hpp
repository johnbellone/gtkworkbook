#ifndef HPP_THREAD_THREADGROUP
#define HPP_THREAD_THREADGROUP

#include <string>
#include <list>
#include "Thread.hpp"

namespace concurrent {

class ThreadGroup {
  friend class Thread;
  typedef std::list<Thread *> ThreadList;
private:
  int maxPriority;
  ThreadGroup * parent;
  std::string name;
  ThreadList threads;
  bool daemon; 

  void addThreadToGroup (Thread *);
  void removeThreadFromGroup (Thread *);
public:
  ThreadGroup (const std::string &);
  ThreadGroup (ThreadGroup *, const std::string &);
  ~ThreadGroup (void);

  bool parentOf (ThreadGroup *);
  bool checkAccess (void);

  inline bool isDaemon (void) const { return this->daemon; }
  
  inline const ThreadGroup * getParent (void) const { return this->parent; }
  inline const std::string & getName (void) const { return this->name; }
  inline int getMaxPriority (void) const { return this->maxPriority; }

  inline void setMaxPriority (int priority) { this->maxPriority = priority; }
  inline void setDaemon (bool daemon) { this->daemon = daemon; }
};

}

#endif
