AUTOMAKE_OPTIONS= foreign

CFLAGS= -std=c99 `pkg-config gtk+-2.0 glib-2.0 --cflags`
LDFLAGS= `pkg-config gtk+-2.0 glib-2.0 --libs`

noinst_LIBRARIES= libgtkworkbook.a
libgtkworkbook_a_SOURCES= workbook.c sheet.c cell.c
libgtkworkbook_a_CFLAGS= -Wall -I/usr/local/include -I/usr/local/include/gtkextra-2.0 -I$(top_srcdir)/include $(CFLAGS)
libgtkworkbook_a_LFLAGS= -L/usr/local/lib -lgtkextra-x11-2.0 -lgthread-2.0 $(LDFLAGS)