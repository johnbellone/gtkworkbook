#ifndef HPP_CONCURRENT_LIST
#define HPP_CONCURRENT_LIST

#include <list>
#include "Mutex.hpp"

namespace concurrent {

  template <typename T>
  class List : public RecursiveMutex, public std::list<T> {
  };
}

#endif
