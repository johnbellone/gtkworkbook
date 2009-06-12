AUTOMAKE_OPTIONS= foreign

CFLAGS= -I/usr/local/include -I/usr/local/include/gtkextra-2.0 -I$(top_srcdir)/include -std=c99 \
	`pkg-config gtk+-2.0 glib-2.0 --cflags`
LDFLAGS= -L/usr/local/lib -L$(top_srcdir)/lib -lcsv -lgtkextra-x11-2.0 -lgtkworkbook \
	`pkg-config gtk+-2.0 glib-2.0 --libs`

bin_PROGRAMS= gtkworkbook
gtkworkbook_SOURCES= application.c config.c main.c parse.c plugin.c
gtkworkbook_CFLAGS= -Wall -lgthread-2.0 $(CFLAGS)  
gtkworkbook_LDFLAGS= -ldl $(LDFLAGS)


lib_LTLIBRARIES= realtime.la
realtime_la_SOURCES= realtime/CsvParser.cpp realtime/Network.cpp realtime/Packet.cpp realtime/plugin_main.cpp \
	realtime/thread_main.cpp realtime/concurrent/Mutex.cpp realtime/concurrent/Semaphore.cpp \
	realtime/concurrent/Thread.cpp realtime/concurrent/ThreadGroup.cpp realtime/concurrent/ThreadPool.cpp \
	realtime/proactor/Dispatcher.cpp realtime/proactor/Event.cpp realtime/proactor/Proactor.cpp \
	realtime/proactor/InputDispatcher.cpp realtime/proactor/Worker.cpp realtime/network/Tcp.cpp
realtime_la_CXXFLAGS= -Wall $(CFLAGS)
realtime_la_LDFLAGS= -module -export-dynamic $(LDFLAGS)