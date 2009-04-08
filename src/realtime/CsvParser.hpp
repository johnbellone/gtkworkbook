#ifndef HPP_CSVPARSER
#define HPP_CSVPARSER

#include "proactor/Job.hpp"
#include <workbook/workbook.h>
#include <workbook/sheet.h>
#include <workbook/cell.h>
#include <csv/csv.h>
#include <iostream>

class CsvParser : public proactor::Job {
private:
  Workbook * wb;
  FILE * pktlog;
  Cell * cell;
public:
  CsvParser (Workbook * wb, FILE * pktlog) {
    this->wb = wb;
    this->pktlog = pktlog;
    this->cell = cell_new();
  }

  virtual ~CsvParser (void) {
    this->cell->destroy (cell);
  }

  void * run (void * null);
};

#endif
