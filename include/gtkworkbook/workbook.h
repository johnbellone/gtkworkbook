/* 
   workbook.h - Workbook Object Header File

   The GTKWorkbook Project <http://gtkworkbook.sourceforge.net/>
   Copyright (C) 2008, 2009 John Bellone, Jr. <jvb4@njit.edu>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PRACTICAL PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301 USA
*/
#ifndef H_WORKBOOK
#define H_WORKBOOK

#include <shared.h>
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

  enum
    {
      SIG_WORKBOOK_CHANGED = 0,
      /**/
      MAX_WORKBOOK_SIGNALS
    };

  typedef struct _Workbook Workbook;

#include "sheet.h"

  struct _Workbook
  {
    /* Members */
    GSourceFunc signals[MAX_WORKBOOK_SIGNALS];
    Sheet * sheet_first;
    Sheet * sheet_last;
    Sheet * focus_sheet;
    Workbook * next;
    Workbook * prev;  
    GtkWidget * gtk_notebook;
    GtkWidget * gtk_window;
    GtkWidget * gtk_box;
    gchar * filename;

    /* Methods */
    void (*destroy) (Workbook *);
    Sheet *(*add_new_sheet) (Workbook *, const gchar *, gint, gint);
    Sheet *(*get_sheet) (Workbook *, const gchar *);
    void (*remove_sheet) (Workbook *, Sheet *);
    gboolean (*move_sheet_index) (Workbook *, Sheet *, gint);
    gboolean (*move_sheet) (Workbook *, Sheet *, const gchar *, gboolean); 
  };

  /* workbook.c */
  Workbook *workbook_open (GtkWidget *, const gchar *);

#ifdef __cplusplus
}
#endif
#endif /*H_WORKBOOK*/
