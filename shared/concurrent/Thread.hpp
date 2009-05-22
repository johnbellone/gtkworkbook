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
#ifndef HPP_CONCURRENT_THREAD
#define HPP_CONCURRENT_THREAD

#include <shared.h>
#include <string>
#include <pthread.h>
#include <map>
#include "Runnable.hpp"

namespace concurrent {

  class ThreadGroup;

  class Thread : public IRunnable {
  private:
    friend class ThreadGroup;

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
