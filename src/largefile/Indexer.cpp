#include "Indexer.hpp"
#include <concurrent/SharedMemoryLock.hpp>

namespace largefile {

  Indexer::Indexer (proactor::InputDispatcher * d, FILE * fp) {
    this->dispatcher = d;
    this->fp = fp;
  }

  Indexer::~Indexer (void) {

  }

  void *
  Indexer::run (void * null) {
    char buf[4096];
	this->running = true;

	concurrent::SharedMemoryLock mutex ((unsigned long int)this->fp, true);

	if (!feof (this->fp)) {

	  fseek (this->fp, this->startOffset, SEEK_SET);
	  
	  while (this->windowSize > 0) {
		if (fgets (buf, 4096, this->fp) == NULL)
		  break;

		index->push_back ( ftell (this->fp) );

		this->windowSize--;
	  }
	}
    return index;
  }

} // end of namespace
