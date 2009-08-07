#ifndef HPP_REALTIME
#define HPP_REALTIME

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <concurrent/Thread.hpp>
#include "../Plugin.hpp"
#include "../Application.hpp"
#include "../config.h"
#include "OpenStreamDialog.hpp"
#include "Network.hpp"
#include "PacketParser.hpp"
#include "CsvParser.hpp"

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
		
		inline OpenStreamDialog * streamdialog() { return &this->stream_dialog; }
	};
}

#endif
