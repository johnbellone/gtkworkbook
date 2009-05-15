#include "FileDispatcher.hpp"
#include <concurrent/ScopedMemoryLock.hpp>

namespace largefile {

  FileDispatcher::FileDispatcher (int e, proactor::Proactor * pro) {
	this->pro = pro;
	this->fp = NULL;
	setEventId(e);
  }

  FileDispatcher::~FileDispatcher (void) {
  }

  bool
  FileDispatcher::open (const std::string & filename) {
	if (filename.length() == 0)
	  return false;
	
	if ((this->fp = fopen (filename.c_str(), "r")) == NULL) {
	  // stub: Throw an error somewhere.
	  return false;
	}

	concurrent::ScopedMemoryLock::addMemoryLock ((unsigned long int)this->fp);
	return true;
  }

  bool 
  FileDispatcher::close (void) {
	concurrent::ScopedMemoryLock lock ((unsigned long int)this->fp, true);
	  
	fclose (this->fp);
	this->fp = NULL;

	lock.remove();
	return true;
  }

} // end of namespace
