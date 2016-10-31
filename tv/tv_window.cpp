#include "../main.h"
#include "../util.h"
#include "tv.h"
#include "tv_frame.h"
#include "tv_window.h"
#include "tv_channel.h"
#include "tv_patch.h"

tv_window_t::tv_window_t() : id(this, __FUNCTION__){
	id.add_data(&x_pos, sizeof(x_pos));
	id.add_data(&y_pos, sizeof(y_pos));
	id.add_data(&channel_id, sizeof(channel_id));
	id.add_id(&channel_id, sizeof(channel_id));
}

tv_window_t::~tv_window_t(){
}

uint64_t tv_window_t::get_channel_id(){
	return channel_id;
}

void tv_window_t::set_channel_id(uint64_t tmp){
	channel_id = tmp;
}
