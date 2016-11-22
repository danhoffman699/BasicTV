#include "tv_frame_standard.h"
#include "tv_frame_audio.h"

uint64_t tv_frame_audio_t::get_raw_sample_pos(uint8_t channel,
					      uint64_t pos){
	if(unlikely(bit_depth%8 != 0)){
		print("bit depth is not supported", P_SPAM);
	}
	const uint64_t channel_length =
		sampling_freq*(bit_depth/8);
	const uint64_t sample_pos =
		(channel_length*channel)+pos;
	// TV_FRAME_AUDIO_SIZE probably needs some major adjustingg
	if(unlikely(sample_pos >= TV_FRAME_AUDIO_SIZE)){
		print("ran out of runway in tv_frame_audio_t", P_ERR);
	}
	return audio[sample_pos];
}

void tv_frame_audio_t::get_type(uint64_t *sampling_freq_,
			   uint8_t *bit_depth_,
			   uint8_t *channel_count_){
	if(sampling_freq_ != nullptr){
		*sampling_freq_ = sampling_freq;
	}
	if(bit_depth_ != nullptr){
		*bit_depth_ = bit_depth;
	}
	if(channel_count_ != nullptr){
		*channel_count_ = channel_count;
	}
}

void tv_frame_audio_t::set_type(uint64_t sampling_freq_,
				uint8_t bit_depth_,
				uint8_t channel_count_){
	sampling_freq = sampling_freq_;
	bit_depth = bit_depth_;
	channel_count = channel_count_;
}

uint64_t tv_frame_audio_t::get_sample(uint8_t channel,
				      uint64_t sample_pos){
	return audio[get_raw_sample_pos(channel,
					 sample_pos)];
}

void tv_frame_audio_t::set_sample(uint8_t channel,
				  uint64_t sample_pos,
				  uint64_t data){
	audio[get_raw_sample_pos(channel,
				 sample_pos)] = data;
}

tv_frame_audio_t::tv_frame_audio_t() : id(this, __FUNCTION__){
}

tv_frame_audio_t::~tv_frame_audio_t(){
}
