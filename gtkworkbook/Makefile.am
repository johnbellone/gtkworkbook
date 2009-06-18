AUTOMAKE_OPTIONS= foreign

C_FLAGS= -I/usr/local/include -I/usr/local/include/gtkextra-2.0 -std=c99 `pkg-config gtk+-2.0 glib-2.0 --cflags`
L_FLAGS= -L/usr/local/lib -lgtkextra-x11-2.0 -lgthread-2.0 `pkg-config gtk+-2.0 glib-2.0 --libs`

lib_LTLIBRARIES= libgtkworkbook.la
libgtkworkbook_la_SOURCES= workbook.c sheet.c cell.c
libgtkworkbook_la_CFLAGS= -Wall -I$(top_srcdir)/include $(C_FLAGS)
libgtkworkbook_la_LFLAGS= $(L_FLAGS)
