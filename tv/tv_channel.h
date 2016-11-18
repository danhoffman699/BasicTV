#include "../main.h"
#include "tv.h"
#ifndef TV_CHANNEL_H
#define TV_CHANNEL_H
/*
  tv_channel_t: channel information. contains information about the stream
  and frames (video only, audio only, etc.)
 */

#define TV_CHAN_FRAME_LIST_SIZE 256

/*
  For simplicity, we can do this. However, I'm thinking about creating
  a ternay system so 1.5 bits are used for three values instead of two.
  Great for expansion, but I can never get a round value
*/

#define TV_CHAN_STREAMING (1 << 0)
#define TV_CHAN_CHECKED_AUDIO (1 << 1)
#define TV_CHAN_AUDIO (1 << 2)
#define TV_CHAN_CHECKED_VIDEO (1 << 3)
#define TV_CHAN_VIDEO (1 << 4)
#define TV_CHAN_CHECKED_GUIDE (1 << 5)
#define TV_CHAN_GUIDE (1 << 6)

// When data is added to the stream_list, then just change the
// status to reflect the types of data that were added (assuming
// we have that information offhand)

struct tv_channel_t{
private:
	std::array<uint64_t, TV_CHAN_FRAME_LIST_SIZE> stream_list = {{0}};
	uint64_t status = 0;
public:
	data_id_t id;
	tv_channel_t();
	~tv_channel_t();
	uint64_t get_frame_id(uint64_t entry);
	void set_frame_id(uint64_t entry,
			   uint64_t stream_id);
	bool is_streaming();
	bool is_audio();
	bool is_video();
};

#endif
