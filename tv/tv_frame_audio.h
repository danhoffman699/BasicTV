#ifndef TV_FRAME_AUDIO_H
#define TV_FRAME_AUDIO_H
#include "tv_frame_standard.h"

// 7.1 surround sound at 48kHz sounds good enough

#define TV_FRAME_DEFAULT_SAMPLING_RATE 48000
#define TV_FRAME_DEFAULT_CHANNEL_COUNT 8
#define TV_FRAME_DEFAULT_BIT_DEPTH 16
// running at 10hz as a minimum
#define TV_FRAME_AUDIO_SIZE (TV_FRAME_DEFAULT_SAMPLING_RATE*TV_FRAME_DEFAULT_CHANNEL_COUNT/10)


class tv_frame_audio_t : public tv_frame_standard_t{
private:
	std::array<uint8_t, TV_FRAME_AUDIO_SIZE> audio = {{0}};
	uint8_t bit_depth = 0;
	uint32_t sampling_freq = 0;
	uint8_t channel_count = 0;
	// checks bounds
	uint64_t get_raw_sample_pos(uint8_t channel,
				    uint64_t pos);
public:
	data_id_t id;
	tv_frame_audio_t();
	~tv_frame_audio_t();
	void get_type(uint64_t *sampling_freq_,
		      uint8_t *bit_depth_,
		      uint8_t *channel_count_);
	void set_type(uint64_t sampling_freq_,
		      uint8_t bit_depth_,
		      uint8_t channel_count_);
	uint64_t get_sample(uint8_t channel,
			    uint64_t sample_pos);
	void set_sample(uint8_t channel,
			uint64_t sample_pos,
			uint64_t data);
};
#endif
