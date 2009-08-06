#ifndef HPP_OPENSTREAMDIALOG
#define HPP_OPENSTREAMDIALOG

#include <gtk/gtk.h>

namespace realtime {

	class Realtime;

	struct OpenStreamDialog {
		OpenStreamDialog (void) {
			rt = NULL;
			widget = NULL;
			host_entry = NULL;
			port_entry = NULL;
		}

		Realtime * rt;
		GtkWidget * widget;
		GtkWidget * host_entry;
		GtkWidget * port_entry;
	};
}

#endif
