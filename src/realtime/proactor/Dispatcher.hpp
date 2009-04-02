#ifndef HPP_PROACTOR_DISPATCHER
#define HPP_PROACTOR_DISPATCHER

#include "../concurrent/Thread.hpp"

namespace proactor {

  class Proactor;

  class Dispatcher : public concurrent::Thread {
  private:
    int eid;
  protected:
    Proactor * pro;
  public:
    virtual ~Dispatcher (void) { }

    inline void setEventId (int e) {
      this->eid = e;
    }

    inline int getEventId (void) {
      return this->eid;
    }
  };

}

#endif
