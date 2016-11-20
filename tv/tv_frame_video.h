#ifndef TV_FRAME_VIDEO_H
#define TV_FRAME_VIDEO_H
#include "tv_frame_standard.h"

/*
  Masks are stored in two parts, one for the length of the
  masked data (log base 2), the second is the offset from 
  zero
 */

#define TV_FRAME_8K_RES (7680*4320)

#define TV_FRAME_SIZE (1920*1080*4)

#define TV_FRAME_DEFAULT_X 640
#define TV_FRAME_DEFAULT_Y 480
#define TV_FRAME_DEFAULT_BPC 8
#define TV_FRAME_DEFAULT_RED_MASK   0xFF0000
#define TV_FRAME_DEFAULT_GREEN_MASK 0x00FF00
#define TV_FRAME_DEFAULT_BLUE_MASK  0x0000FF
#define TV_FRAME_DEFAULT_ALPHA_MASK 0

#define SET_MASK_OFFSET(x) (x << 8)
#define SET_MASK_LENGTH(x) (x)
#define GET_MASK_OFFSET(x) (x >> 8)
#define GET_MASK_LENGTH(x) (x & 0x00FF)

class tv_frame_video_t : public tv_frame_standard_t{
private:
	std::array<uint8_t, TV_FRAME_SIZE> frame = {{0}};
	uint16_t x_res = 0;
	uint16_t y_res = 0;
	uint8_t bpc = 0; // bits per component
	uint16_t red_mask = 0;
	uint16_t green_mask = 0;
	uint16_t blue_mask = 0;
	uint16_t alpha_mask = 0;
	uint64_t get_raw_pixel_pos(uint16_t x,
				   uint16_t y);
public:
	data_id_t id;
	tv_frame_video_t();
	~tv_frame_video_t();
	void set_pixel(uint16_t x_,
		       uint16_t y_,
		       std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color);
	std::tuple<uint64_t, uint64_t, uint64_t, uint8_t>
		get_pixel(uint16_t x_,
			  uint16_t y_);
	void get_masks(uint64_t *red_mask_,
		       uint64_t *green_mask_,
		       uint64_t *blue_mask_,
		       uint64_t *alpha_mask_,
		       uint8_t *bpc_);
	void get_res(uint16_t *x_res,
		     uint16_t *y_res);
	std::array<uint8_t, TV_FRAME_SIZE>* get_pixel_data_ptr();
	void set_all(uint16_t x_res_,
		     uint16_t y_res_,
		     uint8_t bpc_,
		     uint64_t red_mask_,
		     uint64_t green_mask_,
		     uint64_t blue_mask_,
		     uint64_t alpha_mask_);
	// too interdependent to have different setters
};

#endif
