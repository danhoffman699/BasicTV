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

// no reason why these are needed
#define TV_CHAN_PLAYBACK_SPEED_PAUSE 0
#define TV_CHAN_PLAYBACK_SPEED_PLAY 1

// When data is added to the stream_list, then just change the
// status to reflect the types of data that were added (assuming
// we have that information offhand)

/*
  There are actually two different types of "broadcast delay":

  tv_channel_t broadcast delay, typically in the seconds, which is
  meant to ensure that enough data has been received to recreate the
  stream in part. If this is too low, then blank screens, choppy audio,
  and out of sync AV might happen. However, that variable should be set
  dynamically to match the network, so no two sets are guaranteed to be
  perfectly in-sync with each other. 

  Encrypted broadcast delay: the ID is encrypted with a second
  key that is sent out a certain time before the live time for
  that set of frames. Encrypting a minute of frames with one key,
  and sending the key out a minute in advance seems fair. 

  People can press a record button and the information for that slot will
  be received from the network. Of course, if the event is happening
  live, this isn't practical.
 */

struct tv_channel_t{
private:
	std::array<uint64_t, TV_CHAN_FRAME_LIST_SIZE> stream_list = {{0}};
	uint64_t status = 0;
	uint64_t broadcast_delay_micro_s = 0;
	uint64_t pivot_time_micro_s = 0;
	int64_t playback_speed = 0;
public:
	data_id_t id;
	tv_channel_t();
	~tv_channel_t();
	uint64_t get_frame_id(uint64_t entry);
	void set_frame_id(uint64_t entry,
			  uint64_t stream_id);
	uint64_t get_broadcast_delay_micro_s();
	void set_broadcast_delay_micro_s(uint64_t broadcast_delay_micro_s_);
	bool is_streaming();
	bool is_audio();
	bool is_video();
	void set_playback_speed(int64_t playback_speed);
	void set_pivot_time_micro_s(uint64_t pivot_time_micro_s_);
	uint64_t get_playback_speed();
};

#endif
