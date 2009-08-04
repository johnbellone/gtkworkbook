#ifndef HPP_OPENSTREAMDIALOG
#define HPP_OPENSTREAMDIALOG

#include <gtk/gtk.h>

namespace realtime {

	class Realtime;

	struct OpenStreamDialog {
		OpenStreamDialog (void) {
			rt = NULL;
			widget = NULL;
			address_entry = NULL;
		}

		Realtime * rt;
		GtkWidget * widget;
		GtkWidget * address_entry;
	};
}

#endif
