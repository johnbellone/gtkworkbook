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
#include "FileDispatcher.hpp"
#include "Plaintext.hpp"
#include "Gzip.hpp"
#include <proactor/Proactor.hpp>
#include <cstdio>
#include <iostream>

using namespace largefile;

AbstractFileDispatcher *
AbstractFileDispatcher::CreateFromExtension (const std::string & filename, int e) {
	std::string ext = filename.substr (filename.find_last_of ('.'), filename.length());

	// Return the proper worker depending on the file's extension. This is so that we can
	// support automatically opening .gz, .lz and .bz2 extensions automatically in the future.
	if (0 == ext.compare (".gz"))
		return new GnuzipDispatcher (e);
	return new PlaintextDispatcher (e);
}

AbstractFileDispatcher::AbstractFileDispatcher (int e) {
	setEventId (e);

	this->marks = new FileIndex;
}

AbstractFileDispatcher::AbstractFileDispatcher (int e, FileIndex * marks) {
	setEventId (e);
	
	this->marks = marks;
}

AbstractFileDispatcher::~AbstractFileDispatcher (void) {
	this->marks->lock();
	delete this->marks;
	this->marks = NULL;
}
