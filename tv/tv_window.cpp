#include "../main.h"
#include "../util.h"
#include "tv.h"
#include "tv_frame.h"
#include "tv_window.h"
#include "tv_channel.h"
#include "tv_patch.h"

tv_window_t::tv_window_t() : id(this, __FUNCTION__){
	id.add_data(&x_res, sizeof(x_res));
	id.add_data(&y_res, sizeof(y_res));
	id.add_data(&x_pos, sizeof(x_pos));
	id.add_data(&y_pos, sizeof(y_pos));
	id.add_data(&channel_id, sizeof(channel_id));
	id.add_id(&channel_id, sizeof(channel_id));
	x_res = 640;
	y_res = 480;
}

tv_window_t::~tv_window_t(){
}

uint64_t tv_window_t::get_channel_id(){
	return channel_id;
}

void tv_window_t::set_channel_id(uint64_t tmp){
	channel_id = tmp;
}

uint64_t tv_window_t::get_x_pos(){
	return x_pos;
}

uint64_t tv_window_t::get_y_pos(){
	return y_pos;
}

uint64_t tv_window_t::get_x_res(){
	return x_res;
}

uint64_t tv_window_t::get_y_res(){
	return y_res;
}
