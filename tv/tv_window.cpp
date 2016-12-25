#include "../main.h"
#include "../util.h"
#include "tv.h"
#include "tv_frame_standard.h"
#include "tv_frame_video.h"
#include "tv_frame_audio.h"
#include "tv_window.h"
#include "tv_channel.h"

tv_window_t::tv_window_t() : id(this, __FUNCTION__){
	id.add_data(&pos, sizeof(pos));
	id.add_data(&channel_id, sizeof(channel_id), ID_DATA_ID);
}

tv_window_t::~tv_window_t(){
}

uint64_t tv_window_t::get_channel_id(){
	return channel_id;
}

void tv_window_t::set_channel_id(uint64_t tmp){
	channel_id = tmp;
}

void tv_window_t::set_pos(uint8_t pos_){
	pos = pos_;
}

uint8_t tv_window_t::get_pos(){
	return pos;
}
