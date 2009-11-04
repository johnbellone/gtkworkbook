/* 
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
#include <libgtkworkbook/workbook.h>
#include <glib/gthread.h>
#include <string.h>

/* workbook.c (static) */
static void workbook_method_destroy (Workbook *);
static Workbook *workbook_object_init (GtkWidget *, const gchar *);
static Workbook *workbook_object_free (Workbook *);
static Sheet *workbook_method_addnewsheet (Workbook *, 
														 const gchar *, 
														 gint, gint);
static Sheet *workbook_method_get_sheet (Workbook *, const gchar *);
static void workbook_method_remove_sheet (Workbook *, Sheet *);
static gboolean workbook_method_move_sheet_index (Workbook *, Sheet *, gint);
static gboolean workbook_method_move_sheet (Workbook *, 
														  Sheet *,
														  const gchar *,
														  gboolean);

/* @description: This method "opens" a Workbook. In the future it will load
   a Workbook from a specified filename (which will most likely be a GZIP
   file) but for now it merely loads one into memory.
   @window: A pointer to the GtkWindow object. 
   @filename: A pointer to the filename string. */
Workbook *
workbook_open (GtkWidget * window, const gchar * filename)
{
	ASSERT (window != NULL);

	/* STUB: A workbook is opened here from some form of an archived file.
		At this point we would load up the configuration files for the styles,
		sheets and plugins to be loaded then this would all be executed here. */

	Workbook * book = workbook_object_init (window, filename);
	
	return book;
}

/* @description: This method adds a new Sheet to the Workbook object. The
   specified parameters are passed on to the Sheet object's constructor.
   But first we check to make sure that a sheet with the same label does
   not already exist inside of the Workbook.
   @book: Pointer to the Workbook object.
   @label: The Sheet's label string.
   @rows: Number of rows the GtkSheet object should have.
   @cols: Number of columns the GtkSheet object should have.*/
static Sheet *
workbook_method_addnewsheet (Workbook * book,
									  const gchar * label, 
									  gint rows, 
									  gint cols)
{
	ASSERT (book != NULL);

	Sheet * sheet = book->get_sheet (book, label);
	if (sheet != NULL) 
	{
      g_warning ("Cannot create '%s' because it already exists", 
					  label);
      return NULL;
	}

	sheet = sheet_new (book, label, rows, cols);

	LINK_OBJECT (book->sheet_first, book->sheet_last, sheet);

	if (IS_NULL (book->focus_sheet)) {
		sheet->has_focus = TRUE;
		book->focus_sheet = sheet;
	}

	return sheet;
}

/* @description: This method performs a search of all the Workbooks and
   returns a pointer to the Sheet object if it matches the provided label.
   @wb: Pointer to the workbook we're searching.
   @sheet: String label of the sheet we are searching for. */
static Sheet *
workbook_method_get_sheet (Workbook * wb, const gchar * sheet)
{
	ASSERT (wb != NULL);

	if (!IS_NULLSTR (sheet))
	{
      ITERATE_BEGIN (Sheet, wb->sheet_first);
      {
			if (!sheet) return NULL;
			if (!strcmp (sheet, it->name))
				return it;
      }
      ITERATE_END ();
	}

	return NULL;
}

/* @description: This method removes a Sheet object from the Workbook. It also
   removes to GtkSheet tab from the GtkNotebook (held in the Workbook).
   @wb: The Workbook pointer we are removing the object from.
   @sheet: The Sheet object that we will be removing. */
static void
workbook_method_remove_sheet (Workbook * wb, Sheet * sheet)
{
	ASSERT (wb != NULL);

	if (sheet->workbook != wb)
	{
      g_warning ("'%s' does not belong to workbook '%s'",
					  sheet->name, wb->filename);
      return;
	}
  
	ITERATE_BEGIN (Sheet, wb->sheet_first);
	{
		/* Remove the sheet from the GtkNotebook */
		if (it == sheet) {
			if (wb->focus_sheet == sheet) {
				wb->focus_sheet = sheet->prev;
			}

			DOUBLE_UNLINK (wb->sheet_first, wb->sheet_last, sheet);
			
			gint page = gtk_notebook_page_num (GTK_NOTEBOOK (wb->gtk_notebook),
														  sheet->gtk_box);
			gtk_notebook_remove_page (GTK_NOTEBOOK (wb->gtk_notebook), page); 
			gtk_widget_queue_draw (wb->gtk_notebook);
			return;
      }
	}
	ITERATE_END ();

	g_warning ("Sheet '%s' was not found inside of workbook '%s'", 
				  sheet->name, wb->filename);
}

