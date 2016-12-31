#include "../main.h"
#include "../util.h"
#include "tv_channel.h"
#include "tv_frame_standard.h"
#include "tv.h"

tv_channel_t::tv_channel_t() : id(this, __FUNCTION__){
	ADD_DATA_ARRAY(stream_list,
		       TV_CHAN_FRAME_LIST_SIZE,
		       8); // uint64_t
	ADD_DATA_NONET(broadcast_delay_micro_s);
	ADD_DATA(status);
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

std::vector<id_t_> tv_channel_t::get_stream_list(){
	return std::vector<id_t_>(
		stream_list.begin(),
		stream_list.end());
}

void tv_channel_t::add_stream_id(id_t_ id_){
	del_stream_id(id_);
	stream_list.push_back(id_);
}

void tv_channel_t::del_stream_id(id_t_ id_){
	for(uint64_t i = 0;i < stream_list.size();i++){
		if(stream_list[i] == id_){
			stream_list.erase(
				stream_list.begin()+i);
			// don't break until I know everything works fine
		}
	}
}

uint64_t tv_channel_t::get_broadcast_delay_micro_s(){
	const uint64_t curr_time =
		get_time_microseconds();
	const long double playback_mul =
		playback_speed/(long double)100.0;
	const uint64_t pivot_time = 
		pivot_time_micro_s; // possibly make a private getter?
	return pivot_time+((curr_time-pivot_time)*playback_mul);
}

// I see no reason why playback speed is set and pivot time isn't reset,
// outside of it just being broken or stupid.

void tv_channel_t::set_playback_speed(int64_t playback_speed_){
	playback_speed = playback_speed_;
	set_pivot_time_micro_s(get_time_microseconds());
}

void tv_channel_t::set_pivot_time_micro_s(uint64_t pivot_time_micro_s_){
	pivot_time_micro_s = pivot_time_micro_s_;
}
