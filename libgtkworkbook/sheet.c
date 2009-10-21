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
#include <libgtkworkbook/sheet.h>
#include <gtkextra/gtksheet.h>
#include <string.h>

/* sheet.c (static) */
static Sheet *sheet_object_init (Workbook *, const gchar *, gint, gint);
static void sheet_object_free (Sheet *);
static void sheet_method_destroy (Sheet *);
static void sheet_method_set_cell (Sheet *, gint, gint, const gchar *);
static void sheet_method_apply_cell (Sheet *, const Cell *);
static void sheet_method_apply_cellarray (Sheet *, Cell **, gint);
static void sheet_method_apply_cellrange (Sheet *, const GtkSheetRange *, const CellAttributes *);
static void sheet_method_range_set_background (Sheet *, const GtkSheetRange *, const gchar *);
static void sheet_method_range_set_foreground (Sheet *, const GtkSheetRange *, const gchar *);
static void sheet_method_set_attention (Sheet *, gint);
static gboolean sheet_method_load (Sheet *, const gchar *);
static gboolean sheet_method_save (Sheet *, const gchar *);
static void sheet_method_apply_cellrow (Sheet *, gint);
static void sheet_method_get_cellrow (Sheet *, gint, Cell **, gint);
static void sheet_method_set_cell_value_length (Sheet *,gint,gint,void *,size_t);
static void sheet_method_set_column_title (Sheet *, gint, const gchar *);
static void sheet_method_set_row_title (Sheet *, gint, const gchar *);
static void sheet_method_freeze_selection (Sheet *);
static void sheet_method_thaw_selection (Sheet *);

struct geometryFileHeader {
	gint fileVersion;
	gint maxRow;
	gint maxColumn;
};

struct geometryFileEntry {
	gint cellRow;
	gint cellCol;
	gint cellTextLength;
	gboolean cellIsVisible;
	gboolean cellIsEditable;
	GtkJustification cellJustification;
	GdkColor cellForeground;
	GdkColor cellBackground;
};

/* @description: This method creates a new Sheet object and returns the
   pointer to that object. It calls the constructor function to do so.
   @book: A pointer to the Workbook that the object will be a part of.
   @label: A string to the Sheet's label. 
   @rows: The number of rows.
   @columns: The number of columns. */
Sheet *
sheet_new (Workbook * book, const gchar * label, gint rows, gint columns)
{
	ASSERT (book != NULL);
	
	Sheet * sheet = sheet_object_init (book, label, rows, columns);

	/* STUB: Perform anything that is based on a style here. */

	return sheet;
}

/* @description: This function is the Sheet's constructor. 
   @book: A pointer to the Workbook that the Sheet object will be assigned.
   @label: A string label - the name of the sheet that we will use to search.
   @rows: The amount of rows the GtkSheet widget should have.
   @cols: The amount of columns the GtkSheet widget should have. */
