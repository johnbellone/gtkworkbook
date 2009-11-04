#include "WorkbookTest.h"

TEST_F (WorkbookTest, GtkWindowPtrIsNotNull) {
	ASSERT_TRUE (gtk_window != NULL);
}

TEST_F (WorkbookTest, WorkbookPtrIsNotNull) {
	ASSERT_TRUE (workbook != NULL);
}

TEST_F (WorkbookTest, SheetListIsEmpty) {
	EXPECT_EQ (NULL, workbook->sheet_first);
	EXPECT_EQ (NULL, workbook->sheet_last);
	EXPECT_EQ (NULL, workbook->focus_sheet);
}

TEST_F (WorkbookTest, MethodAddSheetWorks) {
	Sheet * sheet = workbook->add_new_sheet (workbook, "test", 1, 1);
	ASSERT_TRUE (sheet != NULL);

	EXPECT_EQ (1, sheet->max_rows);
	EXPECT_EQ (1, sheet->max_columns);
}

TEST_F (WorkbookTest, MethodGetSheetWorks) {
	workbook->add_new_sheet (workbook, "one", 1, 1);
	workbook->add_new_sheet (workbook, "two", 1, 1);
	workbook->add_new_sheet (workbook, "three", 1, 1);
	
	Sheet * a = workbook->add_new_sheet (workbook, "test", 1, 1);
	Sheet * b = workbook->get_sheet (workbook, "test");
	ASSERT_TRUE (a == b);
}

TEST_F (WorkbookTest, MethodRemoveSheetWorks) {
	Sheet * sheet = workbook->add_new_sheet (workbook, "test", 1, 1);
	workbook->remove_sheet (workbook, sheet);

	EXPECT_EQ (NULL, workbook->sheet_first);
	EXPECT_EQ (NULL, workbook->sheet_last);
	EXPECT_EQ (NULL, workbook->focus_sheet);
}



