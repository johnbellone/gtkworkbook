MD=mkdir -p
MAKE=-make -C
PWD=`pwd`

all:	libshared libconfig libworkbook libcsv gtkworkbook

libshared:
	${MD} OBJS/libshared
	${MAKE} shared/ all

libcsv:
	${MD} OBJS/libcsv
	${MAKE} csv/ all

libconfig:
	${MD} OBJS/libconfig
	${MAKE} config/ all

libworkbook:
	${MD} OBJS/libworkbook
	${MAKE} workbook/ all

gtkworkbook:
	${MD} OBJS/realtime
	$(MAKE) src/ all

clean:
	${MAKE} shared/ clean
	$(MAKE) config/ clean
	${MAKE} workbook/ clean
	${MAKE} csv/ clean
	$(MAKE) src/ clean

install: all
	${MAKE} shared/ install
	$(MAKE) config/ install
	${MAKE} workbook/ install
	${MAKE} csv/ install
	${MAKE} src/ install
