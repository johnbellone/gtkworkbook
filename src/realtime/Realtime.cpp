#include <sstream>
#include <gdk/gdkkeysyms.h>
#include <gtkworkbook/workbook.h>
#include <proactor/Proactor.hpp>
#include <proactor/Event.hpp>
#include "Realtime.hpp"

using namespace realtime;

/* @description: This method creates a filename with the prefix supplied and
   uses the pid of the process as its suffix. 
   @pre: The prefix (should be a file path, obviously). */
static std::string
AppendProcessId (const gchar * pre) {
  std::stringstream s;
  s << pre << getppid();
  return s.str();
}

static void
StreamOpenDialogCallback (GtkWidget * w, gpointer data) {
	Realtime * rt = (Realtime *)data;
	OpenStreamDialog * dialog = rt->streamdialog();
	//Workbook * wb = rt->workbook();
	//Sheet * sheet = wb->focus_sheet;

	if (dialog->widget == NULL) {
		dialog->rt = rt;

		dialog->widget = gtk_dialog_new_with_buttons ("Open stream ", GTK_WINDOW (rt->app()->gtkwindow()),
																	 (GtkDialogFlags) (GTK_DIALOG_MODAL|GTK_DIALOG_NO_SEPARATOR),
																	 GTK_STOCK_OK,
																	 GTK_RESPONSE_OK,
																	 GTK_STOCK_CANCEL,
																	 GTK_RESPONSE_CANCEL,
																	 NULL);
		GtkWidget * gtk_frame = gtk_frame_new ("Stream Options");
		GtkWidget * box = GTK_DIALOG (dialog->widget)->vbox;
		dialog->address_entry = gtk_entry_new_with_max_length (15);

		gtk_container_add (GTK_CONTAINER (gtk_frame), dialog->address_entry);
		gtk_box_pack_start (GTK_BOX (box), gtk_frame, TRUE, TRUE, 0);
		
		g_signal_connect (G_OBJECT (dialog->widget), "delete-event",
								G_CALLBACK (gtk_widget_hide_on_delete), NULL);
	}

	gtk_widget_show_all ( dialog->widget );
	
	if (gtk_dialog_run (GTK_DIALOG (dialog->widget)) == GTK_RESPONSE_OK) {
		const char * entry_value = gtk_entry_get_text (GTK_ENTRY (dialog->address_entry));
		Sheet * sheet = rt->workbook()->add_new_sheet (rt->workbook(), entry_value, 100, 20);
		
		if (sheet == NULL) {
			g_warning ("Failed adding a new sheet to [realtime] workbook");
		}
		else {
			if (rt->Openstream (sheet, "", 2000) == false) {
				
			}
			else {

			}
		}
	}

	gtk_widget_hide_all ( dialog->widget );
}

Realtime::Realtime (Application * appstate, Handle * platform)
	: Plugin (appstate, platform) {
	this->ncd = NULL;
	this->wb = workbook_open (appstate->gtkwindow(), "realtime");

	ConfigPair * logpath =
		appstate->config()->get_pair (appstate->config(), "realtime", "log", "path");

	if (IS_NULL (logpath)) {
		g_critical ("Failed loading log->path from configuration file. Exiting application.");
		exit(1);
	}

	std::string logname = std::string (logpath->value).append("/");
	logname.append (AppendProcessId("realtime.").append(".log"));
	
	if ((pktlog = fopen (logname.c_str(), "w")) == NULL) {
		g_critical ("Failed opening file '%s' for packet logging", logname.c_str());
    }
}

Realtime::~Realtime (void) {
	if (this->ncd != NULL)
		delete this->ncd;
	
	FCLOSE (pktlog);
}

/*
bool
Realtime::Openserver (Sheet * sheet, int port) {
	this->lock();
	
	int eventId = -1;
	if (this->npd == NULL) {
		evenetId = proactor::Event::uniqueEventId();

		this->npd = new network::NetworkPktReceiver (eventId, appstate->proactor());
		if (this->npd->start() == false) {
			g_critical ("Failed starting network packet receiver");
			this->unlock();
			return false;
		}
	}
	else {
		eventId = this->npd->getEventId();
	}

	int port = atoi (servport->value);
	this->server = new network::TcpServerSocket (port);
	if (socket->start (5) == false) {
		g_critical ("Failed starting server socket on port %d", port);
		return false;
	}
	
	this->unlock();
	return true;
}
*/

bool
Realtime::Openstream (Sheet * sheet, const std::string & address, int port) {
	this->lock();

	int eventId = -1;
	if (this->ncd == NULL) {
		eventId = proactor::Event::uniqueEventId();
		
		this->ncd = new NetworkCsvReceiver (eventId, appstate->proactor());
		if (this->ncd->start() == false) {
			g_critical ("Failed starting network csv receiver");
			this->unlock();
			return false;
		}
	}
	else {
		eventId = this->ncd->getEventId();
	}

	network::TcpClientSocket * client = new network::TcpClientSocket;
	if (client->connect (address.c_str(), port) == false) {
		g_critical ("Failed connecting to client socket %s:%d", address.c_str(), port);
		delete client;
		this->unlock();
		return false;
	}
	
	ConnectionThread * reader = new ConnectionThread (this->ncd, client);
	if (this->ncd->addWorker (reader) == false) {
		g_critical ("Failed starting the client reader");
		delete reader;
		delete client;
		this->unlock();
		return false;
	}
	
	this->unlock();
	return true;
}

bool
Realtime::Start() {
	Config * cfg = this->app()->config();
	ConfigPair * servport = cfg->get_pair (cfg, "realtime", "tcp", "port");
	ConfigPair * verbosity = cfg->get_pair (cfg, "realtime", "debug", "verbosity");

	if (IS_NULL (servport)) {
      g_critical ("Failed loading tcp->port from configuration file; "
						"exiting thread");
      return false;
	}

	if (IS_NULL (verbosity))
		g_warning ("Failed loading debug->verbosity from configuration file.");

	this->client = new network::TcpClientSocket;
	if (this->client->connect ("localhost", 50000) == false) {
		g_critical ("Failed connecting to %s:%d", "localhost", 50000);
		return false;
	}

	return true;
}

GtkWidget *
Realtime::CreateMainMenu (void) {
	GtkWidget * rtmenu = gtk_menu_new();
	GtkWidget * rtmenu_item = gtk_menu_item_new_with_label ("Realtime");
	GtkWidget * rtmenu_open = gtk_menu_item_new_with_label ("Open Csv stream...");
	gtk_menu_shell_append (GTK_MENU_SHELL (rtmenu), rtmenu_open);

	g_signal_connect (G_OBJECT (rtmenu_open), "activate",
							G_CALLBACK (StreamOpenDialogCallback), this);
		
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (rtmenu_item), rtmenu);
	return rtmenu_item;
}

GtkWidget *
Realtime::BuildLayout (void) {
	GtkWidget * gtk_menu = this->app()->gtkmenu();
	GtkWidget * box = gtk_vbox_new (FALSE, 0);
	GtkWidget * realtime_menu = this->CreateMainMenu();

	// Append to the existing menu structure from the application.
	gtk_menu_shell_append (GTK_MENU_SHELL (gtk_menu), realtime_menu);

	// Setup the workbook.
	wb->signals[SIG_WORKBOOK_CHANGED] = this->app()->signals[Application::SHEET_CHANGED];
	wb->gtk_box = box;

	// Pack all of the objects into a vertical box, and then pack that box into the application.
	gtk_box_pack_start (GTK_BOX (box), wb->gtk_notebook, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (this->app()->gtkvbox()), box, FALSE, FALSE, 0);
	return box;
}
