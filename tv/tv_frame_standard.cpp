#include "tv_frame_standard.h"

tv_frame_standard_t::tv_frame_standard_t(){
}

tv_frame_standard_t::~tv_frame_standard_t(){
}

void tv_frame_standard_t::list_virtual_data(data_id_t *id){
	id->add_data(&ttl_micro_s, sizeof(ttl_micro_s));
	id->add_data(&frame_entry, sizeof(frame_entry));
}

void tv_frame_standard_t::set_standard(uint64_t start_time_micro_s_,
				       uint32_t ttl_micro_s_,
				       uint64_t frame_entry_){
	start_time_micro_s = start_time_micro_s_;
	ttl_micro_s = ttl_micro_s_;
	frame_entry = frame_entry_;
	// TODO:  check for invalid stuff
}

void tv_frame_standard_t::get_standard(uint64_t *start_time_micro_s_,
				       uint32_t *ttl_micro_s_,
				       uint64_t *frame_entry_){
	if(likely(start_time_micro_s_ != nullptr)){
		*start_time_micro_s_ = start_time_micro_s;
	}
	if(likely(ttl_micro_s_ != nullptr)){
		*ttl_micro_s_ = ttl_micro_s;
	}
	if(likely(frame_entry_ != nullptr)){
		*frame_entry_ = frame_entry;
	}
}

bool tv_frame_standard_t::valid(){
	const bool retval = BETWEEN(start_time_micro_s,
				    get_time_microseconds(),
				    start_time_micro_s+(uint64_t)ttl_micro_s);
	return retval;
}
