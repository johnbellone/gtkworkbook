MD=mkdir -p
MAKE=-make -C
PWD=`pwd`

all:	libconfig libworkbook gtkcsv

libconfig:
	${MD} OBJS/libconfig
	${MAKE} config/ all

libworkbook:
	${MD} OBJS/libworkbook
	${MAKE} workbook/ all

gtkcsv:
	${MD} OBJS/realtime
	$(MAKE) src/ all

clean:
	$(MAKE) config/ clean
	${MAKE} workbook/ clean
	$(MAKE) src/ clean

install: all
	$(MAKE) config/ install
	${MAKE} workbook/ install
	${MAKE} src/ install
