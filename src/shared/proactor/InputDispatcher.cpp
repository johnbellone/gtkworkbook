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
#include "InputDispatcher.hpp"
#include "Proactor.hpp"

namespace proactor {

	InputDispatcher::~InputDispatcher (void) {
		this->inputQueue.lock();
		while (this->inputQueue.size() > 0)
			this->inputQueue.pop();
		this->inputQueue.unlock();
	}

	void *
	InputDispatcher::stop (void) {
		WorkerListType::iterator it = this->workers.begin();
		while (it != this->workers.end()) {
			(*it)->stop();
			it = this->workers.erase(it);
		}
		return Thread::stop();
	}

	void *
	InputDispatcher::run (void * null) {
		while (this->isRunning() == true) {
			while (this->inputQueue.size() == 0) {
				if (this->isRunning() == false)
					return NULL;
				Thread::sleep(1);
			}
	
			this->pro->onReadComplete ( this->inputQueue.pop() );
		}
  
		return NULL; 
	}

} // end of namespace
