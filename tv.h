#include "id.h"
#ifndef TV_H
#define TV_H
#include "utility"
#define TV_FRAME_SIZE (7680*4320*16)

/*
  8K TVs in 4x4 should be good enough, actual limit is
  the stack size
 */

/*
  Lossless here means that the versions of the stream are the
  same on both versions, and the actual patch is not compressed
  This is not inherently the highest quality stream, just the
  same version of the same quality stream
 */
#define TV_PATCH_INFO_LOSSLESS (1)
#define TV_PATCH_INFO_LOSSY (0)

struct tv_patch_t{
private:
	// location of the byte to change
	std::array<uint64_t, 65536> location = {};
	// new value of the byte
	std::array<uint64_t, 65536> value = {};
	// id to be applied to
	/*
	  TODO: define a minimum amount of frames that the 
	  client is guaranteed to keep, so tv_patch_t can
	  more easily reference different frames without
	  a handshake system to ensure that the client
	  has the data (also helps with distribution of content).
	 */
	uint64_t frame_id = 0;
	/*
	  Any useful boolean information:
	  lossy or lossless?
	  physically more information
	 */
	uint64_t info = 0;
public:
	data_id_t id;
	tv_patch_t(uint64_t frame_id_);
	~tv_patch_t();
	uint64_t get_frame_id();
	void set_frame_id(uint64_t frame_id_);
};

struct tv_frame_t{
private:
	// raw data from the frame, X(XRES) + Y for the value
	/*
	  TODO: Change this into uint8_t and add multiple
	  frames in the same variable to send out more 
	  color information (8-bit is okay for dial-up and
	  really slow connections)
	 */
	std::array<uint64_t, TV_FRAME_SIZE> frame = {};
	// channel ID
	uint64_t channel_id = 0;
	/* 
	   frame_number, increment by one every time.
	 */
	uint64_t frame_number = 0;
	uint16_t x_res = 0;
	uint16_t y_res = 0;
public:
	data_id_t id;
	tv_frame_t();
	~tv_frame_t();
	void set_pixel(uint64_t x, uint64_t y, uint64_t value);
	uint64_t get_pixel(uint64_t x, uint64_t y);
};

struct tv_channel_t{
private:
	std::array<uint8_t, 1024> public_key = {};
	std::array<uint64_t, 1024> frame_list_id = {};
public:
	data_id_t id;
	tv_channel_t();
	~tv_channel_t();
	uint64_t get_frame_id(uint64_t backstep);
};

namespace tv{
	void init();
	void loop();
	void close();
}
#endif
