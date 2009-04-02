/* 
   ThreadPool.cpp - ThreadPool Object Source File

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
#include "ThreadPool.hpp"
using namespace concurrent;

ThreadPool::ThreadPool (int nMaxThreads) {
  this->running = false;

  for (int ii = 0; ii < nMaxThreads; ii++)
    {
      std::string name = std::string("Thread ");
      name.push_back (ii);
      Thread * t = new Thread (new Task (this), name);
      this->threads.push_back (t);
    }
}

ThreadPool::~ThreadPool (void) {
  if (this->running == true)
    this->stop(true);

  std::list<Thread *>::iterator it = this->threads.begin();
  while (it != this->threads.end())
    {
      Thread * t = (*it++);
      delete t;
    }

  while (this->runQueue.size() > 0)
    {
      IRunnable * r = this->runQueue.pop();
      delete r;
    }
}

void 
ThreadPool::start (void) {
  this->running = true;

  std::list<Thread *>::iterator it = this->threads.begin();
  while (it != this->threads.end())
    {
      (*it)->start();
      it++;
    }
}

void
ThreadPool::stop (bool join = false) {
  this->running = false;

  /* Waiting to join all of the threads will mean that the thread that the
     pool is calling the stop() method from will block until all of the 
     threads inside of the pool have stopped.

     There is a PROBLEM here. If you call this, for some reason, from inside
     of one of the Task threads then you are going to hit a deadlock. Never
     pass the join parameter if this is being called from inside the Tasks. */
  if (join == true)
    {
      std::list<Thread *>::iterator it = this->threads.begin();
      while (it != this->threads.end())
	{
	  (*it)->join ();
	  it++;
	}
    }
}

void
ThreadPool::execute (IRunnable * runner) {
  this->runQueue.push (runner);
}
