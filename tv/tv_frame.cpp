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
	id.add_data(&timestamp_micro_s, sizeof(timestamp_micro_s));
	/*
	  TODO: add the rest of the variables
	 */
}

tv_frame_t::~tv_frame_t(){
}

void tv_frame_t::reset(uint64_t x,
		       uint64_t y,
		       uint8_t bpc_,
		       uint64_t red_mask_,
		       uint64_t green_mask_,
		       uint64_t blue_mask_,
		       uint64_t alpha_mask_,
		       uint64_t time_to_live_micro_s_, 
		       uint64_t sampling_rate_,
		       uint8_t channel_count_,
		       uint8_t amp_depth_){
	// set sane audio values
	const uint64_t audio_size = (time_to_live_micro_s_/1000000)*sampling_rate_;
	if(audio_size >= TV_FRAME_AUDIO_SIZE){
		print("audio is too large for frame", P_ERR);
	}
	if(x*y*(bpc*3) > TV_FRAME_SIZE){
		print("frame is too large for picture array", P_ERR);
	}
	x_res = x;
	y_res = y;
	P_V(bpc, P_SPAM);
	bpc = bpc_;
	red_mask = red_mask_;
	green_mask = green_mask_;
	blue_mask = blue_mask_;
	alpha_mask = alpha_mask_;
	time_to_live_micro_s = time_to_live_micro_s_;
	channel_count = channel_count_;
	amp_depth = amp_depth_;
	frame.fill(0);
	timestamp_micro_s = get_time_microseconds();
}

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2

uint64_t tv_frame_t::get_raw_pixel_pos(uint64_t x,
				       uint64_t y){
	if(unlikely(x >= x_res || y >= y_res)){
		print("resolution out of bounds", P_CRIT);
	}
	const uint64_t major =
		x_res*y;
	const uint64_t minor =
		x;
	if(unlikely(bpc != 8)){
		print("cannot support alternate frames at the time", P_ERR);
	}
	const uint64_t raw_pixel_pos = 
		(major+minor)*3;
	return raw_pixel_pos;
}

static void tv_frame_color_sanity_check(std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color){
	const uint64_t bpc = std::get<3>(color);
	if(std::get<0>(color) > MASK(bpc) ||
	   std::get<1>(color) > MASK(bpc) ||
	   std::get<2>(color) > MASK(bpc)){
		print("color is not withing BPC bounds", P_ERR);
	}
	if(bpc != 8){
		print("BPC is not a valid value", P_ERR);
	}
}

void tv_frame_t::set_pixel(uint64_t x,
			   uint64_t y,
			   std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color){
	const uint64_t pixel_pos =
		get_raw_pixel_pos(x, y);
	uint64_t *pixel = (uint64_t*)&(frame[pixel_pos]);
	tv_frame_color_sanity_check(color);
	color = convert::color::bpc(color, bpc);
	(*pixel) &= ~flip_bit_section(0, bpc*3);
	(*pixel) |= std::get<0>(color) & MASK(bpc);
	(*pixel) |= (std::get<1>(color) & MASK(bpc)) S_L (bpc);
	(*pixel) |= (std::get<2>(color) & MASK(bpc)) S_L (bpc*2);
}

std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> tv_frame_t::get_pixel(uint64_t x,
									uint64_t y){
	std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color;
	const uint64_t *pixel =
		(uint64_t*)&(frame[get_raw_pixel_pos(x, y)]);
	std::get<0>(color) = ((*pixel S_S (bpc*0)) & MASK(bpc));
	std::get<1>(color) = ((*pixel S_S (bpc*1)) & MASK(bpc));
	std::get<2>(color) = ((*pixel S_S (bpc*2)) & MASK(bpc));
	std::get<3>(color) = bpc;
	/*
	  color sanity checks only check against BPC, which is used as the mask,
	  so it is guaranteed to pass (even if it is wrong)
	 */
	return color;
}

void tv_frame_t::set_pixel_data(std::array<uint8_t, TV_FRAME_SIZE> frame_){
}

std::array<uint8_t, TV_FRAME_SIZE>* tv_frame_t::get_pixel_data_ptr(){
	return &frame;
}

// frame only uses uint8_t for compression, uint64_t is used
// for internal use

uint64_t tv_frame_t::get_frame_number(){
	return frame_number;
}

uint64_t tv_frame_t::get_x_res(){
	return x_res;
}

uint64_t tv_frame_t::get_y_res(){
	return y_res;
}

uint8_t tv_frame_t::get_bpc(){
	if(unlikely(bpc == 0)){
		print("invalid bpc", P_ERR);
	}
	return bpc;
}

uint64_t tv_frame_t::get_red_mask(){
	return red_mask;
}

uint64_t tv_frame_t::get_green_mask(){
	return green_mask;
}

uint64_t tv_frame_t::get_blue_mask(){
	return blue_mask;
}

uint64_t tv_frame_t::get_alpha_mask(){
	return alpha_mask;
}

uint64_t tv_frame_t::get_time_to_live_micro_s(){
	return time_to_live_micro_s;
}

uint64_t tv_frame_t::get_timestamp_micro_s(){
	return timestamp_micro_s;
}

uint64_t tv_frame_t::get_end_time_micro_s(){
	return get_timestamp_micro_s()+
		get_time_to_live_micro_s();
}

void tv_frame_t::set_timestamp_micro_s(uint64_t timestamp_micro_s_){
	timestamp_micro_s = timestamp_micro_s_;
}
