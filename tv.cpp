#include "main.h"
#include "util.h"
#include "settings.h"
#include "file.h"
#include "tv.h"
#include "id.h"

tv_channel_t::tv_channel_t() : id(this, __FUNCTION__){
	id.add_data(&(public_key[0]), 1024); // stored in bytes
	id.add_data(&(frame_list_id[0]), 1024*sizeof(frame_list_id[0]));
	id.add_id(&(frame_list_id[0]), 1024*sizeof(frame_list_id[0]));
}

tv_channel_t::~tv_channel_t(){
	
}

tv_frame_t::tv_frame_t() : id(this, __FUNCTION__){
	id.add_data(&(frame[0]), sizeof(frame[0])*TV_FRAME_SIZE);
	id.add_data(&channel_id, sizeof(channel_id));
	id.add_id(&channel_id, sizeof(channel_id));
	id.add_data(&frame_number, sizeof(frame_number));
}

tv_frame_t::~tv_frame_t(){
}

tv_patch_t::tv_patch_t(uint64_t frame_id_) : id(this, __FUNCTION__){
	id.add_data(&(location[0]), 65536*sizeof(location[0]));
	id.add_data(&(value[0]), 65536*sizeof(value[0]));
	id.add_id(&frame_id, sizeof(frame_id));
	frame_id = frame_id_;
}

tv_patch_t::~tv_patch_t(){
}

void tv::init(){
}

void tv::loop(){
}

void tv::close(){
}
