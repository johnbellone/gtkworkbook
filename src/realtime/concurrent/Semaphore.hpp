#ifndef HPP_THREAD_SEMAPHORE
#define HPP_THREAD_SEMAPHORE

#include <semaphore.h>

namespace concurrent {

class Semaphore {
private:
  sem_t semaphore;
public:
  Semaphore (void);
  virtual ~Semaphore (void);

  void acquire (void);
  void release (void);
};

}

#endif
