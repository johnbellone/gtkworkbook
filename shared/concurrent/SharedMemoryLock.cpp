#include "SharedMemoryLock.hpp"
#include <sstream>

namespace concurrent {
  SharedMemoryLock::AddressToMutexMap SharedMemoryLock::addressMutexMap;

  SharedMemoryLock::SharedMemoryLock (const void * pointer, bool engage) {
    std::stringstream ss; ss<<pointer;
    this->address = ss.str();
    this->hasLock = false;

    SharedMemoryLock::addressMutexMap.lock();

    AddressToMutexMap::iterator it = SharedMemoryLock::addressMutexMap.find (this->address);

    SharedMemoryLock::addressMutexMap.unlock();

    /* Now the reason behind setting this pointer to NULL is to force a coredump. Because
       if someone is passing a pointer that has not been formally added via the static method
       calls then that means it is very unlikely that they are not removing it either. We do
       not want a memory leak therefore this seems like the best method. */
    if (it == SharedMemoryLock::addressMutexMap.end()) 
      this->mutex = NULL;
    else
      this->mutex = (*it).second;

    if (engage == true)
      this->lock();
  }

  SharedMemoryLock::~SharedMemoryLock (void) {
    if (this->hasLock == true)
      unlock();
  }

  void
  SharedMemoryLock::lock (void) {
    this->mutex->lock();
    this->hasLock = true;
  }

  void 
  SharedMemoryLock::unlock (void) {
    this->mutex->unlock();
    this->hasLock = false;
  }

  bool
  SharedMemoryLock::trylock (void) {
    this->hasLock = this->mutex->trylock();
    return this->hasLock;
  }

  bool 
  SharedMemoryLock::addMemoryLock (const void * pointer) {
    addressMutexMap.lock();

    std::stringstream ss; ss << pointer;
    std::string the_address = ss.str();
    AddressToMutexMap::iterator it = addressMutexMap.find (the_address);

    // Already exists inside of the map; we're going to return an error here.
    if (it == addressMutexMap.end()) {
      addressMutexMap.unlock();
      return false;
    }
    
    addressMutexMap.insert (std::make_pair (the_address, new Mutex));

    addressMutexMap.unlock();
    return true;
  }

  bool 
  SharedMemoryLock::removeMemoryLock (const void * pointer) {
    addressMutexMap.lock();

    std::stringstream ss; ss << pointer;
    std::string the_address = ss.str();
    AddressToMutexMap::iterator it = addressMutexMap.find (the_address);
    if (it == addressMutexMap.end()) {
      addressMutexMap.unlock();
      return false;
    }

    delete (*it).second;
    addressMutexMap.erase(it);

    addressMutexMap.unlock();
    return true;
  }

} // end of namespace
