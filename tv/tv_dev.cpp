#include "tv_dev.h"

tv_dev_t::tv_dev_t(){}

tv_dev_t::~tv_dev_t(){}

void tv_dev_t::list_virtual_data(data_id_t *id){
	// no need to include it
}

void tv_dev_t::open_dev(std::string filename_,
			uint16_t refresh_rate_){
	file_descriptor = open(filename_.c_str(),
			       O_RDWR | O_NONBLOCK,
			       0);
	if(file_descriptor == -1){
		print("unable to open file descriptor:" + std::to_string(errno), P_ERR);
	}
	refresh_rate = refresh_rate_;
}

uint64_t tv_dev_t::get_last_frame_id(){
	return last_frame_id;
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
	if(file_descriptor <= 0){
		print("invalid file descriptor", P_SPAM);
	}
	int retval;
	do{
		retval = ioctl(file_descriptor, request, arg);
	}while(unlikely(retval == -1 && (errno == EINTR || errno == EAGAIN)));
	if(retval == -1){
		print("ioctl (" + convert::number::to_binary(request) + ") failed on a non-EINTR error:" + std::to_string(errno), P_ERR);
	}else{
		print("ioctl (" + convert::number::to_binary(request) + ") succeeded", P_SPAM);
	}
	return retval;
}

/*
  For now, this will be the de-facto method of importing video into the
  program. Use V4L2VD to handle this (or a similar program)
 */

bool tv_dev_video_t::is_compatible(){
	v4l2_capability cap;
	CLEAR(cap);
	set_ioctl(VIDIOC_QUERYCAP, &cap);
	return (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) &&
		(cap.capabilities & V4L2_CAP_STREAMING);
}

void tv_dev_video_t::userp_init(uint64_t image_size){
	raw_pixel_size = image_size;
	raw_pixel_data = new uint8_t[raw_pixel_size];
	pixel_size = 640*480*3; // TODO: define with variables
	pixel_data = new uint8_t[pixel_size];
	try{
		v4l2_requestbuffers req;
		req.count = 1;
		req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory = V4L2_MEMORY_USERPTR;
		set_ioctl(VIDIOC_REQBUFS, &req);
	}catch(...){}
}

void tv_dev_video_t::standard_init(){
	try{
		v4l2_cropcap cropcap;
		CLEAR(cropcap);
		v4l2_crop crop;
		CLEAR(crop);
		cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		set_ioctl(VIDIOC_CROPCAP, &cropcap);
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect;
		set_ioctl(VIDIOC_S_CROP, &crop);
	}catch(std::exception e){
		//print("couldn't initialize cropping", P_WARN);
		// errors can be safely ignored (per the V4L2 sample)
	}
	// perhaps force the resolution?
	// ripped from an example from GitHub
	v4l2_format fmt;
	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 640;
	fmt.fmt.pix.height = 480;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
	fmt.fmt.pix.field = V4L2_FIELD_SEQ_TB;
	set_ioctl(VIDIOC_S_FMT, &fmt);
	// RGB24 isn't too common for webcams, so I need a
	// lightweight function to convert this to RGB
	if(fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_RGB24){
		print("pixel format has changed", P_WARN);
	}
	// TODO: reflect possible size changes
	uint64_t min = fmt.fmt.pix.width * 2;
        if (fmt.fmt.pix.bytesperline < min){
                fmt.fmt.pix.bytesperline = min;
	}
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
        if (fmt.fmt.pix.sizeimage < min){
                fmt.fmt.pix.sizeimage = min;
	}
	userp_init(fmt.fmt.pix.sizeimage);
}

void tv_dev_video_t::request_buffers(){
	v4l2_requestbuffers req;
	CLEAR(req);
	req.count = 1;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;
	set_ioctl(VIDIOC_REQBUFS, &req);
}

void tv_dev_video_t::add_buffer(){
	v4l2_buffer buf;
	CLEAR(buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_USERPTR;
	buf.index = 0;
	buf.m.userptr = (unsigned long)raw_pixel_data;
	buf.length = raw_pixel_size;
	if(raw_pixel_data == nullptr || raw_pixel_size == 0){
		print("raw pixel data hasn't been initialized yet", P_ERR);
	}
	try{
		set_ioctl(VIDIOC_QBUF, &buf);
	}catch(...){
		// queue is just one long, so errors means it is
		// already filled, and that is safe (for now)
	}
}

void tv_dev_video_t::start_capturing(){
	add_buffer();
	v4l2_buf_type type;
	CLEAR(type);
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	set_ioctl(VIDIOC_STREAMON, &type);
}

tv_dev_video_t::tv_dev_video_t(std::string filename_,
			       uint16_t refresh_rate_) : id(this,
							    __FUNCTION__){
	list_virtual_data(&id);
	open_dev(filename_,
		 refresh_rate_);
	try{
		if(!is_compatible()){
			print("video device is incompatible", P_ERR);
		}
		standard_init();
		request_buffers();
		start_capturing();
		// break this up into smaller functions
	}catch(std::runtime_error e){
		print("unable to initialize tv_dev_video_t", P_ERR);
	}
	print("updated everything", P_SPAM);
}

tv_dev_video_t::~tv_dev_video_t(){
}

/*
  pixel_data is not what is directly read from V4L2, but is instead
  the RGB24 version of that information. Since my webcam (at least) doesn't
  have native support for it, handle all of the conversions here.
 */

void tv_dev_video_t::update_pixel_data(){
	v4l2_format fmt;
	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	set_ioctl(VIDIOC_G_FMT, &fmt);
	uint64_t current_raw = 0;
	uint64_t current_std = 0;
	switch(fmt.fmt.pix.pixelformat){
	case V4L2_PIX_FMT_YUYV:
		// TODO: care about other cameras
		// Just to test it, conver the brightness into grayscale
		while(current_raw+5 < raw_pixel_size &&
			current_std+4 < pixel_size){
			const uint8_t y_comp =
				raw_pixel_data[current_raw];
			pixel_data[current_std] = y_comp;
			pixel_data[current_std+1] = y_comp;
			pixel_data[current_std+2] = y_comp;
			current_raw += 4;
			current_std += 3;
		}
	default:
		break;
	}
}

void tv_dev_video_t::update_raw_pixel_data(){
	add_buffer();
	v4l2_buffer buf;
	CLEAR(buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_USERPTR;
	set_ioctl(VIDIOC_DQBUF, &buf);
	assert(buf.m.userptr == (unsigned long)raw_pixel_data);
}

uint64_t tv_dev_video_t::update(){
	// TODO: enforce the refresh rate here
	update_raw_pixel_data();
	update_pixel_data();
	tv_frame_video_t *new_frame =
		new tv_frame_video_t;
	new_frame->set_all(640,
			   480,
			   TV_FRAME_DEFAULT_BPC,
			   TV_FRAME_DEFAULT_RED_MASK,
			   TV_FRAME_DEFAULT_GREEN_MASK,
			   TV_FRAME_DEFAULT_BLUE_MASK,
			   0);
	// pixel_data should be converted to RGB24, but that isn't
	// the case now
	std::memcpy(new_frame->get_pixel_data_ptr(),
		    pixel_data,
		    pixel_size);
	set_last_frame_id(new_frame->id.get_id());
	return get_last_frame_id();
}

tv_dev_audio_t::tv_dev_audio_t() : id(this, __FUNCTION__){
	list_virtual_data(&id);
}

// worry about destroying stuff safely later

tv_dev_audio_t::~tv_dev_audio_t(){
}
