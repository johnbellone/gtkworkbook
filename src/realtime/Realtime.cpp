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

	
	
	this->unlock();
	return true;
}

bool
Realtime::Openstream (Sheet * sheet, const std::string & address, int port) {
	this->lock();

	int eventId = -1;
	if (this->ncd == NULL) {
		eventId = proactor::Event::uniqueEventId();
		
		this->ncd = new network::NetworkCsvReceiver (eventId, appstate->proactor());
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
	
	ConnectionThread * reader = new network::ConnectionThread (this->ncd, client);
	if (nd->addWorker (reader) == false) {
		g_critical ("Failed starting the client reader");
		delete reader;
		delete client;
		delete csv;
		this->unlock();
		return false;
	}
	
	this->unlock();
	return true;
}

bool
Realtime::Start() {
	Config * cfg = this->app()->cfg;
	
	ConfigPair * servport = cfg->get_pair (cfg, "realtime", "tcp", "port");
	ConfigPair * verbosity = cfg->get_pair (cfg, "realtime", "debug", "verbosity");

	if (IS_NULL (servport)) {
      g_critical ("Failed loading tcp->port from configuration file; "
						"exiting thread");
      return;
	}

	if (IS_NULL (verbosity))
		g_warning ("Failed loading debug->verbosity from configuration file.");

	this->client = new network::TcpClientSocket;
	if (client->connect ("localhost", 50000) == false) {
		g_critical ("Failed connecting to %s:%d", "localhost", 50000);
		return false;
	}

	int port = atoi (servport->value);
	this->server = new network::TcpServerSocket (port);
	if (socket->start (5) == false) {
		g_critical ("Failed starting server socket on port %d", port);
		return false;
	}
}

GtkWidget *
Realtime::CreateMainMenu (void) {
	GtkWidget * rtmenu = gtk_menu_new();
	GtkWidget * rtmenu_item = gtk_menu_item_new_with_label ("Realtime");

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
