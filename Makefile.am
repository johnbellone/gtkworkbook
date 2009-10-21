# GTKWorkbook Automake Script
# @author: jb <jvb4@njit.edu>

cfgdir= $(prefix)/cfg

CFLAGS= -g -O0
CXXFLAGS= -g -O0

C_FLAGS= -I/usr/local/include -I/usr/include -I./include -I./src/shared `pkg-config gtk+-2.0 glib-2.0 --cflags` \
-I$(prefix)/include -I$(prefix)/include/gtkextra-2.0 -I/usr/local/include/gtkextra-2.0
L_FLAGS= -L/usr/local/lib -L/usr/lib -L$(prefix)/lib `pkg-config gtk+-2.0 glib-2.0 --libs` -lgtkextra-x11-2.0

lib_LTLIBRARIES = lib/libgtkworkbook.la lib/realtime.la lib/largefile.la
bin_PROGRAMS = lib/gtkworkbook

# libgtkworkbook
lib_libgtkworkbook_la_CFLAGS = -fPIC -Wall -Wno-write-strings -std=c99 $(C_FLAGS) 
lib_libgtkworkbook_la_LFLAGS = $(L_FLAGS) -module -export-dynamic 
lib_libgtkworkbook_la_SOURCES = libgtkworkbook/cell.c \
	  	                libgtkworkbook/workbook.c \
			        libgtkworkbook/sheet.c \
			        libgtkworkbook/row.c

# realtime
lib_realtime_la_CPPFLAGS = -fPIC -Wall -Wno-write-strings $(C_FLAGS) 
lib_realtime_la_LFLAGS = $(L_FLAGS) -lgtkworkbook -lcsv -lgthread-2.0
lib_realtime_la_LDFLAGS = $(L_FLAGS) -lgtkworkbook -lcsv -module -export-dynamic -lgthread-2.0
lib_realtime_la_SOURCES = src/realtime/Network.cpp \
		          src/realtime/Packet.cpp \
		          src/realtime/PacketParser.cpp \
	       	          src/realtime/PluginFactory.cpp \
		          src/realtime/Realtime.cpp

# largefile
lib_largefile_la_CPPFLAGS = -fPIC -Wall -Wno-write-strings $(C_FLAGS) 
lib_largefile_la_LFLAGS = $(L_FLAGS) -lgtkworkbook -lcsv -lgthread-2.0
lib_largefile_la_LDFLAGS = $(L_FLAGS) -lgtkworkbook -lcsv -module -export-dynamic -lgthread-2.0
lib_largefile_la_SOURCES = src/largefile/Largefile.cpp \
		           src/largefile/File.cpp \
		           src/largefile/PluginFactory.cpp 

# gtkworkbook
lib_gtkworkbook_CPPFLAGS= -Wall  -rdynamic $(C_FLAGS)
lib_gtkworkbook_LFLAGS= -ldl $(L_FLAGS) -lgtkworkbook -lcsv -lgthread-2.0
lib_gtkworkbook_LDFLAGS= $(L_FLAGS) -lgtkworkbook -lcsv -rdynamic -lgthread-2.0
lib_gtkworkbook_LDADD= lib/libgtkworkbook.la
lib_gtkworkbook_SOURCES = src/Application.cpp \
		          src/config.cpp src/main.cpp \
		          src/parse.cpp src/Plugin.cpp \
		          src/RecordView.cpp \
		          src/shared/concurrent/Mutex.cpp \
		          src/shared/concurrent/Runnable.cpp \
		          src/shared/concurrent/ScopedMemoryLock.cpp \
		          src/shared/concurrent/Semaphore.cpp \
		          src/shared/concurrent/Thread.cpp \
		          src/shared/concurrent/ThreadGroup.cpp \
		          src/shared/concurrent/ThreadPool.cpp \
		          src/shared/network/Tcp.cpp \
		          src/shared/proactor/Dispatcher.cpp \
		          src/shared/proactor/Event.cpp \
		          src/shared/proactor/InputDispatcher.cpp \
		          src/shared/proactor/Proactor.cpp \
		          src/shared/proactor/Worker.cpp \
		          src/shared/workers/CsvParser.cpp

install-data-hook:
	mkdir -p $(cfgdir)
	cp $(top_srcdir)/cfg/application.cfg $(cfgdir)/application.cfg