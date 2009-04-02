/* 
   Queue.hpp - Mutex Queue Object Header File

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
