#include "Semaphore.hpp"
using namespace concurrent;

Semaphore::Semaphore (void) {
  sem_init (&this->semaphore, 0, 0);
}

Semaphore::~Semaphore (void) {
  sem_destroy (&this->semaphore);
}

void
Semaphore::acquire (void) {
  sem_wait (&this->semaphore);
}

void
Semaphore::release (void) {
  sem_post (&this->semaphore);
}
