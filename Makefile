MD=mkdir -p
MAKE=-make -C
PWD=`pwd`

all:	libgtkworkbook gtkworkbookapp

libgtkworkbook:
	${MD} OBJS/libgtkworkbook
	${MAKE} gtkworkbook/ all

gtkworkbookapp:
	${MD} OBJS/realtime OBJS/largefile OBJS/shared
	$(MAKE) src/ all

clean:
	${MAKE} gtkworkbook/ clean
	$(MAKE) src/ clean

install: all
	${MAKE} gtkworkbook/ install
	${MAKE} src/ install
