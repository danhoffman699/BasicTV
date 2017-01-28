#ifndef SYSTEM_H
#define SYSTEM_H

#include "util.h"
#include "file.h"

namespace system_handler{
	std::string cmd_output(std::string cmd);
	int run(std::string str);
	// file operators
	void mkdir(std::string dir);
	void write(std::string cmd, std::string file);
	void rm(std::string file);
	std::vector<std::string> find(std::string directory, std::string search);
	std::vector<std::string> find_all_files(std::string directory, std::string search);
};

#endif
