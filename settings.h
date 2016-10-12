#ifndef SETTINGS_H
#define SETTINGS_H
#include "vector"
#include "utility"
#include "string"
#include "mutex"
#include "sstream"
#include "cstdio"
namespace settings{
	void set_settings(std::string settings_file = "settings.cfg");
	std::string get_setting(std::string);
}
#endif
