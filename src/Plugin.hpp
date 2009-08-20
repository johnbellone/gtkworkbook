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
#ifndef HPP_PLUGIN
#define HPP_PLUGIN

#include "Application.hpp"
#include <dlfcn.h>
#include <libgtkworkbook/workbook.h>
#include <concurrent/Mutex.hpp>
#include <string>

#ifdef WIN32
#include <windows.h>
typedef HINSTANCE PlatformHandle;
#define LIBRARY_CLOSE(handle) FreeLibrary(handle)
#define LIBRARY_OPEN(filename) LoadLibrary(filename)
#define LIBRARY_SYM(handle, symbol) GetProcAddress(handle, symbol)
#define LIBRARY_ERROR() GetLastError()
#else
typedef void * PlatformHandle;
#define LIBRARY_CLOSE(handle) dlclose(handle)
#define LIBRARY_OPEN(filename) dlopen(filename, RTLD_LAZY)
#define LIBRARY_SYM(handle, symbol) dlsym(handle, symbol)
#define LIBRARY_ERROR() dlerror()
#endif

struct Handle {
	Handle() :handle(NULL) { }
	PlatformHandle handle;
};

class Plugin : public concurrent::RecursiveMutex {
protected:
	Workbook * wb;
	Application * appstate;
private:
	Handle * platform;
public:
	static Plugin * open_plugin (Application * appstate, const std::string filename);
	
	Plugin (Application * appstate, Handle * platform);
	virtual ~Plugin (void);

	Workbook * workbook (void);
	Application * app (void);
};

#endif
