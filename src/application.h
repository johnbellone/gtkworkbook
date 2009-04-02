/* @author: John `jb Bellone <jvb4@njit.edu> */
#ifndef H_APPLICATION
#define H_APPLICATION

typedef struct _ApplicationState ApplicationState;

#include <gtk/gtk.h>
#include <workbook/workbook.h>
#include <config/config.h>
#include <shared.h>
#include "plugin.h"

enum
  {
    SIG_DESTROY_EVENT = 0,
    SIG_DELETE_EVENT,
    SIG_NOTEBOOK_SWITCHED,
    SIG_NOTEBOOK_REMOVED,
    SIG_NOTEBOOK_REORDERED,
    SIG_SHEET_CHANGED,
    /**/
    MAX_SIGNALS
  };

struct _ApplicationState
{
  /* Members */
  GSourceFunc signals[MAX_SIGNALS];
  Config * cfg;
  Workbook * workbook_first, * workbook_last;
  Plugin * plugin_first, * plugin_last;
  GtkWidget * gtk_window;
  GtkWidget * gtk_menu;
  GtkWidget * gtk_window_vbox;
  gchar * absolute_path;

  /* Methods */
  int (*run) (ApplicationState *);
  void (*close) (ApplicationState *);
  void (*open_extension) (ApplicationState *, const gchar *, gboolean);
  Plugin *(*load_plugin) (ApplicationState *, const gchar *);
};

/* application.c */
ApplicationState * application_init (int * argc, char *** argv);

#endif
