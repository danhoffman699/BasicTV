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
	/*
	  TODO: add the rest of the variables
	 */
}

tv_frame_t::~tv_frame_t(){
}

/*
  NOTE:
  Colors are stored in 1-4 bytes, 1 byte uses a lookup table, 2-4 use
  actual byte-data. They are stored in little-endian
 */

/*
  TODO: Instead of throwing errors on too large of data, implement some
  conversion/downsampling functions. It should never run anyways
 */

void tv_frame_t::reset(uint64_t x,
		       uint64_t y,
		       uint8_t color_depth_,
		       uint64_t time_to_live_, 
		       uint64_t sampling_rate_,
		       uint8_t channel_count_,
		       uint8_t amp_depth_){
	const uint64_t audio_size = time_to_live_*sampling_rate_;
	if(audio_size >= TV_FRAME_AUDIO_SIZE){
		print("audio is too large for frame", P_ERR);
	}
	if(x*y*color_depth_ > TV_FRAME_SIZE){
		print("frame is too large for picture array", P_ERR);
	}
	x_res = x;
	y_res = y;
	color_depth = color_depth_;
	time_to_live = time_to_live_;
	channel_count = channel_count_;
	amp_depth = amp_depth_;
	frame.fill(0);
	unix_timestamp = std::time(nullptr);
}

void tv_frame_t::set_pixel(uint64_t x, uint64_t y, uint64_t value){
	
}

// frame only uses uint8_t for compression, uint64_t is used
// for internal use

uint64_t tv_frame_t::get_pixel(uint64_t x, uint64_t y){
	// TODO: implement
	
	return 0;
}

uint64_t tv_frame_t::get_frame_number(){
	return frame_number;
}

uint64_t tv_frame_t::get_frame_linked_list_entry(){
	for(uint64_t i = 0;i < TV_FRAME_LINKED_LIST_HEIGHT;i++){
		if(frame_linked_list[i][1] == id.get_id()){
			return i;
		}
	}
	print("can't find frame ID in own copy of array", P_ERR);
}

uint64_t tv_frame_t::get_frame_id_prev(){
	return frame_linked_list[get_frame_linked_list_entry()][0];
}

uint64_t tv_frame_t::get_frame_id_next(){
	return frame_linked_list[get_frame_linked_list_entry()][2];
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
