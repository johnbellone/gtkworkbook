MD=mkdir -p
MAKE=-make -C
PWD=`pwd`

all:	libgtkworkbookshared libgtkworkbook gtkworkbook

libgtkworkbook:
	${MD} OBJS/libgtkworkbook
	${MAKE} gtkworkbook/ all

libgtkworkbookshared:
	${MD} OBJS/libgtkworkbookshared
	${MAKE} shared/ all

gtkworkbook:
	${MD} OBJS/realtime
	$(MAKE) src/ all

clean:
	${MAKE} shared/ clean
	${MAKE} gtkworkbook/ clean
	$(MAKE) src/ clean

install: all
	${MAKE} shared/ install
	${MAKE} gtkworkbook/ install
	${MAKE} src/ install
