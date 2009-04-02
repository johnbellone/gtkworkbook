#ifndef HPP_PROACTOR_PROACTOR
#define HPP_PROACTOR_PROACTOR

#include "../concurrent/Thread.hpp"
#include "../concurrent/Map.hpp"
#include "../concurrent/Queue.hpp"
#include "../concurrent/List.hpp"
#include "Job.hpp"
#include "Event.hpp"
#include "Dispatcher.hpp"
#include <list>
#include <queue>

namespace proactor {

  class Dispatcher;

  class Proactor : public concurrent::Thread {
  private:
    typedef std::list<Job *> EventHandlers;
    typedef concurrent::List<Dispatcher *> DispatcherList;
    typedef concurrent::Map<int, EventHandlers *> EventMapType;
    typedef concurrent::Queue<Event> EventQueueType;
    
    EventMapType eventsToHandlers;
    DispatcherList dispatchers;
    EventQueueType inputQueue;
  public:
    Proactor (void);
    virtual ~Proactor (void);

    void registerHandler (int e, Job * job);
    bool unregisterHandler (int e, Job * job);
    void addDispatcher (Dispatcher * d);
    bool removeDispatcher (Dispatcher * d);
    
    void * run (void * null);
 
    void onReadComplete (int e, const char * buf);

    inline const std::string & peekInputQueue (void) {
      return (this->inputQueue.front()).buf;
    }
  };

}

#endif
