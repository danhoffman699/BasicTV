#include "tv_dev.h"

tv_dev_t::tv_dev_t(){}

tv_dev_t::~tv_dev_t(){
	close(file_descriptor);
}

void tv_dev_t::list_virtual_data(data_id_t *id){
	id->add_data(&last_frame_id, sizeof(last_frame_id));
}

void tv_dev_t::open_dev(std::string filename_){
	file_descriptor = open(filename_.c_str(),
			       O_RDWR | O_NONBLOCK,
			       0);
	if(file_descriptor == -1){
		print("unable to open file descriptor:" + std::to_string(errno), P_ERR);
	}
}

id_t_ tv_dev_t::get_last_frame_id(){
	return last_frame_id;
}

void tv_dev_t::set_last_frame_id(id_t_ last_frame_id_){
	last_frame_id = last_frame_id_;
}


uint64_t tv_dev_t::get_file_descriptor(){
	return file_descriptor;
}

int tv_dev_t::set_ioctl(int request, void *arg){
	if(unlikely(file_descriptor <= 0)){
		print("invalid file descriptor", P_SPAM);
	}
	int retval;
ioctl_start:
	retval = ioctl(file_descriptor, request, arg);

	if(unlikely(retval == -1)){
		if(errno == EAGAIN || errno == EINTR){
			goto ioctl_start;
		}
		print("ioctl (" + convert::number::to_binary(request) + ") failed on a non-fixable error:" + std::to_string(errno), P_ERR);
	}else{
		print("ioctl (" + convert::number::to_binary(request) + ") succeeded", P_SPAM);
	}
	return retval;
}