static Sheet *
sheet_object_init (Workbook * book,
						 const gchar * label, 
						 gint rows, gint columns)
{
	Sheet * sheet = NEW (Sheet);

	/* Create the sheet containers and GtkSheet object. */
	sheet->gtk_box = gtk_vbox_new (FALSE, 1);

	sheet->gtk_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_box_pack_start (GTK_BOX (sheet->gtk_box), sheet->gtk_scrolledwindow, 1,1,1);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sheet->gtk_scrolledwindow),
											  GTK_POLICY_AUTOMATIC,
											  GTK_POLICY_AUTOMATIC);
	gtk_widget_show (sheet->gtk_scrolledwindow);

	sheet->gtk_label = gtk_label_new (label);
  
	sheet->gtk_sheet = gtk_sheet_new (rows, columns, label);
	gtk_sheet_set_autoresize (GTK_SHEET (sheet->gtk_sheet), TRUE);
	gtk_container_add (GTK_CONTAINER (sheet->gtk_scrolledwindow),
							 GTK_WIDGET (sheet->gtk_sheet));

	/* We should be able to use sheet->gtk_box now throughout all of our
		tests when iterating through a GtkNotebook structure. The page number
		will change when something is removed (or reordered). The pointer will
		stay the same. */
	sheet->page = gtk_notebook_append_page (GTK_NOTEBOOK (book->gtk_notebook),
														 sheet->gtk_box,
														 sheet->gtk_label);

	/* Members */
	sheet->workbook = book;
	sheet->name = g_strdup (label);
	sheet->attention = 0;
	sheet->notices = 0;
	sheet->has_focus = FALSE;
	sheet->next = sheet->prev = NULL;
	sheet->max_rows = rows;
	sheet->max_columns = columns;
	sheet->column_titles = row_new ( sheet->max_columns );
	sheet->row_titles = row_new ( sheet->max_rows );
	
	/* Allocation procedure for our cell buffer */
	sheet->cells = (Cell ***) g_malloc (rows * sizeof (Cell**));
	for (int ii = 0; ii < rows; ii++) {
		sheet->cells[ii] = (Cell **) g_malloc (columns * sizeof (Cell*));

		for (int jj = 0; jj < columns; jj++) {
			sheet->cells[ii][jj] = cell_new();
			sheet->cells[ii][jj]->row = ii;
			sheet->cells[ii][jj]->column = jj;
		}
	}
	
	/* Methods */
	sheet->destroy = sheet_method_destroy;
	sheet->set_cell = sheet_method_set_cell;
	sheet->set_cell_value_length = sheet_method_set_cell_value_length;
	sheet->apply_range = sheet_method_apply_cellrange;
	sheet->apply_array = sheet_method_apply_cellarray;
	sheet->apply_cell = sheet_method_apply_cell;
	sheet->apply_row = sheet_method_apply_cellrow;
	sheet->range_set_foreground = sheet_method_range_set_foreground;
	sheet->range_set_background = sheet_method_range_set_background;
	sheet->set_attention = sheet_method_set_attention;
	sheet->save = sheet_method_save;
	sheet->load = sheet_method_load;
	sheet->get_row = sheet_method_get_cellrow;
	sheet->set_column_title = sheet_method_set_column_title;
	sheet->set_row_title = sheet_method_set_row_title;
	sheet->freeze_selection = sheet_method_freeze_selection;
	sheet->thaw_selection = sheet_method_thaw_selection;
	
	/* Connect any signals that we need to. */
	if (!IS_NULL (sheet->workbook->signals[SIG_WORKBOOK_CHANGED]))
	{
		/*
      gtk_signal_connect (GTK_OBJECT (sheet->gtk_sheet), "changed",
								  G_CALLBACK (sheet->workbook->signals[SIG_WORKBOOK_CHANGED]), sheet);
		*/
	}

	gtk_widget_show_all (sheet->gtk_box);
	return sheet;
}

static gboolean
sheet_method_load (Sheet * sheet, const gchar * filepath)
{
	ASSERT (sheet != NULL);

	if (IS_NULLSTR (filepath)) {
      g_warning ("%s: filepath cannot be a NULL string", __FUNCTION__);
      return FALSE;
	}

	FILE * fp = NULL;
	if ((fp = fopen (filepath, "rb")) == NULL) {
      g_warning ("%s: failed opening file '%s' for reading", 
					  __FUNCTION__,
					  filepath);
      return FALSE;
	}
	
	GtkSheet * gtksheet = GTK_SHEET (sheet->gtk_sheet);
	struct geometryFileHeader header = {-1,-1,-1};
	struct geometryFileEntry entry = {-1,-1,-1};
 
	fread ((void *)&header, sizeof (struct geometryFileHeader), 1, fp);

	if (header.fileVersion != GEOMETRY_FILE_VERSION) {
      g_warning ("Geometry file version %d is not accepted. (%d)",
					  header.fileVersion, GEOMETRY_FILE_VERSION);
      FCLOSE (fp);
		return FALSE;
	}

	while (fread ((void *)&entry, sizeof (struct geometryFileEntry), 1, fp) > 0)
	{
      gchar * text = g_strndup ("", entry.cellTextLength);
      fread ((void *)text, sizeof (gchar), entry.cellTextLength, fp);
      gtk_sheet_set_cell_text (gtksheet, 
										 entry.cellRow, 
										 entry.cellCol, 
										 text);
      
      GtkSheetCell ** cell = &gtksheet->data[entry.cellRow][entry.cellCol];
      
      (*cell)->attributes->is_editable = entry.cellIsEditable;
      (*cell)->attributes->is_visible = entry.cellIsVisible;
      (*cell)->attributes->justification = entry.cellJustification;
      (*cell)->attributes->foreground.pixel = entry.cellForeground.pixel;
      (*cell)->attributes->foreground.red = entry.cellForeground.red;
      (*cell)->attributes->foreground.green = entry.cellForeground.green;
      (*cell)->attributes->foreground.blue = entry.cellForeground.blue;
      (*cell)->attributes->background.pixel = entry.cellBackground.pixel;
      (*cell)->attributes->background.red = entry.cellBackground.red;
      (*cell)->attributes->background.green = entry.cellBackground.green;
      (*cell)->attributes->background.blue = entry.cellBackground.blue;

      FREE (text);
	}

	FCLOSE (fp);
	return TRUE;
}



