#include "system.h"
#include "file.h"
#include "util.h"

int system_handler::run(std::string str){
	str += ";touch finished 2>&1 /dev/null";
	/*
	  Most commands need some time to be processed on the lower level (GPIO).
	  Speed shouldn't be a problem
	*/
	int retval = system(str.c_str());
	file::wait_for_file("finished");
	rm("finished");
	return retval;
}

void system_handler::write(std::string cmd, std::string file){
	run(cmd + " | tee " + file);
	file::wait_for_file(file);
}

void system_handler::mkdir(std::string dir){
	run("mkdir -p " + dir);
}

std::string system_handler::cmd_output(std::string cmd){
	
	write(cmd, "TMP_OUT");
	const std::string file_data = file::read_file("TMP_OUT");
	rm("TMP_OUT");
	return file_data;
}

void system_handler::rm(std::string file){
	system(("rm -r " + file).c_str());
}

/*
  TODO: when I implement this for Windows, get a general purpose search function
  for std::vector<std::string> that functions like grep
 */

std::vector<std::string> system_handler::find(std::string directory, std::string search){
	// no need for anything more advanced right now
	std::string raw_cmd = cmd_output("find " + directory + " | grep " + search);
	std::vector<std::string> retval =
		newline_to_vector(
			cmd_output("find " + directory + " | grep " + search));
	if(retval.size() > 1){
		retval.erase(retval.begin()); // directory itself
	}
	return retval;
}

std::vector<std::string> system_handler::find_all_files(std::string directory, std::string search){
	std::vector<std::string> retval =
		system_handler::find(directory, search);
	for(uint64_t i = 0;i < retval.size();i++){
		if(!file::is_file(retval[i])){
			retval.erase(
				retval.begin()+i);
			i--;
		}
	}
	return retval;
}
