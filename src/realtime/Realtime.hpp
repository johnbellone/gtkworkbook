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
#ifndef HPP_REALTIME
#define HPP_REALTIME

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <concurrent/Thread.hpp>
#include <workers/CsvParser.hpp>
#include "../Plugin.hpp"
#include "../Application.hpp"
#include "../config.h"
#include "OpenStreamDialog.hpp"
#include "Network.hpp"
#include "PacketParser.hpp"

namespace realtime {

	class Realtime : public Plugin {
	private:
		typedef std::pair <network::TcpSocket *, concurrent::Thread *> ActiveThread;
		typedef std::vector <ActiveThread> ActiveThreads;
			
		ActiveThreads threads;
		OpenStreamDialog stream_dialog;
		FILE * pktlog;
		NetworkDispatcher * tcp_server;
		PacketParser * packet_parser;
		
		GtkWidget * CreateMainMenu (void);
	public:
		Realtime (Application * appstate, Handle * platform);
		virtual ~Realtime (void);

		GtkWidget * BuildLayout (void);

		bool CreateNewServerConnection (network::TcpServerSocket * socket, AcceptThread * accept_thread);
		bool CreateNewClientConnection (network::TcpClientSocket * socket, CsvParser * csv, NetworkDispatcher * nd);
		bool OpenTcpServer (int port);
		bool OpenTcpClient (Sheet * sheet, const std::string & address, int port);
		void Start (void);
		
		inline OpenStreamDialog * streamdialog (void) { return &this->stream_dialog; }
	};
}

#endif
