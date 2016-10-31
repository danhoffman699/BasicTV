#include "../main.h"
#include "../util.h"
#include "tv_channel.h"
#include "tv_frame.h"
#include "tv.h"

tv_channel_t::tv_channel_t() : id(this, __FUNCTION__){
}

tv_channel_t::~tv_channel_t(){
}

uint64_t tv_channel_t::get_latest_frame_id(){
	return latest_frame_id;
}

uint64_t tv_channel_t::get_latest_guide_id(){
	return latest_guide_id;
}

bool tv_channel_t::is_streaming(){
	return !!(status & TV_CHAN_STREAMING);
}

bool tv_channel_t::is_audio(){
	return !(status & TV_CHAN_NO_AUDIO);
}

bool tv_channel_t::is_video(){
	return !(status & TV_CHAN_NO_VIDEO);
}
