#include "main.h"
#include "file.h"
#include "util.h"

void file::write_file(std::string file, std::string data){
	std::ofstream out(file, std::ofstream::trunc);
	if(out.is_open() == false){
		throw std::runtime_error((std::string)"Unable to open file " + file);
	}
	out << data;
	out.close();
}

std::string file::read_file(std::string file){
	std::string retval;
	if(exists(file) == false){
		retval = "";
	}else{
		std::ifstream in(file);
		if(in.is_open() == false){
			throw std::runtime_error((std::string)"Unable to open file " + file);
		}
		std::string buffer;
		while(getline(in, buffer)){
			retval += buffer + "\n";
		}
		in.close();
	}
	return retval;
}

bool file::exists(std::string file){
	std::string filename;
	if(filename.find_first_of("/") != std::string::npos){
		filename = file.substr(file.find_last_of("/"), file.size());
	}else{
		filename = file;
	}
	const int file_len = filename.size();
	if(file_len >= 250){
		print("file length is above 250, this is bad", P_ERR);
	}else if(file_len >= 127){
		print("file length is above 127, this shouldn't happen too much", P_WARN);
	}
	struct stat sb;
	switch(stat(file.c_str(), &sb)){
	case EACCES:
		print("search permission is denied for file " + file, P_ERR);
		return false;
	case ELOOP:
		print("too many symbolic links for file " + file, P_ERR);
		// should this count?
		return false;
	case ENAMETOOLONG:
		print("file path is too long for file " + file, P_ERR);
		return false;
	case ENOENT:
		print("directory doesn't exist or path is blank for file " + file, P_ERR);
		return false;
	case ENOTDIR:
		print("directory file mixup in the path for file " + file, P_ERR);
		return false;
	case EOVERFLOW:
		print("data of file is too large to be represented for file " + file, P_ERR);
		return true;
	default:
		print("file " + file + " exists", P_DEBUG);
		return true;
	}
}

void file::write(std::string file, std::string data){
	write_file(file, data);
}

std::string file::read(std::string file){
	return read_file(file);
}

void file::wait_for_file(std::string file){
	while(exists(file) == false){
		sleep_ms(1);
	}
}
