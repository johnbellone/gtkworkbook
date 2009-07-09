#ifndef HPP_APPLICATION
#define HPP_APPLICATION

#include <string>
#include <list>
#include <cstdio>
#include <cstdlib>
#include <gtkworkbook/workbook.h>
#include "config.h"
#include "plugin.h"
#include "proactor/Proactor.hpp"

class Application {
public:
	enum sigs {
		DESTROY_EVENT = 0,
		DELETE_EVENT,
		NOTEBOOK_SWITCHED,
		NOTEBOOK_REMOVED,
		NOTEBOOK_REORDERED,
		SHEET_CHANGED,
		/**/
		MAX_SIGNALS
	};
private:
	typedef std::list<Workbook *> WorkbookList;
	typedef std::list<Plugin *> PluginList;

	GSourceFunc signals[MAX_SIGNALS];
	Workbook * active_workbook;
	GtkWidget * gtk_window;
	GtkWidget * gtk_window_vbox;
	GtkWidget * gtk_menu;
	Config * cfg;
	proactor::Proactor proactor;
	WorkbookList workbooks;
	PluginList plugins;
	gchar * absolute_path;
	
	void init (int argc, char ** argv);
public:
	Application (int argc, char ** argv);
	~Application (void);

	int run (void);
	void shutdown (void);
	Plugin * load_plugin (const gchar * filename);
	void open_extension (const gchar * filename, gboolean absolute_path);
};

#endif
