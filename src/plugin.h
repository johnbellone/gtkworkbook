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
#ifndef H_PLUGIN
#define H_PLUGIN

typedef struct _PluginMethod PluginMethod;
typedef struct _Plugin Plugin;

#ifdef WIN32
#include <windows.h>
typedef HINSTANCE LibraryHandle;
#define LIBRARY_CLOSE(handle) FreeLibrary(handle)
#define LIBRARY_OPEN(filename) LoadLibrary(filename)
#define LIBRARY_SYM(handle, symbol) GetProcAddress(handle, symbol)
#define LIBRARY_ERROR() GetLastError()
#else
typedef void * LibraryHandle;
#define LIBRARY_CLOSE(handle) dlclose(handle)
#define LIBRARY_OPEN(filename) dlopen(filename, RTLD_LAZY)
#define LIBRARY_SYM(handle, symbol) dlsym(handle, symbol)
#define LIBRARY_ERROR() dlerror()
#endif

#include <shared.h>
#include <dlfcn.h>
#include <glib/gthread.h>

struct _Plugin
{
  /* Members */
  LibraryHandle pfnhandle;
  PluginMethod * first, * last;
  Plugin * next, * prev;
  GPtrArray * threads;

  /* Methods */
  void *(*method_register) (Plugin *, gchar *);
  void (*method_deregister) (Plugin *, PluginMethod *);
  void (*destroy) (Plugin *);
  GThread *(*create_thread) (Plugin *, GThreadFunc, gpointer);
};

struct _PluginMethod
{
  /* Members */
  void * pfnmethod;
  PluginMethod * next, * prev;
  Plugin * plugin;

  /* Methods */
  void (*deregister) (PluginMethod *method, Plugin *plugin);
};

/* plugin.c */
Plugin * plugin_open (const gchar * filename);

#endif
