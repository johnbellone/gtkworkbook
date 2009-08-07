MD=mkdir -p
MAKE=-make -C
PWD=`pwd`

all:	libgtkworkbook libgtkworkbookshared gtkworkbookapp

libgtkworkbook:
	${MD} OBJS/libgtkworkbook
	${MAKE} gtkworkbook/ all

libgtkworkbookshared:
	${MD} OBJS/libgtkworkbookshared
	${MAKE} shared/ all

gtkworkbookapp:
	${MD} OBJS/realtime OBJS/largefile
	$(MAKE) src/ all

clean:
	${MAKE} shared/ clean
	${MAKE} gtkworkbook/ clean
	$(MAKE) src/ clean

install: all
	${MAKE} shared/ install
	${MAKE} gtkworkbook/ install
	${MAKE} src/ install
