/* 
   ThreadGroup.cpp - ThreadGroup Object Source File

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
#include "ThreadGroup.hpp"
#include <algorithm>

namespace concurrent {

  ThreadGroup::ThreadGroup (void) 
    : parent(NULL), daemon(false) {
  }

  ThreadGroup::ThreadGroup (ThreadGroup * parent, const std::string & name) 
    : daemon(false) {
    this->parent = parent;
    this->name = name;
  }

  ThreadGroup::ThreadGroup (const std::string & name) 
    : parent(NULL), daemon(false) {
    this->name = name;
  }

  ThreadGroup::~ThreadGroup (void) {
    if (this->daemon == true) {
      this->stop();
    }
  }

  bool 
  ThreadGroup::parentOf (ThreadGroup * parent) {
    if ((this->parent == NULL) || (parent == NULL))
      return false;
    // Needs to be better than this. 
    return (this->parent == parent) ? true : false;
  } 
    
  bool
  ThreadGroup::addThread (Thread * thread) {
    ThreadList::iterator it = std::find (this->threads.begin(),
					 this->threads.end(),
					 thread);
    if (it == this->threads.end()) {
      thread->group = this;
      this->threads.push_back (thread);
      return true;
    }
    return false;
  }

  bool 
  ThreadGroup::removeThread (Thread * thread) {
    ThreadList::iterator it = std::find (this->threads.begin(),
					 this->threads.end(),
					 thread);

    if (it == this->threads.end())
      return false;
    this->threads.erase(it);
    return true;
  }

  void
  ThreadGroup::interrupt (void) {
    ThreadList::iterator it = this->threads.begin();
    while (it != this->threads.end()) {
      (*it++)->interrupt();
    }
  }

  bool
  ThreadGroup::start (void) {
    bool rvalue = false;

    ThreadList::iterator it = this->threads.begin();
    while (it != this->threads.end()) {
      rvalue = (*it++)->start();
    }

    return rvalue;
  }

  void 
  ThreadGroup::stop (void) {
    ThreadList::iterator it = this->threads.begin();
    while (it != this->threads.end()) {
      (*it++)->stop();
    }
  }
} // end of namesapce
