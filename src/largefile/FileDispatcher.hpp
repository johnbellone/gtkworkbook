#ifndef HPP_FILEDISPATCHER
#define HPP_FILEDISPATCHER

#include <proactor/InputDispatcher.hpp>
#include <proactor/Proactor.hpp>
#include <string>

namespace largefile {

  class FileDispatcher : public proactor::InputDispatcher {
  private:
	FILE * fp;
  public:
	FileDispatcher (int e, proactor::Proactor * pro);
	virtual ~FileDispatcher (void);

	bool open (const std::string & filename);
	bool close (void);
  };

} // end of namespace

#endif
