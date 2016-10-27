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
	void set_setting(std::string a, std::string b);
	std::string get_setting(std::string);
	void set_default_setting(std::string setting, std::string value);
}
extern void settings_init();
#endif
