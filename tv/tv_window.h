#include "../main.h"
#include "tv.h"
#ifndef TV_WINDOW_H
#define TV_WINDOW_H
#include "SDL2/SDL.h"
// get a better way to do this
#define TV_WINDOW_CT 0
#define TV_WINDOW_UL 1
#define TV_WINDOW_UR 2
#define TV_WINDOW_LL 2
#define TV_WINDOW_LR 3
/*
  A channel has to exist inside of a window at all times. When moving from
  a lower resolution to a higher resolution display, the top-left channel
  is always kept and the lower-right channels are cropped off or added upon.
  The position of the window is defined in an X and Y plane
 */

/*
  tv_window_t: window class for tv_channel_t. Doesn't actually do much, just
  exists to store the layout of the screen
 */

struct tv_window_t{
private:
	uint8_t pos = TV_WINDOW_CT;
	uint64_t channel_id = 0;
public:
	data_id_t id;
	tv_window_t();
	~tv_window_t();
	void set_pos(uint8_t pos_);
	uint8_t get_pos();
	uint64_t get_channel_id();
	void set_channel_id(uint64_t);
	// generated from the position
	uint64_t get_x_res();
	uint64_t get_y_res();
};

#endif
