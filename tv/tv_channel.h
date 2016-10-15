#include "../main.h"
#include "tv.h"
#ifndef TV_CHANNEL_H
#define TV_CHANNEL_H
/*
  tv_channel_t: channel information. contains information about the stream
  and frames (video only, audio only, etc.)
 */

#define TV_CHAN_STREAMING (1 << 0)
#define TV_CHAN_NO_AUDIO (1 << 1)
#define TV_CHAN_NO_VIDEO (1 << 2)

struct tv_channel_t{
private:
	std::array<uint8_t, PGP_PUBKEY_SIZE> pubkey = {{0}};
	std::array<uint64_t, TV_FRAME_RETAIN_SIZE> frame_list_id = {{0}};
	uint64_t status = 0;
public:
	data_id_t id;
	tv_channel_t();
	~tv_channel_t();
	uint64_t get_frame_id(uint64_t backstep);
	void set_frame_id(uint64_t id, uint64_t backstep);
	bool is_streaming();
	bool is_audio();
	bool is_video();
};

#endif
