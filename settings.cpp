#include "main.h"
#include "settings.h"
#include "file.h"
#include "util.h"
#include "lock.h"

static std::vector<std::pair<std::string, std::string> > settings_vector;
static lock_t settings_lock;

void settings::set_settings(std::string settings_file){
	std::string cfg_file = file::read_file(settings_file);
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
		LOCK_RUN(settings_lock, [](char *setting, char *var){
				settings_vector.push_back(
					std::make_pair(setting, var));
			}(setting, var));
	}
}

// no real way to know the type, so leave that to the parent
// no way to tell a blank string from no setting, seems like good
// default behavior, but code should be secure enough to allow for
// a thrown exception, most values are stored as integers anyways

std::string settings::get_setting(std::string setting){
	std::string retval;
	bool found = false;
	print("requesting setting " + setting, P_SPAM);
	LOCK_RUN(settings_lock, [&](){
			for(unsigned int i = 0;i < settings_vector.size();i++){
				if(settings_vector[i].first == setting){
					retval = settings_vector[i].second;
					found = true;
					// don't break, later values can
					// override previous values
				}
			}
		}());
	if(!found){
		throw std::runtime_error("setting " + setting + " not found");
	}
	return retval;
}
