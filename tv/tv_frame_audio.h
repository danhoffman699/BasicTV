#ifndef TV_FRAME_AUDIO_H
#define TV_FRAME_AUDIO_H
#include "tv_frame_standard.h"

#define TV_FRAME_AUDIO_DEFAULT_SAMPLING_RATE 48000
#define TV_FRAME_AUDIO_DEFAULT_BIT_DEPTH 24
#define TV_FRAME_AUDIO_SAMPLES_LENGTH TV_FRAME_AUDIO_DEFAULT_SAMPLING_RATE*(TV_FRAME_AUDIO_DEFAULT_BIT_DEPTH/8)*TV_FRAME_DEFAULT_TTL

/*
  tv_frame_audio_t is defined as one channel of audio
 */

class tv_frame_audio_t : public tv_frame_standard_t{
private:
	// time information is stored in tv_frame_standard_t
	std::array<uint8_t, TV_FRAME_AUDIO_SAMPLES_LENGTH> samples;
	uint8_t bit_depth = 0;
	uint32_t sampling_freq = 0;
	/*
	  There is a TTL for tv_frame_standard_t, but having two different
	  variables, one for the original and one for the desired, can be
	  very useful, and I don't like the interdependency of information
	 */
	uint64_t length_micro_s = 0;
	uint64_t get_raw_sample_pos(uint64_t pos);
public:
	data_id_t id;
	tv_frame_audio_t();
	~tv_frame_audio_t();
	void get_type(uint64_t *sampling_freq_,
		      uint8_t *bit_depth_,
		      uint64_t *length_micro_s_);
	void set_type(uint64_t sampling_freq_,
		      uint8_t bit_depth_,
		      uint64_t length_micro_s_);
	uint64_t get_sample(uint64_t sample_pos);
	void set_sample(uint64_t sample_pos,
			uint64_t data);
};
#endif
