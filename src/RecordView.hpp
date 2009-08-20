#ifndef HPP_RECORDVIEW
#define HPP_RECORDVIEW

#include <gtk/gtk.h>
#include <gtkworkbook/workbook.h>
#include "Application.hpp"

class RecordView {
private:
	GtkWidget * gtk_window;
	Application * appstate;
public:
	RecordView (Application * app);
	~RecordView (void);

	Application * app (void);
	GtkWidget * window (void);
};

#endif
