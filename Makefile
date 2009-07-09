MD=mkdir -p
MAKE=-make -C
PWD=`pwd`

all:	libgtkworkbook gtkworkbook

libgtkworkbook:
	${MD} OBJS/libgtkworkbook
	${MAKE} gtkworkbook/ all

gtkworkbook:
	${MD} OBJS/realtime
	$(MAKE) src/ all

clean:
	${MAKE} gtkworkbook/ clean
	$(MAKE) src/ clean

install: all
	${MAKE} gtkworkbook/ install
	${MAKE} src/ install
