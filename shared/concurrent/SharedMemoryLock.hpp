#ifndef HPP_CONCURRENT_SHAREDMEMORYLOCK
#define HPP_CONCURRENT_SHAREDMEMORYLOCK

#include "Map.hpp"
#include "Mutex.hpp"
#include <string>

namespace concurrent {

  class SharedMemoryLock : public ILockable {
  private:
    typedef Map<std::string,Mutex *> AddressToMutexMap;
    static AddressToMutexMap addressMutexMap;

    bool hasLock;
    std::string address;
    Mutex * mutex;
  public:
    SharedMemoryLock (const void * pointer, bool engage = false);
    virtual ~SharedMemoryLock (void);
 
    void lock (void);
    void unlock (void);
    bool trylock (void);

    static bool addMemoryLock (const void * pointer);
    static bool removeMemoryLock (const void * pointer);
  };
}

#endif 
