#include <workbook/workbook.h>
#include <config/config.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <string>
#include "concurrent/ThreadArgs.hpp"
#include "proactor/Proactor.hpp"
#include "proactor/Event.hpp"
#include "AcceptThread.hpp"
#include "PacketHandler.hpp"
#include "Packet.hpp"

/* @description: This method creates a filename with the prefix supplied and
   uses the pid of the process as its suffix. 
   @pre: The prefix (should be a file path, obviously). */
static std::string
append_pidname (const gchar * pre) {
  std::stringstream s;
  s << pre << getppid();
  return s.str();
}

/* @description: This is the main execution function for the thread.
   @wb: The Workbook that the thread will be changing. */
void
thread_main (ThreadArgs * args) {
  g_usleep (5000000); /* 5 seconds - initial startup delay */

  Workbook * wb = (Workbook *)args->at(0);
  Config * cfg  = (Config *)args->at(1);

  ConfigPair * logpath = cfg->get_pair (cfg, "realtime", "log", "path");
  if (IS_NULL (logpath))
    {
      g_critical ("Failed loading log->path from configuration file; "
		  "exiting thread");
      return;
    }

  ConfigPair * servport = cfg->get_pair (cfg, "realtime", "tcp", "port");
  if (IS_NULL (servport))
    {
      g_critical ("Failed loading tcp->port from configuration file; "
		  "exiting thread");
      return;
    }

  FILE * pktlog = NULL;
  std::string logname = std::string (logpath->value).append("/");
  logname.append (append_pidname("realtime.").append(".log"));

  if ((pktlog = fopen (logname.c_str(), "w")) == NULL)
    {
      g_critical ("Failed opening file '%s' for packet logging; exiting"
		  " thread", logname.c_str());
      return;
    }

  /* Start up the Tcp Socket server on the port specified inside of the
     configuration file. This IS NOT a separate thread. */
  int port = atoi(servport->value);
  TcpServerSocket socket ( port );
  if (socket.start(5) == false)
    {
      g_critical ("Failed starting TcpServerSocket on port localhost:%d;"
		  " exiting thread", port);
      return;
    }
  
  // Get a unique event identifier that will be used throughout.
  int e = proactor::Event::uniqueEventId();

  proactor::Proactor proactor;
  if (proactor.start() == false)
    {
      g_critical ("Failed starting Proactor; exiting thread.");
      return;
    }

  PacketHandler handler (wb, pktlog);
  proactor.registerHandler (e, &handler);
  if (handler.start() == false) 
    {
      g_critical ("Failed starting PacketHandler; exiting thread.");
      return;
    }
  
  AcceptThread acceptor ( socket.newAcceptor(), &proactor );
  acceptor.setEventId (e);
  if (acceptor.start() == false)
    {
      g_critical ("Failed starting Acceptor; exiting thread.");
      return;
    }
 
  while (!IS_NULLSTR (wb->filename)) 
    {
      // Continually sleep basically until our application terminates.
      ::sleep (1);
    }

  FCLOSE (pktlog);

  // Interrupt threads immediately canceling them so we can quit.
  acceptor.interrupt();
  handler.interrupt();
  proactor.interrupt();

  socket.close();  
}
