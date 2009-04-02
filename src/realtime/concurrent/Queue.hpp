#ifndef HPP_THREAD_QUEUE
#define HPP_THREAD_QUEUE

#include "Mutex.hpp"
#include <queue>

namespace concurrent {
  
  /* @description: This object wraps the existing std::queue container with
     the custom Mutex object from the concurrent namespace. The single lock
     may be placed in the future with a read/write lock in order to gain a
     speed advantage.
     @author: John `jb Bellone <jvb4@njit.edu> */
  template <typename T>
  class Queue : public RecursiveMutex {
  private:
    std::queue<T> the_queue;
  public:
    Queue (void) { }
    virtual ~Queue (void) { }

    inline const T & back (void) {
      this->lock();
      T & elem = this->the_queue.back();
      this->unlock();
      return elem;
    }

    inline const T & front (void) {
      this->lock();
      T & elem = this->the_queue.front();
      this->unlock();
      return elem;
    }

    inline void push (const T & x) {
      this->lock();
      this->the_queue.push (x);
      this->unlock();
    }

    inline T pop (void) {
      this->lock();
      T elem = this->the_queue.front();
      this->the_queue.pop();
      this->unlock();
      return elem;
    }
    
    inline size_t size (void) {
      this->lock();
      size_t size = this->the_queue.size();
      this->unlock();
      return size;
    }

    inline bool empty (void) {
      this->lock();
      bool result = this->the_queue.empty();
      this->unlock();
      return result;
    }
  };
}
#endif
