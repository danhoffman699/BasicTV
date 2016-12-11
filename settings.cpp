#include "main.h"
#include "settings.h"
#include "file.h"
#include "util.h"

static std::vector<std::pair<std::string, std::string> > settings_vector;

/*
  TODO: allow appending settings to a settings file
 */

void settings::set_setting(std::string a, std::string b){
	settings_vector.push_back(std::make_pair(a, b));
}

void settings::set_settings(std::string settings_file){
	std::string cfg_file;
	try{
		cfg_file = file::read_file(settings_file);
	}catch(...){
		print("cannot set settings file", P_WARN);
		// isn't distributed in the GitHub
		return;
	}
	std::stringstream ss(cfg_file);
	std::string temp_str;
	char setting[512];
	char var[512];
	while(std::getline(ss, temp_str)){
		if(temp_str.size() == 0){
			continue;
		}
		if(temp_str[0] == '#' || temp_str[0] == '\n'){
			continue;
		}
		memset(setting, 0, 512);
		memset(var, 0, 512);
		const int res = std::sscanf(temp_str.c_str(), "%s = %s",
					    setting, var);
		if(res == EOF){
			print("setting '" + temp_str + "' is corrupt", P_ERR);
			continue; // no variable or otherwise indecipherable
		}
		char var_print[1024];
		bool hide_values = false;
		try{
			const std::string hide_values_str =
				get_setting("hide_values");
			hide_values = hide_values_str == "true";
		}catch(...){
			hide_values = false;
		}
		if(hide_values){
			memset(var_print, 0, 1024);
			memset(var_print, '*', strlen(var));
		}else{
			memcpy(var_print, var, 512);
		}
		print("setting " + (std::string)setting + " == " + var_print,
		      P_DEBUG);
		if(memcmp(setting, "import", 6) == 0){
			print("importing external file", P_NOTICE);
			set_settings(var);
		}
		settings_vector.push_back(
			std::make_pair(setting, var));
	}
	for(uint32_t i = 1;i < argc-1;i++){
		const uint32_t argv_len =
			strlen(argv[i]);
		if(argv_len > 2 && argv[i][0] == '-' && argv[i][1] == '-'){
			const std::string curr_setting =
				std::string(
					&argv[i][2],
					argv_len-2);
			const std::string value =
				argv[i+1];
			print("appending setting from argv: " + curr_setting + " == " + value, P_SPAM);
			settings_vector.push_back(
				std::make_pair(
					curr_setting,
					value));
			i++; // skip over the value
		}
	}
}

// no real way to know the type, so leave that to the parent
// no way to tell a blank string from no setting, seems like good
// default behavior, but code should be secure enough to allow for
// a thrown exception, most values are stored as integers anyways

std::string settings::get_setting(std::string setting){
	std::string retval;
	bool found = false;
	for(unsigned int i = 0;i < settings_vector.size();i++){
		if(settings_vector[i].first == setting){
			retval = settings_vector[i].second;
			found = true;
			// don't break, later values can
			// override previous values
		}
	}
	if(!found){
		throw std::runtime_error("setting " + setting + " not found");
	}
	return retval;
}

void settings::set_default_setting(std::string setting, std::string value){
	for(uint64_t i = 0;i < settings_vector.size();i++){
		if(settings_vector[i].first == setting){
			return;
		}
	}
	settings_vector.push_back(std::make_pair(setting, value));
}

void settings_init(){
	settings::set_settings("settings.cfg");
}