/* @description: This method moves the Sheet's GtkSheet tab inside of the
   Workbook's GtkNotebook.
   @wb: The Workbook object that contains the GtkNotebook.
   @sheet: The Sheet object that contains the GtkSheet we're moving.
   @index: The _new_ page index. */
static gboolean
workbook_method_move_sheet_index (Workbook * wb, Sheet * sheet, gint index)
{
	gtk_notebook_reorder_child (GTK_NOTEBOOK (wb->gtk_notebook),
										 sheet->gtk_box,
										 index);
	return TRUE;
}

static gboolean
workbook_method_move_sheet (Workbook * wb, 
									 Sheet * sheet, 
									 const gchar * id,
									 gboolean after)
{
	ASSERT (wb != NULL);
	ASSERT (sheet != NULL);

	if (IS_NULLSTR (id))
		return FALSE;

	Sheet * sh = wb->get_sheet (wb, id);
	if (IS_NULL (sh))
	{
      g_warning ("Sheet '%s' does not exist in workbook '%s'", 
					  sheet->name, wb->filename);
      return FALSE;
	}
	
	gint page = gtk_notebook_page_num (GTK_NOTEBOOK (wb->gtk_notebook),
												  sh->gtk_box);
	if (page == -1)
	{
      g_warning ("Sheet '%s' does not appear to be in workbook '%s' notebook",
					  sheet->name, wb->filename);
      return FALSE;
	}

	if (after == TRUE)  page++;
	else                page--;
	
	gtk_notebook_reorder_child (GTK_NOTEBOOK (wb->gtk_notebook),
										 sheet->gtk_box,
										 page);
	return TRUE;
}

/* @description: This is a Workbook object's constructor. 
   @window: A pointer to the GtkWindow context.
   @filename: A pointer to the filename string. */
static Workbook *
workbook_object_init (GtkWidget * window, const gchar * filename)
{
	Workbook * book = NEW (Workbook);

	/* Set up the signals. */
	book->signals[SIG_WORKBOOK_CHANGED] = NULL;
	
	/* Set up the notebook */

	book->gtk_notebook = gtk_notebook_new ();
  
	GtkNotebook * notebook = GTK_NOTEBOOK (book->gtk_notebook);

	gtk_notebook_set_tab_pos (notebook, GTK_POS_BOTTOM);
	gtk_notebook_popup_enable (notebook);
	gtk_notebook_set_show_tabs (notebook, TRUE);
	gtk_notebook_set_show_border (notebook, TRUE);
	/*gtk_widget_set_usize (book->gtk_notebook, 1024, 768);*/

	gtk_widget_show_all (book->gtk_notebook);

	/* Members */
	book->sheet_first = book->sheet_last = NULL;
	book->next = book->prev = NULL;
	book->focus_sheet = NULL;
	book->gtk_window = window;
	book->filename = g_strdup (filename);
    
	/* Methods */
	book->destroy = workbook_method_destroy;
	book->add_new_sheet = workbook_method_addnewsheet;
	book->get_sheet = workbook_method_get_sheet;
	book->remove_sheet = workbook_method_remove_sheet;
	book->move_sheet_index = workbook_method_move_sheet_index;
	book->move_sheet = workbook_method_move_sheet;

	return book;
}

/* @description: This method destroys the Workbook object and all of the
   Sheet objects that it contains. 
   @book: The Workbook object to destroy. */
static void
workbook_method_destroy (Workbook * book)
{
	ASSERT (book != NULL);

	/* Wrap anything up here. */
	Sheet * current = book->sheet_first, * next = NULL;
	while (current)
	{
      next = current->next;
      current->destroy (current);
      current = next;
	}

	if (book->next && book->prev) {
		book->prev->next = book->next;
		book->next->prev = book->prev;
	}

	book->next = book->prev = NULL;
	
	workbook_object_free (book);
}

/* @description: This method frees the memory that the Workbook object has 
   been using. This is only able to be called from book->destroy()
   @book: The Workbook object we are freeing. */
static Workbook *
workbook_object_free (Workbook * book) {
	ASSERT (book != NULL);

	book->sheet_first = book->sheet_last = NULL;

	FREE (book->filename);
	FREE (book);
	return book;
}
