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
	if ((plugin_main = (Plugin_Main)LIBRARY_SYM(platform->handle, "plugin_main")) == NULL) {
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
