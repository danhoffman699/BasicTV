#ifndef TV_DEV_H
#define TV_DEV_H
/*
  tv_dev: interface for Linux kernel devices
 */

// TODO: make this unexportable
class tv_dev_t{
private:
	uint16_t refresh_rate = 0;
	std::string filename;
	std::ifstream file_stream;
	uint64_t last_frame_id = 0;
	// timestamp ought to be associated with frame
public:
	tv_dev_t();
	~tv_dev_t();
	void open_dev(std::string filename_,
		      uint16_t refresh_rate_);
	void get_last_frame_id();
	uint16_t get_refresh_rate();
};

// virtual classes live off the ID of the implementor

class tv_dev_video_t : public tv_dev_t{
private:
public:
	data_id_t id;
	tv_dev_video_t();
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
