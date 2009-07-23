#ifndef HPP_GOTODIALOG
#define HPP_GOTODIALOG

#include <gtk/gtk.h>

namespace largefile {

	class Largefile;
	
	struct RadioButton {
		GtkWidget * widget;
		gint index;
	};
	
	struct GotoDialog {
		GotoDialog(void) {
			lf = NULL;
			widget = NULL;
			radio_byte.widget = NULL;
			radio_byte.index = 0;
			radio_line.widget = NULL;
			radio_line.index = 1;
			radio_perc.widget = NULL;
			radio_perc.index = 2;
			active_index = 0;
		}
		
		Largefile * lf;
		GtkWidget * widget;
		RadioButton radio_byte;
		RadioButton radio_line;
		RadioButton radio_perc;
		gint active_index;
	};
}

#endif
