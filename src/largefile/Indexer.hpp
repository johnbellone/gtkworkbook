#ifndef HPP_INDEXER
#define HPP_INDEXER

#include <proactor/InputDispatcher.hpp>
#include <proactor/Worker.hpp>
#include <cstdio>

namespace largefile {
  typedef std::vector<unsigned int> FileIndex;

  class Indexer : public proactor::Worker {
  private:
    FILE * fp;
	FileIndex * index;
	long int startOffset;
	long int windowSize;
  public:
    Indexer (proactor::InputDispatcher *d, 
			 FILE * fp, 
			 FileIndex * index, 
			 long int offset, 
			 long int window);
    virtual ~Indexer (void);

    void * run (void * null);
  };

} // end of namespace

#endif
