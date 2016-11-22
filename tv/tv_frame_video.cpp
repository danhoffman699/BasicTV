#include "tv_frame_standard.h"
#include "tv_frame_video.h"

static uint64_t mask_16_to_64(uint16_t mask){
	const uint8_t mask_length = GET_MASK_LENGTH(mask);
	const uint8_t mask_offset = GET_MASK_OFFSET(mask);
	const uint64_t retval =
		flip_bit_section(
			mask_offset,
			mask_offset+mask_length);
	return retval;
}

static uint16_t mask_64_to_16(uint64_t mask){
	uint8_t mask_length = 0;
	uint8_t mask_offset = 255;
	for(uint8_t i = 0;i < 64;i++){
		if((mask >> i) & 1){
			if(mask_offset == 255){
				mask_offset = i;
			}
			mask_length++;
		}
	}
	if(mask_offset == 255){
		mask_offset = 0;
	}
	return SET_MASK_LENGTH(mask_length) |
		SET_MASK_OFFSET(mask_offset);
}

static bool valid_bpc(uint8_t bpc){
	return bpc == 8;
}

static bool valid_masks(uint64_t red,
			uint64_t green,
			uint64_t blue,
			uint64_t alpha){
	P_V_B(red, P_SPAM);
	P_V_B(green, P_SPAM);
	P_V_B(blue, P_SPAM);
	P_V_B(alpha, P_SPAM);
	return true;
}

static void tv_frame_color_sanity_check(std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color){
	const uint64_t bpc = std::get<3>(color);
	if(unlikely(std::get<0>(color) > MASK(bpc) ||
		    std::get<1>(color) > MASK(bpc) ||
		    std::get<2>(color) > MASK(bpc))){
		print("color is not withing BPC bounds", P_ERR);
	}
	if(unlikely(!valid_bpc(bpc))){
		print("BPC is invalid", P_ERR);
	}
}

tv_frame_video_t::tv_frame_video_t() : id(this, __FUNCTION__){
	list_virtual_data(&id);
	ADD_DATA(x_res);
	ADD_DATA(y_res);
	ADD_DATA(bpc);
	ADD_DATA(red_mask);
	ADD_DATA(green_mask);
	ADD_DATA(blue_mask);
	ADD_DATA(alpha_mask);
}

tv_frame_video_t::~tv_frame_video_t(){
}

uint64_t tv_frame_video_t::get_raw_pixel_pos(uint16_t x,
					     uint16_t y){
	if(unlikely(x >= x_res || y >= y_res)){
		print("resolution out of bounds", P_CRIT);
	}
	const uint64_t major =
		x_res*y;
	const uint64_t minor =
		x;
	if(unlikely(!valid_bpc(bpc))){
		print("cannot support alternate frames at the time", P_ERR);
	}
	const uint64_t raw_pixel_pos = 
		(major+minor)*3;
	return raw_pixel_pos;
}

void tv_frame_video_t::set_pixel(uint16_t x,
				 uint16_t y,
				 std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color){
	tv_frame_color_sanity_check(color);
	const uint64_t pixel_pos =
		get_raw_pixel_pos(x, y);
	uint64_t *pixel = (uint64_t*)&(frame[pixel_pos]);
	color = convert::color::bpc(color, bpc);
	(*pixel) &= ~flip_bit_section(0, bpc*3);
	(*pixel) |= std::get<0>(color) & MASK(bpc);
	(*pixel) |= (std::get<1>(color) & MASK(bpc)) << (bpc);
	(*pixel) |= (std::get<2>(color) & MASK(bpc)) << (bpc*2);
}

std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> tv_frame_video_t::get_pixel(uint16_t x_,
									      uint16_t y_){
	std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color;
	const uint64_t *pixel =
		(uint64_t*)&(frame[get_raw_pixel_pos(x_, y_)]);
	std::get<0>(color) = ((*pixel >> (bpc*0)) & MASK(bpc));
	std::get<1>(color) = ((*pixel >> (bpc*1)) & MASK(bpc));
	std::get<2>(color) = ((*pixel >> (bpc*2)) & MASK(bpc));
	std::get<3>(color) = bpc;
	return color;
}

std::array<uint8_t, TV_FRAME_VIDEO_SIZE>* tv_frame_video_t::get_pixel_data_ptr(){
	return &frame;
}

void tv_frame_video_t::get_masks(uint64_t *red_mask_,
				 uint64_t *green_mask_,
				 uint64_t *blue_mask_,
				 uint64_t *alpha_mask_,
				 uint8_t *bpc_){
	
	if(likely(red_mask_ != nullptr)){
		*red_mask_ = mask_16_to_64(red_mask);
	}
	if(likely(green_mask_ != nullptr)){
		*green_mask_ = mask_16_to_64(green_mask);
	}
	if(likely(blue_mask_ != nullptr)){
		*blue_mask_ = mask_16_to_64(blue_mask);
	}
	if(likely(alpha_mask_ != nullptr)){
		*alpha_mask_ = mask_16_to_64(alpha_mask);
	}
	if(likely(bpc_ != nullptr)){
		*bpc_ = bpc; 
	}
}

void tv_frame_video_t::get_res(uint16_t *x_res_,
			       uint16_t *y_res_){
	if(likely(x_res_ != nullptr)){
		*x_res_ = x_res;
	}
	if(likely(y_res_ != nullptr)){
		*y_res_ = y_res;
	}
}

void tv_frame_video_t::set_all(uint16_t x_res_,
			       uint16_t y_res_,
			       uint8_t bpc_,
			       uint64_t red_mask_,
			       uint64_t green_mask_,
			       uint64_t blue_mask_,
			       uint64_t alpha_mask_){
	x_res = x_res_;
	y_res = y_res_;
	bpc = bpc_;
	red_mask = mask_64_to_16(red_mask_);
	green_mask = mask_64_to_16(green_mask_);
	blue_mask = mask_64_to_16(blue_mask_);
	alpha_mask = mask_64_to_16(alpha_mask_);
}
