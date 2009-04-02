#include "Mutex.hpp"
using namespace concurrent;

IMutex::IMutex (void) {
  this->mutex = NULL;
  this->attrib = NULL;
}

IMutex::IMutex (const IMutex & m) {
  this->mutex = m.mutex;
  this->attrib = m.attrib;
}

IMutex::~IMutex (void) {
}

IMutex & 
IMutex::operator= (const IMutex & m) {
  this->mutex = m.mutex;
  this->attrib = m.attrib;
  return *this;
}

void
IMutex::lock (void) {
  pthread_mutex_lock (this->mutex);
}

void 
IMutex::unlock (void) {
  pthread_mutex_unlock (this->mutex);
}

bool
IMutex::trylock (void) {
  return (pthread_mutex_trylock (this->mutex) == 0) ?
    true : false;
}
		      
/****************************************************************************/

RecursiveMutex::RecursiveMutex (void) {
  this->mutex = new pthread_mutex_t;
  this->attrib = new pthread_mutexattr_t;

  pthread_mutexattr_init (this->attrib);
  pthread_mutexattr_settype (this->attrib, PTHREAD_MUTEX_RECURSIVE);
  
  pthread_mutex_init (this->mutex, attrib);
}

RecursiveMutex::~RecursiveMutex (void) {
  pthread_mutex_destroy (this->mutex);
  pthread_mutexattr_destroy (this->attrib);

  delete this->mutex;
  delete this->attrib;
}

Mutex::Mutex (void) {
  this->mutex = new pthread_mutex_t;

  pthread_mutex_init (this->mutex, NULL);
}

Mutex::~Mutex (void) {
  pthread_mutex_destroy (this->mutex);

  delete this->mutex;
}
