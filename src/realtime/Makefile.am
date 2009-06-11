AUTOMAKE_OPTIONS= foreign

CXXFLAGS= -Wall -I/usr/local/include -I$(top_srcdir)/gtkworkbook/include -DVERSION=\"$(VERSION)\" -DPROG=\"$(PACKAGE)\"
LDFLAGS= -L/usr/local/lib -L$(top_srcdir)/lib -lcsv -ld -lgtkworkbook

lib_LTLIBRARIES= realtime.la
realtime_la_SOURCES= CsvParser.cpp Network.cpp Packet.cpp plugin_main.cpp thread_main.cpp \
	concurrent/Mutex.cpp concurrent/Semaphore.cpp concurrent/Thread.cpp concurrent/ThreadGroup.cpp \
	concurrent/ThreadPool.cpp proactor/Dispatcher.cpp proactor/Event.cpp proactor/Proactor.cpp \
	proactor/InputDispatcher.cpp proactor/Worker.cpp network/Tcp.cpp
realtime_la_LDFLAGS= -module -export-dynamic $(LDFLAGS)
