#ifndef TV_FRAME_VIDEO_H
#define TV_FRAME_VIDEO_H
#include "tv_frame_standard.h"

/*
  TODO: forget about video until AT LEAST data_id_t can import 
  std::vector<uint8_t>, because the size range is large enough to break my
  computer if it goes willy nilly
 */

#define TV_FRAME_FORMAT_UNDEFINED 0
#define TV_FRAME_FORMAT_RAW 1
#define TV_FRAME_FORMAT_VP9 2

// fl = flags, fo = format, same as audio
#define SET_TV_FRAME_FORMAT(fl, fo) (fl &= 0b00000011;fl |= (fo & 0b00000011);)
#define GET_TV_FRAME_FORMAT(fl) (fl & 0b00000011)

#define TV_FRAME_VIDEO_SIZE (1920*1080*4)

#define TV_FRAME_DEFAULT_X 640
#define TV_FRAME_DEFAULT_Y 480
#define TV_FRAME_DEFAULT_FORMAT TV_FRAME_FORMAT_RAW // VP9 hasn't been implemented yet
#define TV_FRAME_DEFAULT_BPC 8
#define TV_FRAME_DEFAULT_RED_MASK   0xFF0000
#define TV_FRAME_DEFAULT_GREEN_MASK 0x00FF00
#define TV_FRAME_DEFAULT_BLUE_MASK  0x0000FF
#define TV_FRAME_DEFAULT_ALPHA_MASK 0

// fancy 16 bit mask setup
#define SET_MASK_OFFSET(x) (x << 8)
#define SET_MASK_LENGTH(x) (x)
#define GET_MASK_OFFSET(x) (x >> 8)
#define GET_MASK_LENGTH(x) (x & 0x00FF)

/*
  tv_frame_video_t: frame of video

  This should be stored in either VP9 or raw frames. 

  If this were VP9:
  frame vector contains the raw data from VP9
  the standard frame dependency vector contains needed frames for decompression
  format is the compression format used

  If this were raw:
  frame vector contains the raw data that was written
  the standard frame dependency vector is empty
  format is the color encoding format used

  Since the primary use case for raw frames are menus, insanely small color
  depth values are going to be used:
  8-bit just because it fits nicely (maps to RGB values)
  4-bit for menus (maps to hex RGB values)
  1-bit for black and white (raw data visualization, Game of Life, etc)
*/

class tv_frame_video_t : public tv_frame_standard_t{
private:
	std::array<uint8_t, TV_FRAME_VIDEO_SIZE> frame = {{0}};
	uint16_t x_res = 0;
	uint16_t y_res = 0;
	uint8_t flags = 0; // format
	// only for uncompressed
	// if the format == raw
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
	std::array<uint8_t, TV_FRAME_VIDEO_SIZE>* get_pixel_data_ptr();
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
