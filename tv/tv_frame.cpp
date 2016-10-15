#include "../main.h"
#include "../util.h"
#include "tv.h"
#include "tv_channel.h"
#include "tv_frame.h"

tv_frame_t::tv_frame_t() : id(this, __FUNCTION__){
	id.add_data(&(frame[0]), sizeof(frame[0])*TV_FRAME_SIZE);
	id.add_data(&channel_id, sizeof(channel_id));
	id.add_id(&channel_id, sizeof(channel_id));
	id.add_data(&frame_number, sizeof(frame_number));
	id.add_data(&unix_timestamp, sizeof(unix_timestamp));
}

tv_frame_t::~tv_frame_t(){
}

/*
  NOTE:
  Colors are stored in 1-4 bytes, 1 byte uses a lookup table, 2-4 use
  actual byte-data. They are stored in little-endian
 */

/*
  TODO: implement
 */

void tv_frame_t::set_pixel(uint64_t x, uint64_t y, uint64_t value){
	// TODO: implement
}

// frame only uses uint8_t for compression, uint64_t is used
// for internal use

uint64_t tv_frame_t::get_pixel(uint64_t x, uint64_t y){
	// TODO: implement
	
	return 0;
}

uint64_t tv_frame_t::get_timestamp(){
	return unix_timestamp;
}

uint64_t tv_frame_t::get_x_res(){
	return x_res;
}

uint64_t tv_frame_t::get_y_res(){
	return y_res;
}
