#include "RecordView.hpp"

static void
RecordViewCallback (GtkWidget * w, gpointer data) {

}

RecordView::RecordView (Application * appstate) {
	this->appstate = appstate;

	this->gtk_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (this->gtk_window), "Record View");
	/*	gtk_window_set_model (GTK_WINDOW (this->gtk_window), FALSE);
		gtk_window_set_resizeable (GTK_WINDOW (this->gtk_window), TRUE); */
	gtk_window_set_destroy_with_parent (GTK_WINDOW (this->gtk_window), TRUE);
}

RecordView::~RecordView (void) {

}

GtkWidget *
RecordView::window (void) {
	return this->gtk_window;
}

Application *
RecordView::app (void) {
	return this->appstate;
}
