#ifndef HPP_PROACTOR_EVENT
#define HPP_PROACTOR_EVENT

#include <string>

namespace proactor {

  class Event {
  private:
    static int numEvents;
  public:
    int id;
    std::string buf;

    Event (int id, const std::string & buf);

    static int uniqueEventId (void);
  };

}

#endif
