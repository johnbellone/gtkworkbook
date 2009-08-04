#ifndef HPP_REALTIME
#define HPP_REALTIME

#include <map>
#include <string>
#include "../Plugin.hpp"
#include "../Application.hpp"
#include "../config.h"
#include "OpenStreamDialog.hpp"
#include "Network.hpp"

namespace realtime {

	class Realtime : public Plugin {
	private:
		OpenStreamDialog stream_dialog;
		FILE * pktlog;
		NetworkCsvReceiver * ncd;
		NetworkPktReceiver * npd;
		network::TcpClientSocket * client;
		
		GtkWidget * CreateMainMenu (void);
	public:
		Realtime (Application * appstate, Handle * platform);
		virtual ~Realtime (void);

		GtkWidget * BuildLayout (void);

		bool Openstream (Sheet * sheet, const std::string & address, int port);
		bool Start (void);
		
		inline OpenStreamDialog * streamdialog() { return &this->stream_dialog; }
	};
}

#endif
