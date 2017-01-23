#ifndef TV_DEV_VIDEO_H
#define TV_DEV_VIDEO_H
#include "tv_dev.h"
class tv_dev_video_t : public tv_dev_t{
private:
	uint8_t *raw_pixel_data = nullptr;
	uint8_t *pixel_data = nullptr;
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
	id_t_ update();
	uint64_t get_frame_interval_micro_s();
};

#endif
