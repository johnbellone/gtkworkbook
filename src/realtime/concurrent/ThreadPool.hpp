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

}

#endif
