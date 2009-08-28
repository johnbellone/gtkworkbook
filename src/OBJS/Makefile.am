AUTOMAKE_OPTIONS= foreign

CXXFLAGS= -g -O0

C_FLAGS= -I/usr/local/include -I/usr/include -I/usr/local/include/gtkextra-2.0 -I$(top_srcdir)/include `pkg-config gtk+-2.0 glib-2.0 --cflags` -I../shared -I$(HOME)/include/gtkextra-2.0 -I$(HOME)/include
L_FLAGS= -L/usr/local/lib -L/usr/lib -L$(HOME)/lib `pkg-config gtk+-2.0 glib-2.0 --libs` \
-lgtkextra-x11-2.0 -lgthread-2.0

SOURCE= ..
CONCURRENT= ../shared/concurrent
PROACTOR= ../shared/proactor
NETWORK= ../shared/network

bin_PROGRAMS= gtkworkbook
gtkworkbook_SOURCES= $(SOURCE)/Application.cpp $(SOURCE)/config.cpp $(SOURCE)/main.cpp $(SOURCE)/parse.cpp \
$(SOURCE)/Plugin.cpp $(SOURCE)/RecordView.cpp $(CONCURRENT)/Mutex.cpp $(CONCURRENT)/Runnable.cpp \
$(CONCURRENT)/ScopedMemoryLock.cpp $(CONCURRENT)/Semaphore.cpp $(CONCURRENT)/Thread.cpp $(CONCURRENT)/ThreadGroup.cpp \
$(CONCURRENT)/ThreadPool.cpp $(NETWORK)/Tcp.cpp $(PROACTOR)/Dispatcher.cpp $(PROACTOR)/Event.cpp \
$(PROACTOR)/InputDispatcher.cpp $(PROACTOR)/Proactor.cpp $(PROACTOR)/Worker.cpp
gtkworkbook_CPPFLAGS= -Wall $(C_FLAGS) -rdynamic
gtkworkbook_LFLAGS= -ldl $(L_FLAGS) -lgtkworkbook -lcsv
gtkworkbook_LDFLAGS= $(L_FLAGS) -lgtkworkbook -lcsv -rdynamic
gtkworkbook_LDADD= ../../libgtkworkbook/OBJS/libgtkworkbook.la

lib_LTLIBRARIES= realtime.la largefile.la

realtime_la_SOURCES= $(SOURCE)/realtime/CsvParser.cpp $(SOURCE)/realtime/Network.cpp $(SOURCE)/realtime/Packet.cpp \
$(SOURCE)/realtime/PacketParser.cpp $(SOURCE)/realtime/PluginFactory.cpp $(SOURCE)/realtime/Realtime.cpp
realtime_la_CPPFLAGS= -fPIC -Wall -Wno-write-strings $(C_FLAGS)
realtime_la_LFLAGS= $(L_FLAGS) -lgtkworkbook -lcsv
realtime_la_LDFLAGS= $(L_FLAGS) -lgtkworkbook -lcsv -module -export-dynamic 

largefile_la_SOURCES= $(SOURCE)/largefile/CsvParser.cpp $(SOURCE)/largefile/File.cpp \
$(SOURCE)/largefile/PluginFactory.cpp $(SOURCE)/largefile/Largefile.cpp
largefile_la_CPPFLAGS= -fPIC -Wall -Wno-write-strings $(C_FLAGS)
largefile_la_LFLAGS= $(L_FLAGS) -lgtkworkbook -lcsv
largefile_la_LDFLAGS= $(L_FLAGS) -lgtkworkbook -lcsv -module -export-dynamic
