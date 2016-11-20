#ifndef TV_DEV_H
#define TV_DEV_H
#include "sys/stat.h"
#include "fcntl.h"
#include "errno.h"
#include "sys/ioctl.h"
#include "linux/videodev2.h"
#include "cassert"
/*
  tv_dev: interface for Linux kernel devices
 */
// TODO: make this unexportable
class tv_dev_t{
private:
	std::string filename;
	int64_t file_descriptor = 0;
	uint64_t last_frame_id = 0;
	// timestamp ought to be associated with frame
public:
	tv_dev_t();
	~tv_dev_t();
	void list_virtual_data(data_id_t *id);
	void open_dev(std::string filename_);
	uint64_t get_last_frame_id();
	void set_last_frame_id(uint64_t last_frame_id_);
	uint64_t get_file_descriptor();
	int set_ioctl(int request,
		      void *arg);
};

// virtual classes live off the ID of the implementor

// pure video, no audio

class tv_dev_video_t : public tv_dev_t{
private:
	uint8_t *raw_pixel_data = 0;
	uint8_t *pixel_data = 0;
	uint64_t raw_pixel_size = 0;
	uint64_t pixel_size = 0;
	/*
	  Resolutions and the like are stored inside of the v4l2_format
	  data type, which is loaded in through the standard init.
	  Just refer to these variable to prevent continuity errors
	 */
	bool is_compatible();
	void standard_init();
	void userp_init(uint64_t image_size);
	void request_buffers();
	void start_capturing();
	void update_raw_pixel_data();
	void update_pixel_data();
	void add_buffer();
public:
	data_id_t id;
	tv_dev_video_t(std::string filename_);
	~tv_dev_video_t();
	uint64_t update();
};

class tv_dev_audio_t : public tv_dev_t{
private:
public:
	data_id_t id;
	tv_dev_audio_t();
	~tv_dev_audio_t();
	uint64_t update();
};
#endif
