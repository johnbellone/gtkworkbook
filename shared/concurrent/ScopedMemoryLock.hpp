#ifndef HPP_CONCURRENT_SCOPEDMEMORYLOCK
#define HPP_CONCURRENT_SCOPEDMEMORYLOCK

#include "Map.hpp"
#include "Mutex.hpp"
#include <string>

namespace concurrent {

  class ScopedMemoryLock : public ILockable {
  private:
    typedef Map<long int,Mutex *> AddressToMutexMap;
    static AddressToMutexMap addressMutexMap;

    bool hasLock;
	unsigned long address;
    Mutex * mutex;
  public:
    ScopedMemoryLock (unsigned long address, bool engage = false);
    virtual ~ScopedMemoryLock (void);
 
    void lock (void);
    void unlock (void);
    bool trylock (void);
	bool remove (void);

    static bool addMemoryLock (unsigned long address);
    static bool removeMemoryLock (unsigned long address);
  };
} // end of namespace

#endif 
