AUTOMAKE_OPTIONS= foreign

CFLAGS= -Wall -I/usr/local/include -DVERSION=\"$(VERSION)\" -DPROG=\"$(PACKAGE)\"
LDFLAGS= -L/usr/local/lib -ld

bin_PROGRAMS= gtkworkbook
gtkworkbook_SOURCES= application.c config.c main.c parse.c plugin.c
