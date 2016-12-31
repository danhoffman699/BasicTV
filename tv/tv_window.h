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
  tv_window_t: window class for tv_channel_t. This is REQUIRED for any currently
  viewed stream, because it defines the point in time which all streams on the
  channel must operate (timestamp_offset).

  Timestamp offset is typically set to the standard broadcast delay of ten
  seconds. This is useful, since most timestamps for streams are set when they
  leave the first computer, not allowing any sensible time to permeate the
  network. Smaller values can work fine, but I would expect rough playback

  Channels, when using a smaller timestamp offset, might register as flickering
  between live and not live, since the only difference is if the next ID is
  found on the network (typically a good measure, assuming we have enough nodes
  relaying the data). If the next ID is not found on the network, then it is
  assumed that it doesn't exist at all (it should have some time to permeate the
  network)

  Timestamp offset is signed, because there can be cases where looking forward
  into future events can be possible (specifically, releasing information early
  that is encrypted). However, this doesn't make sense currently. If you want to
  try it, go on ahead. 
 */

struct tv_window_t{
private:
	uint8_t pos = TV_WINDOW_CT;
	int64_t timestamp_offset = 0;
	uint64_t channel_id = 0;
	/*
	  I like the idea of using entries instead of IDs (lookup overhead), but
	  resizing the vector ruins that
	 */
	std::vector<id_t_> active_streams;
public:
	data_id_t id;
	tv_window_t();
	~tv_window_t();
	void set_pos(uint8_t pos_);
	uint8_t get_pos();
	void set_channel_id(uint64_t channel_id_);
	uint64_t get_channel_id();
	void set_timestamp_offset(int64_t timestamp_offset_);
	int64_t get_timestamp_offset(){return timestamp_offset;}
	// generated from the position
	void add_active_stream_id(id_t_ id_);
	void del_active_stream_id(id_t_ id_);
	std::vector<id_t_> get_active_streams(){return active_streams;}
	uint64_t get_x_res();
	uint64_t get_y_res();
};

#endif
