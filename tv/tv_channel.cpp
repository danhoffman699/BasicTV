#include "../main.h"
#include "../util.h"
#include "tv_channel.h"
#include "tv_frame.h"
#include "tv.h"

tv_channel_t::tv_channel_t() : id(this, __FUNCTION__){
}

tv_channel_t::~tv_channel_t(){
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
