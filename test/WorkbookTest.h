#ifndef WORKBOOK_TEST_H
#define WORKBOOK_TEST_H

#include <gtest/gtest.h>
#include <libgtkworkbook/workbook.h>
#include <gtk/gtk.h>

class WorkbookTest : public ::testing::Test {
protected:
	GtkWidget * gtk_window;
	Workbook * workbook;
public:
	virtual void SetUp (void) {
		this->gtk_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		this->workbook = workbook_open (this->gtk_window, "");
	}

	virtual void TearDown (void) {
		this->workbook->destroy (this->workbook);
		gtk_widget_destroy (this->gtk_window);
	}
};

#endif
