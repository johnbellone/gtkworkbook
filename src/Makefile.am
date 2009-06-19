AUTOMAKE_OPTIONS= foreign

C_FLAGS= -I/usr/local/include -I/usr/include -I/usr/local/include/gtkextra-2.0 -I$(top_srcdir)/include `pkg-config gtk+-2.0 glib-2.0 --cflags`
L_FLAGS= -L/usr/local/lib -L/usr/lib -L$(top_srcdir)/lib `pkg-config gtk+-2.0 glib-2.0 --libs` -lgtkextra-x11-2.0

bin_PROGRAMS= gtkworkbook
gtkworkbook_SOURCES= application.c config.c main.c parse.c plugin.c
gtkworkbook_CFLAGS= -Wall -lgthread-2.0 -std=c99 $(C_FLAGS)  
gtkworkbook_LFLAGS= -ldl $(L_FLAGS)
gtkworkbook_LDFLAGS= $(L_FLAGS)
gtkworkbook_LDADD= ../gtkworkbook/libgtkworkbook.la ../csv/libcsv.la

lib_LTLIBRARIES= realtime.la
realtime_la_SOURCES= realtime/CsvParser.cpp realtime/Network.cpp realtime/Packet.cpp realtime/plugin_main.cpp \
	realtime/thread_main.cpp realtime/concurrent/Mutex.cpp realtime/concurrent/Semaphore.cpp \
	realtime/concurrent/Thread.cpp realtime/concurrent/ThreadGroup.cpp realtime/concurrent/ThreadPool.cpp \
	realtime/proactor/Dispatcher.cpp realtime/proactor/Event.cpp realtime/proactor/Proactor.cpp \
	realtime/proactor/InputDispatcher.cpp realtime/proactor/Worker.cpp realtime/network/Tcp.cpp
realtime_la_CPPFLAGS= -Wall -Wno-write-strings $(C_FLAGS)
realtime_la_LFLAGS= -lgtkworkbook -lcsv $(L_FLAGS)
realtime_la_LDFLAGS= -module -export-dynamic 
realtime_la_LIBADD= ../gtkworkbook/libgtkworkbook.la ../csv/libcsv.la