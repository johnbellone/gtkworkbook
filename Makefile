MD=mkdir -p
MAKE=-make -C
PWD=`pwd`

all:	libconfig libworkbook libcsv gtkworkbook

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
	$(MAKE) config/ clean
	${MAKE} workbook/ clean
	${MAKE} csv/ clean
	$(MAKE) src/ clean

install: all
	$(MAKE) config/ install
	${MAKE} workbook/ install
	${MAKE} csv/ install
	${MAKE} src/ install
