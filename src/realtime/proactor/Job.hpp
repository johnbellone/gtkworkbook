#ifndef HPP_PROACTOR_JOB
#define HPP_PROACTOR_JOB

#include "../concurrent/Queue.hpp"
#include "../concurrent/Thread.hpp"
#include <string>

namespace proactor {

  class Job : public concurrent::Thread {
  protected:
    concurrent::Queue<std::string> inputQueue;
  public:
    inline void pushInputQueue (std::string buf) {
      this->inputQueue.push (buf);
    }
  };

}

#endif
