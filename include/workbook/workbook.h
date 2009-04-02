/* @author: John `jb Bellone <jvb4@njit.edu> */
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
