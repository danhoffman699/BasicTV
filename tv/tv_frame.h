#include "../main.h"
#include "tv.h"
#ifndef TV_FRAME_H
#define TV_FRAME_H
/*
  tv_frame_t: individual frame with all of the settings as requested
  from tv_client_t
 */

struct tv_frame_t{
private:
	std::array<uint8_t, TV_FRAME_SIZE> frame = {{0}};
	std::array<uint8_t, TV_FRAME_AUDIO_SIZE> audio = {{0}};
	// channel ID
	uint64_t channel_id = 0;
	/*
	   frame_number, increment by one every time.
	 */
	uint64_t frame_number = 0;
	uint64_t x_res = 0;
	uint64_t y_res = 0;
	uint64_t unix_timestamp = 0;
	uint8_t frame_count = 0;
public:
	data_id_t id;
	tv_frame_t();
	~tv_frame_t();
	/*
	  most settings cannot be changed OTF
	 */
	void reset_frame(uint64_t x, uint64_t y);
	void set_pixel(uint64_t x, uint64_t y, uint64_t frame_number);
	uint64_t get_frame_number();
	uint64_t get_pixel(uint64_t x, uint64_t y);
	uint64_t get_x_res();
	uint64_t get_y_res();
	uint64_t get_timestamp();
};

#endif
