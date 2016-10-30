#include "../main.h"
#include "tv.h"
#ifndef TV_CHANNEL_H
#define TV_CHANNEL_H
/*
  tv_channel_t: channel information. contains information about the stream
  and frames (video only, audio only, etc.)

  TODO: possibly define behavior for when two streams have copied metadata
  but have different PGP keys (is this needed?)
 */

#define TV_CHAN_STREAMING (1 << 0)
#define TV_CHAN_NO_AUDIO (1 << 1)
#define TV_CHAN_NO_VIDEO (1 << 2)

struct tv_channel_t{
private:
	/*
	  Any frame ID will be valid here, but I would imagine opting
	  for the lowest frame would be the best option (increases
	  networkability, and prevents people from having to download
	  one frame to get the linked list for the desired frame on
	  a metered connection). The planned behavior is the lowest 
	  quality frame.
	 */
	uint64_t latest_frame_id = 0;
	// TODO: actually implement a TV Guide style system
	uint64_t latest_guide_id = 0;
	uint64_t status = 0;
public:
	data_id_t id;
	tv_channel_t();
	~tv_channel_t();
	bool is_streaming();
	bool is_audio();
	bool is_video();
};

#endif
