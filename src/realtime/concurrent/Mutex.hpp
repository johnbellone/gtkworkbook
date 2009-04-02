#ifndef HPP_THREAD_MUTEX
#define HPP_THREAD_MUTEX

#include <pthread.h>
#include "Lockable.hpp"

namespace concurrent {

  class IMutex : public ILockable {
  protected:
    pthread_mutex_t * mutex;
    pthread_mutexattr_t * attrib;
  public:
    IMutex (void);
    IMutex (const IMutex & m);
    virtual ~IMutex (void);

    void lock (void);
    void unlock (void);
    bool trylock (void);

    IMutex & operator= (const IMutex & m);
  };

  class Mutex : public IMutex {
  public:
    Mutex (void);
    virtual ~Mutex (void);
  };

  class RecursiveMutex : public IMutex {
  public:
    RecursiveMutex (void);
    virtual ~RecursiveMutex (void);
  };
}

#endif
