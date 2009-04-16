/* 
   Thread.cpp - Thread Object Source File

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
#include "Thread.hpp"
#include "ThreadGroup.hpp"
#include <iostream>
#include <ctime>

namespace concurrent {

  static void *
  thread_run (void * runner) {
    void * rvalue = ((IRunnable *)runner)->run (NULL);
    return rvalue;
  }

  Thread::Thread (ThreadGroup * group,
		  IRunnable * runner, 
		  const std::string & name) {
    this->group = group;
    this->runner = runner;
    this->name = name;
    this->running = false;
  }

  Thread::Thread (IRunnable * runner,
		  const std::string & name) 
    : group(NULL) {
    this->runner = runner;
    this->name = name;
    this->running = false;
  }

  Thread::Thread (const std::string & name) 
    : group(NULL), runner(NULL) {
    this->name = name;
    this->running = false;
  }

  Thread::Thread (void) : group(NULL), runner(NULL) {
    this->running = false;
  }

  Thread::~Thread (void) {
    if (this->running == true)
      this->interrupt();
  }

  void 
  Thread::interrupt (void) {
    pthread_cancel (this->thread);
    pthread_detach (this->thread);
  }

  void 
  Thread::yield (void) {
    pthread_yield();
  }

  void * 
  Thread::stop (void) {
    this->running = false;
    return this->join();
  }

  int 
  Thread::sleep (unsigned long ms) {
    struct timespec req = {0};
    time_t sec = (int)(ms/1000);
    ms = ms - (sec*1000);
    req.tv_sec = sec;
    req.tv_nsec = ms*1000000L;
    while (nanosleep (&req, &req) == -1)
      continue;
    return 1;
  }

  bool 
  Thread::start (void) {
    if (this->running == true)
      return false;

    if (this->runner != NULL) {
      if (pthread_create (&this->thread,
			  NULL,
			  thread_run,
			  this->runner))
	  return false;
    }
    else {
      if (pthread_create (&this->thread,
			  NULL,
			  thread_run,
			  this))
	return false;
    }

    this->running = true;
    return true;
  }

  void *
  Thread::join (void) {
    void * rvalue = NULL;
    pthread_join (this->thread, &rvalue);
    return rvalue;
  }

  void *
  Thread::run (void *null) {
    pthread_exit (null);
  }

} // end of namespace
