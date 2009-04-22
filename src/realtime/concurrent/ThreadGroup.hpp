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
#ifndef HPP_CONCURRENT_THREADGROUP
#define HPP_CONCURRENT_THREADGROUP

#include <string>
#include "List.hpp"
#include "Thread.hpp"

namespace concurrent {

  class ThreadGroup {
  private:
    typedef concurrent::List<Thread *> ThreadList;
    
    ThreadGroup * parent;
    std::string name;
    ThreadList threads;
    bool daemon; 
  public:
    ThreadGroup (void);
    ThreadGroup (ThreadGroup * parent, const std::string & name);
    ThreadGroup (const std::string & name);
    virtual ~ThreadGroup (void);

    bool parentOf (ThreadGroup * parent);
    bool addThread (Thread * thread);
    bool removeThread (Thread * thread);
    void interrupt (void);
    bool start (void);
    void stop (void);
    
    inline bool isDaemon (void) const { return this->daemon; }
    inline void setDaemon (bool daemon) { this->daemon = daemon; }
    inline const std::string & getName (void) const { return this->name; }
    inline ThreadGroup * getParent (void) const { return this->parent; }
  };

} // end of namespace

#endif
