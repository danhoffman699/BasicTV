#ifndef TV_DEV_H
#define TV_DEV_H
/*
  tv_dev: interface for Linux kernel devices
 */

// TODO: make this unexportable
class tv_dev_t{
private:
	// refresh rate is defined in Hz
	uint16_t refresh_rate = 0;
	std::string filename;
	uint64_t file_descriptor = 0;
	uint64_t last_frame_id = 0;
	// timestamp ought to be associated with frame
public:
	tv_dev_t();
	~tv_dev_t();
	void list_virtual_data(data_id_t *id);
	void open_dev(std::string filename_,
		      uint16_t refresh_rate_);
	uint64_t get_last_frame_id();
	void set_last_frame_id(uint64_t last_frame_id_);
	uint16_t get_refresh_rate();
	uint64_t get_file_descriptor();
	int set_ioctl(int fd,
		      int request,
		      void *arg);
};

// virtual classes live off the ID of the implementor

// pure video, no audio

class tv_dev_video_t : public tv_dev_t{
private:
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
