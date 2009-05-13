#include "FileDispatcher.hpp"
#include <concurrent/SharedMemoryLock.hpp>

namespace largefile {

  FileDispatcher::FileDispatcher (int e, proactor::Proactor * pro) {
	this->pro = pro;
	setEventId(e);
  }

  FileDispatcher::~FileDispatcher (void) {
  }

  

  bool
  FileDispatcher::open (const std::string & filename) {
	if (filename.length() == 0)
	  return false;

	FILE * fp = NULL;
	
	if ((fp = fopen (filename.c_str(), "r")) == NULL) {
	  // stub: Throw an error somewhere.
	  return false;
	}

	concurrent::SharedMemoryLock::addMemoryLock ((unsigned long int)&fp);
	return true;
  }

  bool 
  FileDispatcher::close (void) {
	if (filename.length() == 0)
	  return false;
	
	FILE * fp = it->second;
	concurrent::SharedMemoryLock lock ((unsigned long int)&fp, true);
	  
	fclose (fp);
	fp = NULL;

	lock.remove();
	return true;
  }

} // end of namespace