static gboolean
sheet_method_save (Sheet * sheet, const gchar * filepath) {
	ASSERT (sheet != NULL);

	if (IS_NULLSTR (filepath)) {
      g_warning ("%s: filepath cannot be a NULL string", __FUNCTION__);
      return FALSE;
	}

	FILE * fp = NULL;
	if ((fp = fopen (filepath, "wb")) == NULL) {
      g_warning ("%s: failed opening file '%s' for writing", 
					  __FUNCTION__, 
					  filepath);
      return FALSE;
	}

	GtkSheetCell *** data = GTK_SHEET (sheet->gtk_sheet)->data;
	struct geometryFileHeader header = {
		GEOMETRY_FILE_VERSION,
		GTK_SHEET (sheet->gtk_sheet)->maxallocrow,
		GTK_SHEET (sheet->gtk_sheet)->maxalloccol
	};

	fwrite ((void *)&header, sizeof(struct geometryFileHeader), 1, fp);

	for (gint ii = 0; ii <= header.maxRow; ii++) {
      for (gint jj = 0; jj <= header.maxColumn; jj++) {
			GtkSheetCell * cell = data[ii][jj];

			if (!IS_NULL (cell) && !IS_NULLSTR(cell->text)) {
				struct geometryFileEntry entry = {
					cell->row,
					cell->col,
					strlen (cell->text),
					cell->attributes->is_visible,
					cell->attributes->is_editable,
					cell->attributes->justification
				};

				entry.cellForeground.pixel = cell->attributes->foreground.pixel;
				entry.cellForeground.red = cell->attributes->foreground.red;
				entry.cellForeground.green = cell->attributes->foreground.green;
				entry.cellForeground.blue = cell->attributes->foreground.blue;
				entry.cellBackground.pixel = cell->attributes->background.pixel;
				entry.cellBackground.red = cell->attributes->background.red;
				entry.cellBackground.green = cell->attributes->background.green;
				entry.cellBackground.blue = cell->attributes->background.blue;

				fwrite ((void *)&entry, 
						  sizeof (struct geometryFileEntry), 1, fp);
				fwrite ((void *)cell->text, 
						  sizeof (gchar), entry.cellTextLength, fp);
			}
		}
	}

	FCLOSE (fp);
	return TRUE;
}

/* @description: This method sets the attention level of the Sheet.
   @sheet: A pointer to the Sheet object.
   @attention: The attention level. */
static void 
sheet_method_set_attention (Sheet * sheet, gint attention) {
	ASSERT (sheet != NULL);

	sheet->attention = attention;

	/* Do something funky to show that you should be looking at ME!
		Oh, GtkNotebook tab, why are thou so vain? */
	if ((sheet->has_focus == FALSE) && (sheet->notices > 0)) {
      
	}
}

/* @description: This method destroys the Sheet object.
   @sheet: A pointer to the object that will be destroyed. */
static void
sheet_method_destroy (Sheet * sheet) {
	ASSERT (sheet != NULL);

	DOUBLE_UNLINK (sheet);

	sheet_object_free (sheet);
}

static void
sheet_method_freeze_selection (Sheet * sheet) {
	ASSERT (sheet != NULL);
	GtkSheet * gtksheet = GTK_SHEET (sheet->gtk_sheet);

	sheet->range_set_background (sheet, &gtksheet->range, "#eeeeee");

	for (int ii = gtksheet->range.row0; ii < gtksheet->range.rowi; ii++) {
		register int jj = gtksheet->range.col0;

		do {
			Cell * cell = sheet->cells[ii][jj++];
			cell->attributes.editable = FALSE;
		} while (jj < gtksheet->range.coli);
	}
}

static void
sheet_method_thaw_selection (Sheet * sheet) {
	ASSERT (sheet != NULL);
	GtkSheet * gtksheet = GTK_SHEET (sheet->gtk_sheet);

	sheet->range_set_background (sheet, &gtksheet->range, "#ffffff");

	for (int ii = gtksheet->range.row0; ii < gtksheet->range.rowi; ii++) {
		register int jj = gtksheet->range.col0;

		do {
			Cell * cell = sheet->cells[ii][jj++];
			cell->attributes.editable = TRUE;
		} while (jj < gtksheet->range.coli);
	}
}

