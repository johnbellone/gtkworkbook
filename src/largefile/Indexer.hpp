#ifndef HPP_INDEXER
#define HPP_INDEXER

#include <proactor/InputDispatcher.hpp>
#include <proactor/Worker.hpp>
#include <cstdio>

namespace largefile {

  class Indexer : public proactor::Worker {
  private:
    FILE * fp;
  public:
    Indexer (proactor::InputDispatcher *d, FILE * fp);
    virtual ~Indexer (void);

    void * run (void * index);
  };

} // end of namespace

#endif
