#include "tv_dev.h"
#include "tv_dev_video.h"

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
	raw_pixel_data = new uint8_t[image_size];
	raw_pixel_size = image_size;
	pixel_size = 640*480*3; // TODO: define with variables
	pixel_data = new uint8_t[pixel_size];
	try{
		v4l2_requestbuffers req;
		req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory = V4L2_MEMORY_USERPTR;
		req.count = 1;
		set_ioctl(VIDIOC_REQBUFS, &req);
	}catch(...){
		print("couldn't request the buffers", P_ERR);
	}
}

uint64_t tv_dev_video_t::get_frame_interval_micro_s(){
	v4l2_format fmt;
	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	try{
		set_ioctl(VIDIOC_G_FMT, &fmt);
	}catch(std::exception e){
		print("cannot fetch format", P_ERR);
	}
	v4l2_frmivalenum frame_interval;
	CLEAR(frame_interval);
	frame_interval.width = fmt.fmt.pix.width;
	frame_interval.height = fmt.fmt.pix.height;
	frame_interval.pixel_format = fmt.fmt.pix.pixelformat;
	frame_interval.index = 1; // I don't know
	try{
		set_ioctl(VIDIOC_ENUM_FRAMEINTERVALS, &frame_interval);
	}catch(std::exception e){
		print("cannot set frame interval", P_ERR);
	}
	v4l2_fract fraction;
	switch(frame_interval.type){
	case V4L2_FRMIVAL_TYPE_DISCRETE:
		fraction = frame_interval.discrete;
		break;
	case V4L2_FRMIVAL_TYPE_CONTINUOUS:
	case V4L2_FRMIVAL_TYPE_STEPWISE:
		fraction = frame_interval.stepwise.step;
		break; // is this correct ^ ?
	default:
		CLEAR(fraction);
		fraction.denominator = 1; // should have real detection
	}
	P_V(fraction.numerator, P_SPAM);
	P_V(fraction.denominator, P_SPAM);
	long double frame_interval_float =
		(long double)fraction.numerator/
		(long double)fraction.denominator;
	uint64_t retval = (uint64_t)((frame_interval_float*1000*1000)+0.5);
	return retval;
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
		print("couldn't initialize cropping", P_WARN);
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
		print("pixel format has changed", P_NOTE);
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
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;
	req.count = 1;
	set_ioctl(VIDIOC_REQBUFS, &req);
}

void tv_dev_video_t::add_buffer(){
	v4l2_buffer buf;
	CLEAR(buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_USERPTR;
	buf.m.userptr = (unsigned long)raw_pixel_data;
	buf.length = raw_pixel_size;
	if(raw_pixel_data == nullptr || raw_pixel_size == 0){
		print("raw pixel data hasn't been initialized yet", P_ERR);
	}
	try{
		set_ioctl(VIDIOC_QBUF, &buf);
	}catch(...){
		//print("couldn't queue item", P_ERR);
		// queue is just one long, so errors means it is
		// already filled, and that is safe (for now)
	}
}

void tv_dev_video_t::start_capturing(){
	add_buffer();
	v4l2_buf_type type;
	CLEAR(type);
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	try{
		set_ioctl(VIDIOC_STREAMON, &type);
	}catch(...){
		print("couldn't start stream", P_ERR);
	}
}

/*
  pixel_data is not what is directly read from V4L2, but is instead
  the RGB24 version of that information. Since my webcam (at least) doesn't
  have native support for it, handle all of the conversions here.

  I have no plans to support YUV formats, and it will be converted to
  RGB as soon as possible. 
 */

void tv_dev_video_t::update_pixel_data(){
	v4l2_format fmt;
	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	set_ioctl(VIDIOC_G_FMT, &fmt);
	uint32_t current_raw = 0;
	uint32_t current_std = 0;
	const uint32_t raw_pixel_size_yuyv = raw_pixel_size-5;
	const uint32_t pixel_size_yuyv = pixel_size-4;
	if(unlikely(raw_pixel_size < 5 ||
		    pixel_size < 4)){
		print("resolution is too small for conversion", P_ERR);
	}
	switch(fmt.fmt.pix.pixelformat){
	case V4L2_PIX_FMT_YUYV: // most webcams, only does grayscale for now
		while(likely(current_raw < raw_pixel_size_yuyv) &&
		      likely(current_std < pixel_size_yuyv)){
			const uint8_t y_comp =
				raw_pixel_data[current_raw];
			pixel_data[current_std] = y_comp;
			pixel_data[current_std+1] = y_comp;
			pixel_data[current_std+2] = y_comp;
			current_raw += 4;
			current_std += 3;
		}
		break;
	case V4L2_PIX_FMT_RGB24: // native format
		std::memcpy(raw_pixel_data,
			    pixel_data,
			    (pixel_size > raw_pixel_size) ?
			    raw_pixel_size : pixel_size);
		break;
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


tv_dev_video_t::tv_dev_video_t(std::string filename_) : id(this,
							   __FUNCTION__){
	list_virtual_data(&id);
	open_dev(filename_);
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
}

tv_dev_video_t::~tv_dev_video_t(){
	delete[] raw_pixel_data;
	raw_pixel_data = nullptr;
	raw_pixel_size = 0;
	delete[] pixel_data;
	pixel_data = nullptr;
	pixel_size = 0;
}
