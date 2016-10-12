#ifndef FILE_H
#define FILE_H
#include "fstream"
#include "exception"
#include "stdexcept"
#include "sys/types.h"
#include "sys/stat.h"
#include "unistd.h"
namespace file{
	void write_file(std::string file, std::string data);
	std::string read_file(std::string file);
	bool exists(std::string file);
	void write(std::string file, std::string data);
	std::string read(std::string file);
	void wait_for_file(std::string file);
}
#endif
