/* @author: John `jb Bellone <jvb4@njit.edu> */
#ifndef H_CELL
#define H_CELL

#include <shared.h>
#include <gtkextra/gtkextra.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct _Cell Cell;
  typedef struct _CellAttributes CellAttributes;

  #include "sheet.h"

  /*
    @description: These objects will evolve as more uses are found for it. 
    Right now it provides as an intermediate abstraction for the cell 
    information of a GtkSheetEntry. It works in conjunction with the Sheet 
    object (the Sheet object performs all the gtk_sheet_* functions). 

    If you decide to manually set any of the members be sure:
    a. For single cell changes row and column ranges must equal each other
       because GtkSheet does not provide interfaces for all operations to
       be done on single cell.
    b. Keep in mind that this object was built with the ability to dump a whole
       array of changes into a Sheet with a single call. This was built for 
       simple brute forcing of cell additions (and changes).
    c. If you find yourself adding a lot of code to this object then it may
       be time to add a new object to libworkbook. It was meant to be simple.

    If you decide to use gtk_sheet_* calls:
    a. Do not add them to any methods inside of this object. This object does
       not (and should not) contain any locking procedures. All the locking for
       threading should be done in the Sheet object (where all gtk+ calls 
       should be performed). 
  */
  struct _CellAttributes
  {
    GString * bgcolor;
    GString * fgcolor;
    GtkJustification justification;
  };

  struct _Cell
  {
    /* Members */
    Sheet * sheet;
    Cell * next;
    GString * value;
    CellAttributes attributes;
    GtkSheetRange range;
    gint row, column;
    
    /* Methods */
    void (*set) (Cell * cell, gint row, gint column, const gchar * value);
    void (*set_value) (Cell * cell, const gchar * value);  
    void (*set_column) (Cell * cell, gint column);
    void (*set_row) (Cell * cell, gint row);
    void (*set_range) (Cell * cell, const GtkSheetRange * range);
    void (*set_justification) (Cell * cell, GtkJustification justification);
    void (*set_fgcolor) (Cell * cell, const gchar * color);
    void (*set_bgcolor) (Cell * cell, const gchar * color);
    void (*set_attributes) (Cell * cell, const CellAttributes * attrib);
    void (*destroy) (Cell * cell);
  };

  /* cell.c */
  Cell *cell_new (void);

#ifdef __cplusplus
}
#endif
#endif /*H_CELL*/
