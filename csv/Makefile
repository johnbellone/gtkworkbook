CC=gcc
AR=ar
CP=cp

VERSION=3.0.0
DESTDIR=
LIBDIR=/usr/lib
INCDIR=/usr/include
MANDIR=/usr/share/man/man3

.PHONnY: all install install_headers install_man install_shared install_static clean test

all: libcsv.so libcsv.a

install: install_headers install_shared_lib install_static_lib install_man

install_shared: install_headers install_shared_lib
install_static: install_headers install_static_lib

install_man: csv.3.gz
	cp -f $^ $(DESTDIR)$(MANDIR)/

install_headers: csv.h
	mkdir -p $(DESTDIR)$(INCDIR)/libcsv/
	cp -f $^ $(DESTDIR)$(INCDIR)/libcsv/

install_shared_lib: libcsv.so
	cp -f $< $(DESTDIR)$(LIBDIR)/$<.$(VERSION)
	ln -sf $<.$(VERSION) $(DESTDIR)$(LIBDIR)/$<.3
	ln -sf $<.3 $(DESTDIR)$(LIBDIR)/$<

install_static_lib: libcsv.a
	cp -f $< $(DESTDIR)$(LIBDIR)/$<

libcsv.o: libcsv.c csv.h
	$(CC) -fPIC $< -c -o $@

libcsv.so: libcsv.o
	$(CC) -shared $< -o $@
	$(CP) $@ ../lib/$@
	$(CP) ../lib/$@ ../lib/$@.1

libcsv.a: libcsv.o
	$(AR) -rc $@ $^
	$(AR) -s $@

clean:
	rm libcsv.o libcsv.so libcsv.a
	rm ../lib/libcsv.*

test: libcsv.o
	$(CC) test_csv.c $< -o $@
	./test
	rm ./test

