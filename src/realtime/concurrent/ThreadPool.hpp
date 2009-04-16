/* 
   ThreadPool.hpp - ThreadPool Object Header File

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
#ifndef HPP_THREAD_THREADPOOL
#define HPP_THREAD_THREADPOOL

#include "Mutex.hpp"
#include "Thread.hpp"
#include "Queue.hpp"
#include <queue>
#include <list>

namespace concurrent {

  /* @description: This is a ThreadPool object that will keep N concurrent
     threads running. Runnable objects can be attached to this object and they
     will be executed immediately as a thread becomes available to run them. 
     @author: John `jb Bellone (jvb4@njit.edu)
     @basis: http://ibm.com/developerworks/java/library/j-jtp0730.html */
  class ThreadPool {
    /* @description: This object is an individual task that will be spawned at
       the start of the ThreadPool. Each thread periodically checks the pool
       object for any available Runnable objects from the queue. If there are
       such objects it will take them off, run them and delete them. */
    class Task : public IRunnable {
    private:
      ThreadPool * pool;
    public:
      Task (ThreadPool * pool) {
	this->pool = pool;
      }
      
      void * run (void *) {
	this->running = true;

	while (this->running == true)
	  {
	    if (pool->isRunning() == false)
	      break;

	    if (pool->getQueueSize() > 0)
	      {
		IRunnable * runner = pool->removeFromQueue();
		runner->run(NULL);
		delete runner;
	      }
	    ::sleep (1);
	  }
	return NULL;
      }
    };
  private:
    concurrent::Queue<IRunnable *> runQueue;
    std::list<Thread *> threads;
    bool running;
  public:  
    ThreadPool (int);
    ~ThreadPool (void);

    /* Thread un-Safe Methods */
    void start (void);
    void stop (bool);

    /* Thread-Safe Methods */
    void execute (IRunnable *);
    inline IRunnable * removeFromQueue (void) { return this->runQueue.pop(); }
    inline bool isRunning (void) const { return this->running; }
    inline int getMaxThreads (void) const { return this->threads.size(); }
    inline size_t getQueueSize (void) { return this->runQueue.size(); }
  };

} // end of namespace

#endif
