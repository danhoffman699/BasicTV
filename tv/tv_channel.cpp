#include "../main.h"
#include "../util.h"
#include "tv_channel.h"
#include "tv_frame_standard.h"
#include "tv.h"

tv_channel_t::tv_channel_t() : id(this, __FUNCTION__){
}

tv_channel_t::~tv_channel_t(){
}

bool tv_channel_t::is_streaming(){
	return status & TV_CHAN_STREAMING;
}

bool tv_channel_t::is_audio(){
	return status & TV_CHAN_AUDIO;
}

bool tv_channel_t::is_video(){
	return status & TV_CHAN_VIDEO;
}

uint64_t tv_channel_t::get_frame_id(uint64_t entry){
	if(unlikely(entry >= TV_CHAN_FRAME_LIST_SIZE)){
		print("requested entry falls outside of bounds", P_WARN);
		return 0;
	}
	return stream_list[entry];
}

void tv_channel_t::set_frame_id(uint64_t entry, uint64_t value){
	if(unlikely(entry >= TV_CHAN_FRAME_LIST_SIZE)){
		print("requested entry falls outside of bounds", P_ERR);
	}
	stream_list[entry] = value;
}
