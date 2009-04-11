/* 
   Dispatcher.hpp - Dispatcher Object Source File

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
#include "Dispatcher.hpp"
using namespace proactor;

Dispatcher::~Dispatcher (void) {
  while (this->inputQueue.size() > 0)
    this->inputQueue.pop();
}

void
Dispatcher::addWorker (Worker * w) {
  this->workers.push_back (w);
}

bool
Dispatcher::removeWorker (Worker * w) {
  WorkerListType::iterator it = std::find (this->workers.begin(),
					   this->workers.end(),
					   w);

  if (it == this->workers.end())
    return false;
  
  this->workers.erase (it);
  return true;
}
