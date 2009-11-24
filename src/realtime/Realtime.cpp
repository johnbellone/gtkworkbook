/*
  The GTKWorkbook Project <http://gtkworkbook.sourceforge.net/>
  Copyright (C) 2008, 2009 John Bellone, Jr. <jvb4@njit.edu>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PRACTICAL PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with the library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301 USA
*/
#include <sstream>
#include <gdk/gdkkeysyms.h>
#include <libgtkworkbook/workbook.h>
#include <proactor/Proactor.hpp>
#include <proactor/Event.hpp>
#include <network/Tcp.hpp>
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

	if (dialog->widget == NULL) {
		dialog->rt = rt;

		dialog->widget = gtk_dialog_new_with_buttons ("Open stream ", GTK_WINDOW (rt->app()->gtkwindow()),
																	 (GtkDialogFlags) (GTK_DIALOG_MODAL|GTK_DIALOG_NO_SEPARATOR),
																	 GTK_STOCK_OK,
																	 GTK_RESPONSE_OK,
																	 GTK_STOCK_CANCEL,
																	 GTK_RESPONSE_CANCEL,
																	 NULL);
		
		GtkWidget * gtk_frame = gtk_frame_new ("Connection Options");
		GtkWidget * hbox = gtk_hbox_new(FALSE, 0);
		GtkWidget * box = GTK_DIALOG (dialog->widget)->vbox;
		dialog->host_entry = gtk_entry_new();
		dialog->port_entry = gtk_entry_new();

		gtk_entry_set_max_length (GTK_ENTRY (dialog->host_entry), 15);
		gtk_entry_set_max_length (GTK_ENTRY (dialog->port_entry), 5);
		gtk_entry_set_width_chars (GTK_ENTRY (dialog->host_entry), 15);
		gtk_entry_set_width_chars (GTK_ENTRY (dialog->port_entry), 5);
		
		gtk_box_pack_start (GTK_BOX (hbox), dialog->host_entry, FALSE, FALSE, 0);
		gtk_box_pack_end (GTK_BOX (hbox), dialog->port_entry, FALSE, FALSE, 0);
		gtk_container_add (GTK_CONTAINER (gtk_frame), hbox);
		gtk_box_pack_start (GTK_BOX (box), gtk_frame, FALSE, FALSE, 0);
		
		g_signal_connect (G_OBJECT (dialog->widget), "delete-event",
								G_CALLBACK (gtk_widget_hide_on_delete), NULL);
	}

	gtk_widget_show_all ( dialog->widget );
	
	if (gtk_dialog_run (GTK_DIALOG (dialog->widget)) == GTK_RESPONSE_OK) {
		const char * host_value = gtk_entry_get_text (GTK_ENTRY (dialog->host_entry));
		const char * port_value = gtk_entry_get_text (GTK_ENTRY (dialog->port_entry));
		Sheet * sheet = rt->workbook()->add_new_sheet (rt->workbook(), host_value, 100, 20);

		if (IS_NULLSTR (host_value) || IS_NULLSTR (port_value)) {
			g_warning ("One of requird values are empty");
		}
		else if (sheet == NULL) {
			g_warning ("Cannot open connection to %s:%s because of failure to add sheet",
						  host_value, port_value);
		}
		else if (rt->OpenTcpClient (sheet, host_value, atoi (port_value)) == false) {
			// STUB: Popup an alertbox about failing to connect?
		}
		else {
			// STUB: Success. Do we want to do anything else here?
			g_message ("Client connection opened on %s:%s on sheet %s", host_value, port_value, sheet->name);
		}
	}

	gtk_widget_hide_all ( dialog->widget );
}

Realtime::Realtime (Application * appstate, Handle * platform)
	: Plugin (appstate, platform) {
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

	this->wb = workbook_open (appstate->gtkwindow(), "realtime");
	this->packet_parser = NULL;
	this->tcp_server = NULL;
}

Realtime::~Realtime (void) {
	// Iterate through the list of active connections, and begin closing them. This should also
	// include deleting the pointers to all of the accepting threads. Eventually there should be
	// a boost::shared_ptr here so that we don't have to do the dirty work.
	ActiveThreads::iterator it = this->threads.begin();
	while (it != this->threads.end()) {
		network::TcpSocket * socket = ((*it).first);
		concurrent::Thread * thread = ((*it).second);

		it = this->threads.erase (it);
				
		if (socket) delete socket;
		if (thread) {
			thread->stop();
			delete thread;
		}
	}

	if (this->packet_parser) {
		this->packet_parser->stop();
		delete this->packet_parser;
	}

	if (this->tcp_server) {
		this->tcp_server->stop();
		delete this->tcp_server;
	}
	
	FCLOSE (this->pktlog);
}

