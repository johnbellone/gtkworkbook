#include "Indexer.hpp"
#include <concurrent/SharedMemoryLock.hpp>

namespace largefile {

  Indexer::Indexer (proactor::InputDispatcher * d, FILE * fp) {
    this->dispatcher = d;
    this->fp = fp;
  }

  Indexer::~Indexer (void) {
  }

  void *
  Indexer::run (void * index) {
    this->running = true;

    while (this->running == true) {
      //      concurrent::SharedMemoryLock mutex (this->fp, true);
      
      
    }

    return index;
  }

} // end of namespace
