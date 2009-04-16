/* 
   Dispatcher.hpp - Dispatcher Object Header File

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
#ifndef HPP_PROACTOR_DISPATCHER
#define HPP_PROACTOR_DISPATCHER

#include "../concurrent/Thread.hpp"
#include "../concurrent/List.hpp"

namespace proactor {

  class Worker;

  class Dispatcher : public concurrent::Thread {
  private:
    typedef concurrent::List<Worker *> WorkerListType;

    WorkerListType workers;
  public:
    virtual ~Dispatcher (void);

    bool addWorker (Worker * w);
    bool removeWorker (Worker * w);
  };

} // end of namespace

#endif
