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

#include "tv_dev_audio.h"
#include "tv_dev_video.h"
#endif
