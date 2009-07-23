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
#ifndef HPP_CONCURRENT_SCOPEDMEMORYLOCK
#define HPP_CONCURRENT_SCOPEDMEMORYLOCK

#include "Map.hpp"
#include "Mutex.hpp"
#include <string>

namespace concurrent {

	class ScopedMemoryLock : public ILockable {
	private:
		typedef Map<long int,Mutex *> AddressToMutexMap;
		static AddressToMutexMap addressMutexMap;

		bool hasLock;
		unsigned long address;
		Mutex * mutex;
	public:
		ScopedMemoryLock (unsigned long address, bool engage = false);
		virtual ~ScopedMemoryLock (void);
 
		void lock (void);
		void unlock (void);
		bool trylock (void);
		bool remove (void);
		bool isLocked (void) const;
	  
		static bool addMemoryLock (unsigned long address);
		static bool removeMemoryLock (unsigned long address);
	};
} // end of namespace

#endif 
