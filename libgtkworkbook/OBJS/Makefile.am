AUTOMAKE_OPTIONS= foreign

C_FLAGS= -I/usr/local/include -I/usr/include -I/usr/local/include/gtkextra-2.0 -I$(top_srcdir)/include `pkg-config gtk+-2.0 glib-2.0 --cflags` -I../../include
L_FLAGS= -L/usr/local/lib -L/usr/lib `pkg-confg gtk+-2.0 glib-2.0 --libs` -lgtkextra-x11-2.0

SOURCE= ..

noinst_LTLIBRARIES= libgtkworkbook.la
libgtkworkbook_la_SOURCES= $(SOURCE)/cell.c $(SOURCE)/workbook.c $(SOURCE)/sheet.c
libgtkworkbook_la_CFLAGS= -fPIC -Wall -Wno-write-strings -std=c99 $(C_FLAGS)
libgtkworkbook_la_LFLAGS= $(L_FLAGS) -module -export-dynamic