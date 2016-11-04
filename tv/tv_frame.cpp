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
	id.add_data(&unix_timestamp_ms, sizeof(unix_timestamp_ms));
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
	const std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	unix_timestamp_ms = (uint64_t)ms.count();
}

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2

uint64_t tv_frame_t::get_raw_pixel_pos(uint64_t x,
				       uint64_t y){
	if(bpc % 8 != 0){
		print("bpc isn't valid", P_CRIT);
	}
	if(x > x_res || y > y_res){
		print("resolution out of bounds", P_CRIT);
	}
	const uint64_t raw_pixel_pos = 
		((y_res*x)+y)*((bpc*3)/8);
	// TV_FRAME_SIZE should be checked against frame res
	return raw_pixel_pos;
}

void tv_frame_t::set_pixel(uint64_t x,
			   uint64_t y,
			   std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color){
	const uint64_t pixel_pos =
		get_raw_pixel_pos(x, y);
	uint64_t *pixel = (uint64_t*)&(frame[pixel_pos]);
	/*
	  Can't use convert functions because of the small-ish sizes
	  possible with this, and this is pretty elegant as it is. 
	  Maybe another time?
	 */
	color = convert::color::bpc(color, bpc);
	(*pixel) &= ~flip_bit_section(0, bpc*3);
	(*pixel) |= std::get<0>(color);
	(*pixel) |= std::get<1>(color) S_L (bpc);
	(*pixel) |= std::get<2>(color) S_L (bpc*2);
}

std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> tv_frame_t::get_pixel(uint64_t x,
									uint64_t y){
	std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color;
	const uint64_t *pixel =
		(uint64_t*)&(frame[get_raw_pixel_pos(x, y)]);
	const uint64_t bpc_mask =
		(1 S_L (bpc+1))-1;
	std::get<0>(color) = ((*pixel << bpc*0) & bpc_mask);
	std::get<1>(color) = ((*pixel << bpc*1) & bpc_mask);
	std::get<2>(color) = ((*pixel << bpc*2) & bpc_mask);
	std::get<3>(color) = bpc;
	return color;
}

// frame only uses uint8_t for compression, uint64_t is used
// for internal use

uint64_t tv_frame_t::get_frame_number(){
	return frame_number;
}

uint64_t tv_frame_t::get_timestamp_ms(){
	return unix_timestamp_ms;
}

uint64_t tv_frame_t::get_x_res(){
	return x_res;
}

uint64_t tv_frame_t::get_y_res(){
	return y_res;
}

uint8_t tv_frame_t::get_bpc(){
	return bpc;
}

uint64_t tv_frame_t::get_red_mask(){
	switch(bpc){
	case 8:
		return (uint64_t)0xFF;
	default:
		print("unsupported bpc value", P_CRIT);
	}
	return 0;
}


uint64_t tv_frame_t::get_green_mask(){
	return (get_red_mask() S_L ((uint64_t)bpc));
}

uint64_t tv_frame_t::get_blue_mask(){
	return (get_green_mask() S_L ((uint64_t)bpc));
}

uint64_t tv_frame_t::get_alpha_mask(){
	return (get_blue_mask() S_L ((uint64_t)bpc));
}
