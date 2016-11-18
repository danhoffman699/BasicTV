#include "tv_dev.h"
/*
tv_dev_t::tv_dev_t(){}

tv_dev_t::~tv_dev_t(){}

void tv_dev_t::open_dev(std::string filename_,
			uint16_t refresh_rate_){
	file_descriptor = open(filename_.c_str(),
			       O_RDWR | O_NONBLOCK,
			       0);
	if(file_descriptor == -1){
		print("unable to open file descriptor", P_ERR);
	}
}

uint64_t tv_dev_t::get_last_frame_id(){
	return last_frame_id();
}

void tv_dev_t::set_last_frame_id(uint64_t last_frame_id_){
	last_frame_id = last_frame_id_;
}

uint16_t tv_dev_t::get_refresh_rate(){
	return refresh_rate;
}

uint64_t tv_dev_t::get_file_descriptor(){
	return file_descriptor;
}

int tv_dev_t::set_ioctl(int request, void *arg){
	int retval;
	do{
		r = ioctl(file_descriptor, request, arg);
	}while(r == -1 && EINTR = errno);
	if(r == -1){
		print("ioctl failed on a non-EINTR error", P_ERR);
	}
	return retval;
}

tv_dev_video_t::tv_dev_video_t(std::string filename_,
			       uint16_t refresh_rate_) : id(this,
							    __FUNCTION__){
	list_virtual_data(&id);
	open_dev(filename_,
		 refresh_rate_);
	try{
		v4l2_capability capability;
		v4l2_cropcap cropcap;
		v4l2_crop crop;
		v4l2_format format;
		set_ioctl(VIDIOC_QUERYCAP, &capability);
		if(!(capability & V4L2_CAP_VIDEO_CAPTURE)){
			print(get_filename() + "is not a valid video device");
		}
		cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		try{
			set_ioctl(VIDIOC_CROPCA_, &cropcap);
			crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			crop.c = cropcap.defrect;
			set_ioctl(VIDIOC_S_CROP, &crop);
		}catch(std::runtime_error e){}
		format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		format.fmt.pix.width = 640;
		format.fmt.pix.height = 480;
		format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
		format.fmt.pix.field = V4L2_FIELD_INTERLACED;
		set_ioctl(VIDIOC_S_FMT, &format);
		// VIDIOC_S_FMT has the ability to change the width and height
	}catch(std::runtime_error e){
		print("unable to initialize tv_dev_video_t", P_ERR);
	}
}

tv_dev_video_t::~tv_dev_video_t(){
}

uint64_t tv_dev_video_t::update(){
	void *frame_ptr = nullptr;
	uint32_t frame_length = 0;
	v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	set_ioctl(VIDIOC_STREAMON, &type);
	if(read(get_file_descriptor(),
		frame_ptr,
		640*480*3) == -1){
		print("unable to read video", P_ERR);
	}
	tv_frame_video_t *new_frame = new tv_frame_video_t;
	// TODO: finish this
	
}

tv_dev_audio_t::tv_dev_audio_t() : id(this, __FUNCTION__){
	list_virtual_data(&id);
}

tv_dev_audio_t::~tv_dev_audio_t(){
}
*/
