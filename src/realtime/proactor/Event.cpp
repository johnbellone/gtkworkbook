#include "Event.hpp"
using namespace proactor;

int Event::numEvents = 0;

Event::Event (int e, const std::string & buf) {
  this->id = e;
  this->buf = buf;
}

int
Event::uniqueEventId (void) {
  return Event::numEvents++;
}