bool
Realtime::CreateNewServerConnection (network::TcpServerSocket * socket, AcceptThread * accept_thread) {
	this->threads.push_back ( ActiveThread (socket, accept_thread) );

	if (this->tcp_server->addWorker (accept_thread) == false) {
		g_critical ("Failed starting accepting thread on socket %d", socket->getPort() );
		return false;
	}

	return true;
}

bool
Realtime::CreateNewClientConnection (network::TcpClientSocket * socket, CsvParser * csv, NetworkDispatcher * nd) {
	this->threads.push_back ( ActiveThread (socket, csv) );
	this->threads.push_back ( ActiveThread (NULL, nd) );  // this is a hack

	ConnectionThread * reader = new ConnectionThread (socket);
	if (nd->addWorker (reader) == false) {
		g_critical ("Failed starting the client reader");
		delete reader;
		return false;
	}

	this->threads.push_back ( ActiveThread (NULL, reader) ); // this is a hack
	return true;
}

bool
Realtime::OpenTcpServer (int port) {
	// Has to be above the service ports.
	if (port < 1000) {
		g_warning ("Failed starting Tcp server: port (%d) must be above 1000", port);
		return false;
	}

	// The first time we attempt to create a port to receive input on we need to create a dispatcher, and
	// specify an event identifier so that we can communicate with it from workers. At this point the
	// Packet Parser is created as well. 
	if (this->tcp_server == NULL) {
		int eventId = proactor::Event::uniqueEventId();
		NetworkDispatcher * nd = new NetworkDispatcher (eventId);
		PacketParser * pp = new PacketParser (this->workbook(), this->pktlog, 0);
		
		if (nd->start() == false) {
			g_critical ("Failed starting network dispatcher for tcp server");
			return false;
		}

		if (this->app()->proactor()->addWorker (eventId, pp) == false) {
			g_critical ("Failed starting packet parser for tcp server");
			return false;
		}
		
		this->tcp_server = nd;
		this->packet_parser = pp;
	}

	network::TcpServerSocket * socket = new network::TcpServerSocket (port);
	if (socket->start(5) == false) {
		g_critical ("Failed starting network socket for tcp server on port %d", port);
		return false;
	}

	AcceptThread * accept_thread = new AcceptThread (socket->newAcceptor());
	return this->CreateNewServerConnection (socket, accept_thread);
}

bool
Realtime::OpenTcpClient (Sheet * sheet, const std::string & address, int port) {
	// Has to be above the service ports.
	if (port < 1000) {
		g_warning ("Failed starting Tcp client: port (%d) must be above 1000", port);
		return false;
	}

	// We need to create a network dispatcher for each one of these connections because of
	// the current limitation of the Proactor design. It really needs to be rewritten, but
	// that is a separate project in and of itself. For now a list of dispatchers must be
	// kept so that we do not lose track.
	int eventId = proactor::Event::uniqueEventId();
	NetworkDispatcher * dispatcher = new NetworkDispatcher (eventId);
	if (dispatcher->start() == false) {
		g_critical ("Failed starting network dispatcher for %s:%d", address.c_str(), port);
		delete dispatcher;
		return false;
	}

	// Keeping this simple is the reason why we need multiple dispatchers. If I could come
	// up with a simple way to strap on the ability to have multiple sheets without the need
	// for an additioanl dispatcher/csv combo I would. It totally destroys the principle of
	// the proactor design.
	CsvParser * csv = new CsvParser (sheet, this->pktlog, 0);
	if (this->app()->proactor()->addWorker (eventId, csv) == false) {
		g_critical ("Failed starting csv parser and adding to proactor for %s:%d",
						address.c_str(), port);
		delete csv;
		delete dispatcher;
		return false;
	}

	network::TcpClientSocket * socket = new network::TcpClientSocket;
	if (socket->connect (address.c_str(), port) == false) {
		g_critical ("Failed making Tcp connection to %s:%d", address.c_str(), port);
		delete socket;
		delete csv;
		delete dispatcher;
		return false;
	}
	
	return this->CreateNewClientConnection (socket, csv, dispatcher);
}

void
Realtime::Start(void) {
	Config * cfg = this->app()->config();
	ConfigPair * servport = cfg->get_pair (cfg, "realtime", "tcp", "port");
	int port = atoi (servport->value);

	if (this->OpenTcpServer (port) == true) {
		g_message ("Opened Tcp server on port %d", port);
	}
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
	gtk_menu_shell_prepend (GTK_MENU_SHELL (gtk_menu), realtime_menu);

	// Setup the workbook.
	wb->signals[SIG_WORKBOOK_CHANGED] = this->app()->signals[Application::SHEET_CHANGED];
	wb->gtk_box = box;

	// Pack all of the objects into a vertical box, and then pack that box into the application.
	gtk_box_pack_start (GTK_BOX (box), wb->gtk_notebook, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (this->app()->gtkvbox()), box, TRUE, TRUE, 0);
	return box;
}
