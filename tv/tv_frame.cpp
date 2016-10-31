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
		       uint8_t bpc_,
		       uint64_t time_to_live_, 
		       uint64_t sampling_rate_,
		       uint8_t channel_count_,
		       uint8_t amp_depth_){
	const uint64_t audio_size = time_to_live_*sampling_rate_;
	if(audio_size >= TV_FRAME_AUDIO_SIZE){
		print("audio is too large for frame", P_ERR);
	}
	if(x*y*(bpc*3) > TV_FRAME_SIZE){
		print("frame is too large for picture array", P_ERR);
	}
	x_res = x;
	y_res = y;
	bpc = bpc_;
	time_to_live = time_to_live_;
	channel_count = channel_count_;
	amp_depth = amp_depth_;
	frame.fill(0);
	unix_timestamp = std::time(nullptr);
}

static uint64_t tv_find_first_pixel(std::array<uint8_t, TV_FRAME_SIZE> frame,
			  uint64_t x_res,
			  uint64_t y_res,
			  uint64_t x,
			  uint64_t y){
	
	// TODO: find a more friendly way to orient this and
	// make some macro
	const uint64_t entry =
		(x_res*y)+x;
	if(entry >= TV_FRAME_SIZE){
		print("pixel out of TV_FRAME_SIZE bounds", P_ERR);
	}
	if(entry >= x_res*y_res){
		print("pixel out of pre-defined bounds", P_ERR);
	}
	return entry;
	
}

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2

uint64_t tv_frame_t::convert_to_raw_pixel(std::array<uint64_t, 3> tmp,
					  uint8_t old_bpc){
	uint64_t retval = 0;
	const uint64_t bpc_max_val = pow(2, bpc);
	const uint64_t old_bpc_max_val = pow(2, old_bpc);
	for(uint64_t i = 0;i < 3;i++){
		retval |= tmp[i]*(bpc_max_val/old_bpc_max_val) >> (i*bpc);
	}
	return retval;
}

void tv_frame_t::write_raw_pixel(uint64_t x, uint64_t y, uint64_t pixel){
	const uint64_t first_pixel =
		tv_find_first_pixel(frame, x_res, y_res, x, y);
	for(uint64_t i = 0;i < 8;i++){
		const uint64_t byte_entry = first_pixel+(i*x_res*y_res);
		uint8_t *byte_tmp = (uint8_t*)&pixel;
		frame[byte_entry] = byte_tmp[i];
		byte_tmp = nullptr;
	}
}

uint64_t tv_frame_t::read_raw_pixel(uint64_t x, uint64_t y){
	const uint64_t first_pixel =
		tv_find_first_pixel(frame, x_res, y_res, x, y);
	uint64_t retval = 0;
	for(uint64_t i = 0;i < 8;i++){
		uint8_t *byte_array = (uint8_t*)&retval;
		byte_array[i] = frame[first_pixel+(i*x_res*y_res)];
		byte_array = nullptr;
	}
	return retval;
}

void tv_frame_t::set_pixel(uint64_t x,
			   uint64_t y,
			   std::array<uint64_t, 3> raw_colors,
			   uint8_t raw_bpc){
	const uint64_t pixel = convert_to_raw_pixel(raw_colors, raw_bpc);
	write_raw_pixel(x, y, pixel);
}

uint64_t tv_frame_t::get_pixel(uint64_t x,
			       uint64_t y,
			       uint8_t bpc_tmp){
	uint64_t raw_pixel = read_raw_pixel(x, y);
	uint64_t pixel = 0;
	for(uint64_t i = 0;i < 3;i++){
		uint64_t tmp = (raw_pixel << i*bpc) & ~(~0 >> bpc);
		tmp *= bpc_tmp/bpc;
		pixel |= (tmp >> i*bpc);
	}
	return pixel;
}

#undef COLOR_RED
#undef COLOR_GREEN
#undef COLOR_BLUE

// frame only uses uint8_t for compression, uint64_t is used
// for internal use

uint64_t tv_frame_t::get_frame_number(){
	return frame_number;
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
