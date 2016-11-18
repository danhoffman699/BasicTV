#ifndef TV_FRAME_AUDIO_H
#define TV_FRAME_AUDIO_H
#include "tv_frame_standard.h"

#define TV_FRAME_DEFAULT_SAMPLING_RATE 11025
#define TV_FRAME_DEFAULT_CHANNEL_COUNT 1
#define TV_FRAME_DEFAULT_AMP_DEPTH 2 // bytes, so 16-bit

class tv_frame_audio_t : public tv_frame_standard_t{
private:
	std::array<uint8_t, TV_FRAME_AUDIO_SIZE> audio = {{0}};
	uint8_t amp_depth = 0;
	uint32_t sampling_freq = 0;
	uint8_t channel_count = 0;
public:
	data_id_t id;
	tv_frame_audio_t();
	~tv_frame_audio_t();
	// TODO: actually bother with audio
};
#endif
