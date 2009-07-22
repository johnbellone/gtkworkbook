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
#include "Plugin.hpp"

Plugin::Plugin (Application * appstate, Handle * platform)
	: appstate(appstate), platform(platform) {
	this->wb = NULL;
}

Plugin::~Plugin (void) {
	// If this comment is still here then this potentially really, really, bad idea turned
	// out to actually work. There is probably a much better and safer way to do this.
	LIBRARY_CLOSE (platform->handle);
	delete platform;
}

Plugin *
Plugin::open_plugin (Application * appstate, const std::string filename) {
	typedef Plugin * (*Plugin_Main) (Application *, Handle *);
      				
	if (filename.length() == 0)
		return NULL;

	// Get a handle to the shared library we're attempting to open.
	Handle * platform = new Handle;
	if ((platform->handle = LIBRARY_OPEN (filename.c_str())) == NULL) {
		fprintf (stderr, "%s\n", LIBRARY_ERROR());
		exit(1);
	}

	// Grab the main method from the plugin so we can execute to get an object back.
	Plugin_Main plugin_main;
	if ((plugin_main = (Plugin_Main)LIBRARY_SYM(platform->handle, "PluginFactoryCreate")) == NULL) {
		fprintf (stderr, "%s\n", LIBRARY_ERROR());
		exit(1);
	}

	// Execute the method, return an object, and basically go on our merry way.
	return plugin_main (appstate, platform);
}

Workbook *
Plugin::workbook(void) {
	return this->wb;
}

Application *
Plugin::app(void) {
	return this->appstate;
}
