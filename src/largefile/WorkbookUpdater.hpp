#ifndef HPP_WORKBOOKUPDATER
#define HPP_WORKBOOKUPDATER

#include <proactor/Worker.hpp>
#include <workbook/workbook.h>
#include <concurrent/Thread.hpp>

namespace largefile {

  class WorkbookUpdater : public proactor::Worker {
  private:
    Workbook * wb;
    FILE * log;
    int verbosity;
  public:
    WorkbookUpdater (Workbook * wb, FILE * log, int verbosity) {
      this->wb = wb;
      this->log = log;
      this->verbosity = verbosity;
    }

    virtual ~WorkbookUpdater (void) {

    }

    void * run (void * null) {
      this->running = true;
      
      while (this->running == true) {

	concurrent::Thread::sleep (100);
      }

      return NULL;
    }
  };

} // end of namespace

#endif
