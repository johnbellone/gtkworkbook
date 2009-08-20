/* 
   The GTKWorkbook Project <http://gtkworkbook.sourceforge.net/>
   Copyright (C) 2009 John Bellone, Jr. <jvb4@njit.edu>

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
#include "ScopedMemoryLock.hpp"

namespace concurrent {
	ScopedMemoryLock::AddressToMutexMap ScopedMemoryLock::addressMutexMap;

	ScopedMemoryLock::ScopedMemoryLock (unsigned long address, bool engage) {
		this->hasLock = false;
		this->address = address;
		this->mutex = NULL;

		ScopedMemoryLock::addressMutexMap.lock();
		AddressToMutexMap::iterator it = addressMutexMap.find (this->address);
		ScopedMemoryLock::addressMutexMap.unlock();

		/* Now the reason behind keeping this pointer to NULL is to force a coredump. Because
			if someone is passing an address that has not been formally added via the static method
			call then that means it is very unlikely that they are not removing it either. We do
			not want a memory leak therefore this seems like the best method. */
		if (it != ScopedMemoryLock::addressMutexMap.end()) 
			this->mutex = it->second;

		if (engage == true)
			this->lock();
	}

	ScopedMemoryLock::~ScopedMemoryLock (void) {
		if (this->hasLock == true)
			unlock();
	}

	void
	ScopedMemoryLock::lock (void) {
		this->mutex->lock();
		this->hasLock = true;
	}

	void 
	ScopedMemoryLock::unlock (void) {
		this->mutex->unlock();
		this->hasLock = false;
	}

	bool
	ScopedMemoryLock::isLocked (void) const {
		return this->hasLock;
	}
	
	bool
	ScopedMemoryLock::trylock (void) {
		this->hasLock = (this->hasLock==true) ? true : this->mutex->trylock();
		return this->hasLock;
	}

	bool
	ScopedMemoryLock::remove (void) {
		if (this->hasLock == false) {
			return false;
		}

		addressMutexMap.lock();

		bool result = false;
		AddressToMutexMap::iterator it = addressMutexMap.find (this->address);
		if (it != addressMutexMap.end()) {
			delete it->second;
			addressMutexMap.end();
			result = true;
		}
    
		addressMutexMap.unlock();
		return result;
	}

	bool 
	ScopedMemoryLock::addMemoryLock (unsigned long address) {
		addressMutexMap.lock();

		bool result = false;
		AddressToMutexMap::iterator it = addressMutexMap.find (address);
		if (it == addressMutexMap.end()) {
			addressMutexMap.insert (std::make_pair (address, new Mutex));
			result = true;
		}

		addressMutexMap.unlock();
		return result;
	}

	bool 
	ScopedMemoryLock::removeMemoryLock (unsigned long address) {
		addressMutexMap.lock();

		bool result = false;
		AddressToMutexMap::iterator it = addressMutexMap.find (address);
		if (it != addressMutexMap.end()) {
			delete it->second;
			addressMutexMap.erase(it);
			result = true;
		}

		addressMutexMap.unlock();
		return result;
	}

} // end of namespace
