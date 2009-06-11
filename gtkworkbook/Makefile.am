AUTOMAKE_OPTIONS= foreign

CFLAGS= -Wall -I/usr/local/include -DVERSION=\"$(VERSION)\" -DPROG=\"$(PACKAGE)\"
LDFLAGS= -L/usr/local/lib -ldl -lgtkextra-x11-2.0 -lgthread-2.0

noinst_LIBRARIES= libgtkworkbook.a
libgtkworkbook_a_SOURCES= workbook.c sheet.c cell.c