/* @description: This method frees the memory that was used by the Sheet
   object. This should only be called from sheet->destroy()
   @sheet: A pointer to the Sheet object that will be freed. */
static void
sheet_object_free (Sheet * sheet) {
	ASSERT (sheet != NULL);

	for (int ii = 0; ii < sheet->max_rows; ii++) {
		for (int jj = 0; jj < sheet->max_columns; jj++) {
			sheet->cells[ii][jj]->destroy (sheet->cells[ii][jj]);
		}
		FREE (sheet->cells[ii]);
	}
	
	FREE (sheet->cells);
	FREE (sheet->name);
	FREE (sheet->row_titles);
	FREE (sheet->column_titles);
	FREE (sheet);
	return;
}

static void
sheet_method_apply_cellrange (Sheet * sheet, 
										const GtkSheetRange * range,
										const CellAttributes * attrib) {
	ASSERT (sheet != NULL);
	g_return_if_fail (range != NULL);
	g_return_if_fail (attrib != NULL);
}

static void
sheet_method_set_column_title (Sheet * sheet, gint column, const gchar * title) {
	ASSERT (sheet != NULL);
	g_return_if_fail (title != NULL);
	
	gtk_sheet_column_button_add_label ( GTK_SHEET (sheet->gtk_sheet), column, title);
	g_string_assign (sheet->column_titles->cells[column]->value, title);
}

static void
sheet_method_set_row_title (Sheet * sheet, gint row, const gchar * title) {
	ASSERT (sheet != NULL);
	g_return_if_fail (title != NULL);
		
	gtk_sheet_row_button_add_label ( GTK_SHEET (sheet->gtk_sheet), row, title);
	g_string_assign (sheet->row_titles->cells[row]->value, title);
}

static void
sheet_method_apply_cellrow (Sheet * sheet, gint row) {
	ASSERT (sheet != NULL);
	GtkSheet * gtksheet = GTK_SHEET (sheet->gtk_sheet);

	for (int jj = 0; jj < sheet->max_columns; jj++) {
		Cell * cell = sheet->cells[row][jj];
		
		if (cell->attributes.editable == TRUE)
			gtk_sheet_set_cell_text (gtksheet, row, jj, cell->value->str);
	}
}

static void
sheet_method_get_cellrow (Sheet * sheet,
								  gint row,
								  Cell ** array,
								  gint size) {
	ASSERT (sheet != NULL);
	ASSERT (array != NULL);

	GtkSheet * gtksheet = GTK_SHEET (sheet->gtk_sheet);
	
	if (size > gtksheet->maxcol)
		size = gtksheet->maxcol;

	for (int ii = 0; ii < size; ii++) {
		Cell * p = array[ii];
		const char * label = gtk_sheet_cell_get_text (gtksheet, row, ii);

		if (label == NULL)
			p->set (p, row, ii, "");
		else
			p->set (p, row, ii, label);
	}
}

static void
sheet_method_apply_cellarray (Sheet * sheet, 
										Cell ** array,
										gint size)
{
	ASSERT (sheet != NULL);
	g_return_if_fail (array != NULL);
	
	GtkSheet * gtksheet = GTK_SHEET (sheet->gtk_sheet);

	/* We'll see how this performs for now. In the future we may want to go
		directly into the GtkSheet structures to get a little more performance
		boost (mainly because we should not have to check all the bounds each
		time we want to update). */
	for (gint ii = 0; ii < size; ii++) {
		Cell * cell = array[ii];

		gtk_sheet_set_cell_text (gtksheet,
										 cell->row,
										 cell->column,
										 cell->value->str);

		if (!IS_NULLSTR (cell->attributes.bgcolor->str))
			sheet->range_set_background (sheet, 
												  &cell->range, 
												  cell->attributes.bgcolor->str);

		if (!IS_NULLSTR (cell->attributes.fgcolor->str))
			sheet->range_set_foreground (sheet, 
												  &cell->range, 
												  cell->attributes.fgcolor->str);

		cell->value->str[0] = cell->attributes.bgcolor->str[0] = cell->attributes.fgcolor->str[0] = 0;
	}
}

