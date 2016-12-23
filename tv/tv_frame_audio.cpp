#include "tv_frame_standard.h"
#include "tv_frame_audio.h"

uint64_t tv_frame_audio_t::get_sample_pos(uint64_t pos){
	const uint8_t byte_depth = bit_depth/8;
	const uint64_t sample_pos = pos*byte_depth;
	uint64_t retval = 0;
	if(GET_TV_FRAME_AUDIO_FORMAT(flags) == TV_FRAME_AUDIO_FORMAT_RAW){
		if(unlikely(sample_pos+byte_depth >= TV_FRAME_AUDIO_DATA_SIZE)){
			print("requesting entry for out-of-bounds sample", P_ERR);
		}
		for(uint8_t i = 0;i < byte_depth;i++){
			retval |= data[sample_pos+i] << (i*8);
		}
	}else{
		if(unlikely(sample_pos+byte_depth >= samples.size())){
			print("requesting entry for out-of-bounds sample", P_ERR);
		}
		for(uint8_t i = 0;i < byte_depth;i++){
			retval |= samples[sample_pos+i] << (i*8);
		}
	}
	return retval;
}

void tv_frame_audio_t::get_type(uint64_t *sampling_freq_,
				uint8_t *bit_depth_,
				uint8_t *flags_){
	if(sampling_freq_ != nullptr){
		*sampling_freq_ = sampling_freq;
	}
	if(bit_depth_ != nullptr){
		*bit_depth_ = bit_depth;
	}
	if(flags_ != nullptr){
		*flags_ = flags;
	}
}

void tv_frame_audio_t::set_type(uint64_t sampling_freq_,
				uint8_t bit_depth_,
				uint8_t flags_){
	if(unlikely(bit_depth_%8 != 0)){
		print("bit depth is not supported", P_ERR);
	}
	if(unlikely(bit_depth_ >= 64)){
		print("bit depth is too large for variable size", P_ERR);
	}
	// no checks for flags needed
	sampling_freq = sampling_freq_;
	bit_depth = bit_depth_;
	flags = flags_;
}

std::vector<uint8_t> tv_frame_audio_t::get_samples(){
	switch(GET_TV_FRAME_AUDIO_FORMAT(flags)){
	case TV_FRAME_AUDIO_FORMAT_RAW:
		return std::vector<uint8_t>(data.begin(), data.end());
		break;
	case TV_FRAME_AUDIO_FORMAT_UNDEFINED:
	case TV_FRAME_AUDIO_FORMAT_OPUS:
	case TV_FRAME_AUDIO_FORMAT_FLAC:
	default:
		print("unsupported format, cannot decode", P_ERR);
		break;
	}
	return {};
}
		
void tv_frame_audio_t::set_samples(std::vector<uint8_t> samples){
}

tv_frame_audio_t::tv_frame_audio_t() : id(this, __FUNCTION__){
	list_virtual_data(&id);
	ADD_DATA_ARRAY(data,
		       TV_FRAME_AUDIO_DATA_SIZE,
		       1);
	ADD_DATA(bit_depth);
	ADD_DATA(sampling_freq);
	ADD_DATA(flags);
	// sampes is cache now
}

tv_frame_audio_t::~tv_frame_audio_t(){
}
