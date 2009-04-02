#ifndef HPP_THREAD_MAP
#define HPP_THREAD_MAP

#include <iterator>
#include <map>
#include "Mutex.hpp"

namespace concurrent {
 
  /* @description: This object uses the standard std::map and wraps all of
     the calls that are exposed with a Mutex object. This object uses a
     single lock and may cause performance issues if several threads are
     attempting to use it. 
     @author: John `jb Bellone <jvb4@njit.edu> */
  template<typename K, typename V>
  class Map : public RecursiveMutex, public std::map<K,V> {
  private:
  public:
    Map (void) : RecursiveMutex() { }
  };
}
#endif
