/* 
   ThreadGroup.hpp - ThreadGroup Object Header File

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
