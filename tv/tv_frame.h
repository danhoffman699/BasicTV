#include "../main.h"
#include "tv.h"
#include "../convert.h"
#ifndef TV_FRAME_H
#define TV_FRAME_H

/*
  Default TV frame properties. Not based on any one standard. Set intentionally
  low so it can more easily spread across the network. This shouldn't be used by
  standard streams, and should instead use custom set variables in the settings
  files or the raw stream settings (lossless). However, this is the standsrd res
  for menus and other things

  TODO: raise this when the compression is good enough all around
 */
#define TV_FRAME_DEFAULT_X 640
#define TV_FRAME_DEFAULT_Y 480
#define TV_FRAME_DEFAULT_COLOR_DEPTH 2
#define TV_FRAME_DEFAULT_REFRESH_RATE 30
// in microseconds
#define TV_FRAME_DEFAULT_TIME_TO_LIVE ((1/MICRO_PREFIX)/TV_FRAME_DEFAULT_REFRESH_RATE)
#define TV_FRAME_DEFAULT_SAMPLING_RATE 11025
#define TV_FRAME_DEFAULT_CHANNEL_COUNT 1
#define TV_FRAME_DEFAULT_AMP_DEPTH 2 // bytes, so 16-bit

/*
  tv_frame_t: individual frame with all of the settings as requested from 
  whatever data type. This can be sent with another layer of encryption, 
  whose key will be sent out later, for "cacheing" live TV so television
  patterns can be recognized and channels that are typically watched at
  certain times can pre-fetch higher quality versions than the internet
  can handle (8K over dial-up). Of course, the lowest quality stream would
  be downloaded first as a sample, but the higher quality version will
  be downloaded afterwards.

  TODO: crunch some numbers to see how far in advance data needs to be
  uploaded to get certain resolution streams over 56.6K (round down to 
  50K, actually, because of other data, overhead, and redundancy).
 */

struct tv_frame_t{
private:
	// raw information, parsed using following information
	std::array<uint8_t, TV_FRAME_SIZE> frame = {{0}};
	std::array<uint8_t, TV_FRAME_AUDIO_SIZE> audio = {{0}};
	// channel ID
	uint64_t channel_id = 0;
	// frame_number, increment by one every time.
	uint64_t frame_number = 0;
	// pretty basic settings
	uint64_t x_res = 0;
	uint64_t y_res = 0;
	uint8_t bpc = 0; // bits per color
	uint8_t amp_depth = 0; // amplitude, bit depth (in bytes)
	// in Hz
	uint64_t sampling_freq = 0;
	// time to live in microseconds (easier for dynamic refresh rate)
	uint64_t time_to_live = 0;
	// channel count (audio)
	uint8_t channel_count = 1;
	// unix timestamp of latest modification
	uint64_t unix_timestamp = 0;
	// second layer of encryption, used for "cacheing" data
	uint64_t second_cite_id = 0;
	uint64_t convert_to_raw_pixel(std::array<uint64_t, 3> tmp, uint8_t);
	void write_raw_pixel(uint64_t x, uint64_t y, uint64_t);
	uint64_t read_raw_pixel(uint64_t x, uint64_t y);
public:
	data_id_t id;
	tv_frame_t();
	~tv_frame_t();
	/*
	  most settings cannot be changed OTF
	 */
	void reset(uint64_t x = TV_FRAME_DEFAULT_X,
		   uint64_t y = TV_FRAME_DEFAULT_Y,
		   uint8_t color_depth_ = TV_FRAME_DEFAULT_COLOR_DEPTH,
		   uint64_t time_to_live_ = TV_FRAME_DEFAULT_TIME_TO_LIVE,
		   uint64_t sampling_rate_ = TV_FRAME_DEFAULT_SAMPLING_RATE,
		   uint8_t channel_count_ = TV_FRAME_DEFAULT_CHANNEL_COUNT,
		   uint8_t amp_depth_ = TV_FRAME_DEFAULT_AMP_DEPTH);
	void set_pixel(uint64_t x, uint64_t y, std::array<uint64_t, 3> raw_colors, uint8_t raw_bpc);
	uint64_t get_frame_number();
	uint64_t get_frame_id_prev();
	uint64_t get_frame_id_next();
	uint64_t get_pixel(uint64_t x, uint64_t y, uint8_t bpc_tmp);
	uint64_t get_x_res();
	uint64_t get_y_res();
	uint64_t get_timestamp();
};

#endif
