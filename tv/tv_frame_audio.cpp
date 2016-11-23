#include "tv_frame_standard.h"
#include "tv_frame_audio.h"

uint64_t tv_frame_audio_t::get_raw_sample_pos(uint64_t pos){
	const uint8_t byte_depth = bit_depth/8;
	const uint64_t sample_pos = pos*byte_depth;
	if(unlikely(sample_pos+byte_depth >= TV_FRAME_AUDIO_SAMPLES_LENGTH)){
		print("requesting entry for out-of-bounds sample", P_ERR);
	}
	uint64_t retval = 0;
	for(uint8_t i = 0;i < byte_depth;i++){
		retval |= samples[sample_pos+i] << (i*8);
	}
	return retval;
}

void tv_frame_audio_t::get_type(uint64_t *sampling_freq_,
				uint8_t *bit_depth_,
				uint64_t *length_micro_s_){
	if(sampling_freq_ != nullptr){
		*sampling_freq_ = sampling_freq;
	}
	if(bit_depth_ != nullptr){
		*bit_depth_ = bit_depth;
	}
	if(length_micro_s_ != nullptr){
		*length_micro_s_ = length_micro_s;
	}
}

void tv_frame_audio_t::set_type(uint64_t sampling_freq_,
				uint8_t bit_depth_,
				uint64_t length_micro_s_){
	if(unlikely(bit_depth_%8 != 0)){
		print("bit depth is not supported", P_ERR);
	}
	if(unlikely(bit_depth_ >= 64)){
		print("bit depth is too large for variable size", P_ERR);
	}
	if(unlikely((length_micro_s_/1000000)*sampling_freq_ > TV_FRAME_AUDIO_SAMPLES_LENGTH)){
		print("buffer requested is too large for audio frame", P_ERR);
	}
	sampling_freq = sampling_freq_;
	bit_depth = bit_depth_;
	length_micro_s = length_micro_s_;
}

uint64_t tv_frame_audio_t::get_sample(uint64_t sample_pos){
	return samples[get_raw_sample_pos(sample_pos)];
}

void tv_frame_audio_t::set_sample(uint64_t sample_pos,
				  uint64_t data){
	samples[get_raw_sample_pos(sample_pos)] = data;
}

tv_frame_audio_t::tv_frame_audio_t() : id(this, __FUNCTION__){
	list_virtual_data(&id);
	ADD_DATA_ARRAY(samples,
		       TV_FRAME_AUDIO_SAMPLES_LENGTH,
		       1);
}

tv_frame_audio_t::~tv_frame_audio_t(){
}
