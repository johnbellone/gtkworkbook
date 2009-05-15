#include "ScopedMemoryLock.hpp"

namespace concurrent {
  ScopedMemoryLock::AddressToMutexMap ScopedMemoryLock::addressMutexMap;

  ScopedMemoryLock::ScopedMemoryLock (unsigned long address, bool engage) {
    this->hasLock = false;
	this->address = address;

    ScopedMemoryLock::addressMutexMap.lock();

    AddressToMutexMap::iterator it = addressMutexMap.find (this->address);

    ScopedMemoryLock::addressMutexMap.unlock();

    /* Now the reason behind setting this pointer to NULL is to force a coredump. Because
       if someone is passing a pointer that has not been formally added via the static method
       calls then that means it is very unlikely that they are not removing it either. We do
       not want a memory leak therefore this seems like the best method. */
    if (it == ScopedMemoryLock::addressMutexMap.end()) 
      this->mutex = NULL;
    else
      this->mutex = it->second;

    if (engage == true)
      this->lock();
  }

  ScopedMemoryLock::~ScopedMemoryLock (void) {
    if (this->hasLock == true)
      unlock();
  }

  void
  ScopedMemoryLock::lock (void) {
    this->mutex->lock();
    this->hasLock = true;
  }

  void 
  ScopedMemoryLock::unlock (void) {
    this->mutex->unlock();
    this->hasLock = false;
  }

  bool
  ScopedMemoryLock::trylock (void) {
    this->hasLock = this->mutex->trylock();
    return this->hasLock;
  }

  bool
  ScopedMemoryLock::remove (void) {
	if (this->hasLock == false) {
 	  return false;
	}

	addressMutexMap.lock();

	AddressToMutexMap::iterator it = addressMutexMap.find (this->address);
	if (it == addressMutexMap.end()) {
	  addressMutexMap.unlock();
	  return false;
	}

	delete it->second;
	addressMutexMap.end();

	addressMutexMap.unlock();
	return true;
  }

  bool 
  ScopedMemoryLock::addMemoryLock (unsigned long address) {
    addressMutexMap.lock();

    AddressToMutexMap::iterator it = addressMutexMap.find (address);

    // Already exists inside of the map; we're going to return an error here.
    if (it == addressMutexMap.end()) {
      addressMutexMap.unlock();
      return false;
    }
    
    addressMutexMap.insert (std::make_pair (address, new Mutex));

    addressMutexMap.unlock();
    return true;
  }

  bool 
  ScopedMemoryLock::removeMemoryLock (unsigned long address) {
    addressMutexMap.lock();

	AddressToMutexMap::iterator it = addressMutexMap.find (address);
    if (it == addressMutexMap.end()) {
      addressMutexMap.unlock();
      return false;
    }

    delete it->second;
    addressMutexMap.erase(it);

    addressMutexMap.unlock();
    return true;
  }

} // end of namespace
