#ifndef HPP_LARGEFILE
#define HPP_LARGEFILE

#include <map>
#include <string>
#include "../Plugin.hpp"
#include "../Application.hpp"
#include "../config.h"

extern "C++" {

class Largefile : public Plugin {
private:
	typedef std::map<std::string,int> FilenameMap;
	
	FILE * pktlog;
	FilenameMap mapping;
public:
	Largefile (Application * appstate, Handle * platform);
	virtual ~Largefile (void);

	bool open_file (const std::string & filename);
	bool exit_file (const std::string & filename);
};

}
	
#endif
