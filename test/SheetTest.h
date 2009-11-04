#ifndef SHEET_TEST_H
#define SHEET_TEST_H

// The testing fixture for libgtkworkbook Sheet object.

#include "WorkbookTest.h"

class SheetTest : public WorkbookTest {
protected:
	Sheet * sheet;
public:
	virtual void SetUp (void) {
		WorkbookTest::SetUp();
		sheet = workbook->add_new_sheet (workbook, "one", 5, 5);
	}

	virtual void TearDown (void) {
		sheet->destroy (sheet);
		WorkbookTest::TearDown();
	}
};

#endif
