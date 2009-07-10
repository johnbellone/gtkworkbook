/* 
   The GTKWorkbook Project <http://gtkworkbook.sourceforge.net/>
   Copyright (C) 2008, 2009 John Bellone, Jr. <jvb4@njit.edu>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PRACTICAL PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301 USA
*/
#include "Mutex.hpp"

namespace concurrent {

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

} // end of namespace
