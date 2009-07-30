#ifndef HPP_REALTIME
#define HPP_REALTIME

#include <map>
#include <string>
#include "../Plugin.hpp"
#include "../Application.hpp"
#include "../config.h"

namespace realtime {

	class Realtime : public Plugin {
	private:
		FILE * pktlog;
		network::NetworkCsvReceiver * ncd;
		network::NetworkPktReceiver * npd;
		
		GtkWidget * CreateMainMenu (void);
	public:
		Realtime (Application * appstate, Handle * platform);
		virtual ~Realtime (void);

		GtkWidget * BuildLayout (void);
	};
}

#endif
