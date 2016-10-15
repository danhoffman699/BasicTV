#include "../main.h"
#include "../util.h"
#include "tv_channel.h"
#include "tv_frame.h"
#include "tv.h"

tv_channel_t::tv_channel_t() : id(this, __FUNCTION__){
	id.add_data(&(pubkey[0]), PGP_PUBKEY_SIZE); // stored in bytes
	id.add_data(&(frame_list_id[0]), 1024*sizeof(frame_list_id[0]));
	id.add_id(&(frame_list_id[0]), 1024*sizeof(frame_list_id[0]));
}

tv_channel_t::~tv_channel_t(){
}

uint64_t tv_channel_t::get_frame_id(uint64_t backstep){
	if(backstep > TV_FRAME_RETAIN_SIZE){
		throw std::runtime_error("backstep > retension buffer");
	}
	return frame_list_id[TV_FRAME_RETAIN_SIZE-backstep-1];
}

void tv_channel_t::set_frame_id(uint64_t id, uint64_t backstep){
	frame_list_id[TV_FRAME_RETAIN_SIZE-backstep-1] = id;
}

bool tv_channel_t::is_streaming(){
	data_id_t *frame_id_ptr = id_array::ptr(frame_list_id[0]);
	if(frame_id_ptr == nullptr){
		return false;
	}
	tv_frame_t *frame_ptr =
		(tv_frame_t*)frame_id_ptr->get_ptr();
	if(frame_ptr == nullptr){
		return false;
	}
	uint8_t timestamp_diff =
		(frame_ptr->get_timestamp()-std::time(nullptr));
	if(timestamp_diff > 1){
		print("frame is in the future?", P_WARN);
		// keep timestamps within check so a channel can't
		// start making frames way too far ahead to keep
		// themselves streaming
		// TODO: create a spam filter and have this as one
		// of the triggers (with a lot of others)
		return false;
	}
	return true;
}

bool tv_channel_t::is_audio(){
	return !(status & TV_CHAN_NO_AUDIO);
}

bool tv_channel_t::is_video(){
	return !(status & TV_CHAN_NO_VIDEO);
}
