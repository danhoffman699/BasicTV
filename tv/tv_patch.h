#include "../main.h"
#include "tv.h"
#ifndef TV_PATCH_H
#define TV_PATCH_H
/*
  tv_patch_t: patching the frame data. This CANNOT alter
  the bit depth or any settings with the tv_frame_t type
  except for framerate, a new frame will have to be sent 
  over for that to happen. This is only meant to patch the 
  current frame and keep all of the settings

  TODO: implement audio patches
*/

struct tv_patch_video_t{
private:
	// location of the byte to change
	std::array<uint64_t, STD_ARRAY_LENGTH> location = {{0}};
	// new value of the byte
	std::array<uint64_t, STD_ARRAY_LENGTH> value = {{0}};
	// id to be applied to
	uint64_t frame_id = 0;
public:
	data_id_t id;
	tv_patch_video_t();
	~tv_patch_video_t();
	uint64_t get_frame_id();
	void set_frame_id(uint64_t frame_id_);
};

#endif