/* @description: This method applies the settings from a Cell object into the
   GtkSheet. In order to properly function this should be really the only the
   GtkSheet object is modified.
   @sheet: A pointer to the Sheet that holds the GtkSheet object.
   @cell: A pointer to the Cell that will be applied. */
static void
sheet_method_apply_cell (Sheet * sheet, const Cell * cell)
{
	ASSERT (sheet != NULL);
	g_return_if_fail (cell != NULL);

	if (sheet->has_focus == FALSE)
		sheet->notices++;

	gtk_sheet_set_cell (GTK_SHEET (sheet->gtk_sheet),
							  cell->row,
							  cell->column,
							  cell->attributes.justification,
							  cell->value->str);

	if (!IS_NULLSTR (cell->attributes.bgcolor->str))
		sheet->range_set_background (sheet, 
											  &cell->range, 
											  cell->attributes.bgcolor->str);

	if (!IS_NULLSTR (cell->attributes.fgcolor->str))
		sheet->range_set_foreground (sheet, 
											  &cell->range, 
											  cell->attributes.fgcolor->str);

	/* Clear all of the strings */
	/*g_string_assign (cell->value, "");
	g_string_assign (cell->attributes.bgcolor, "");
	g_string_assign (cell->attributes.fgcolor, "");*/
}

/* @description: This method changes the background of a range of cells. 
   @sheet: A pointer to the Sheet object that contains GtkSheet.
   @range: A pointer to the GtkSheetRange object that contains the ranges
   that we will be applying the background color to.
   @desc: A string that contains the color's string value (e.g. white, red
   green, etc). */
static void
sheet_method_range_set_background (Sheet * sheet, 
											  const GtkSheetRange * range,
											  const gchar * desc)
{
	ASSERT (sheet != NULL); ASSERT (range != NULL);
	GdkColor color;
	GdkColor black;

	/* The color needs to be taken from the colormap; there is an alternative
		way to do this if we use #rgb or #rrggbb formats. */
	gdk_color_parse (desc, &color);
	gdk_color_parse ("black", &black);
	gdk_color_alloc (gtk_widget_get_colormap (sheet->gtk_sheet), &color);
	gdk_color_alloc (gtk_widget_get_colormap (sheet->gtk_sheet), &black);
  
	gtk_sheet_range_set_background (GTK_SHEET (sheet->gtk_sheet), range, &color);
	gtk_sheet_range_set_border_color (GTK_SHEET (sheet->gtk_sheet), range, &black);
}

/* @description: This method changes the foreground color over a range of
   cells. 
   @sheet: A pointer to the Sheet object that contains GtkSheet.
   @range: A pointer to the GtkSheetRange object that contains the ranges
   that we will be applying the foreground color to.
   @desc: The string representation of the color (e.g. white, green, blue). */
static void
sheet_method_range_set_foreground (Sheet * sheet, 
											  const GtkSheetRange * range,
											  const gchar * desc)
{
	ASSERT (sheet != NULL); ASSERT (range != NULL);
	GdkColor color;
  
   /* The color needs to be taken from the colormap; there is an alternative
		way to do this if we use #rgb or #rrggbb formats. */
	gdk_color_parse (desc, &color);
	gdk_color_alloc (gtk_widget_get_colormap (sheet->gtk_sheet),
						  &color);
      
	gtk_sheet_range_set_foreground (GTK_SHEET (sheet->gtk_sheet),
											  range, &color);
}

static void
sheet_method_set_cell_value_length (Sheet * sheet,
												gint row,
												gint column,
												void * value,
												size_t length) {
	ASSERT (sheet != NULL);
	if (row < 0 || column < 0 || row > sheet->max_rows || column > sheet->max_columns) return;
	Cell * cell = sheet->cells[row][column];
	cell->set_value_length (cell, value, length);
}

/* @description: This method manually sets a GtkSheet cell's value. It does
   not require the use of the Cell object.
   @sheet: A pointer to the Sheet object that contains GtkSheet.
   @row: An integer value of the row.
   @col: An integer value of the column.
   @value: The text string to be applied to the cell. */
static void 
sheet_method_set_cell (Sheet * sheet,
							  gint row,
							  gint column,
							  const gchar * value)
{
	ASSERT (sheet != NULL);

	if (sheet->has_focus == FALSE)
		sheet->notices++;

	gtk_sheet_set_cell (GTK_SHEET (sheet->gtk_sheet), 
							  sheet->cells[row][column]->row, 
							  sheet->cells[row][column]->column, 
							  GTK_JUSTIFY_LEFT, 
							  value);
}
