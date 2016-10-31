/*
  tv.cpp
  This is responsible for the output of the stream to the screen. This does
  not cover inputs to the system, nor does it cover broadcasting streams to
  the network.

  The current tv_frame_t is rendered to the screen by loading the image
  into OpenGL and stretching it properly or by using SDL (SDL is probably
  better because of the GL ES thing with the RPi). All menus, text, etc. 
  are done using built-in libraries that I made that directly interface
  with tv_frame_t to display. That falls in sync with playing the audio
  from tv_frame_t (uncompressed and raw 16/24-bit). Channel ordering is
  done by alphabetizing PGP public keys, and there are reserved channels
  that the software runs for testing purposes, help screens, settings, etc.
  I plan to have a "test card" channel running with the sole intention of
  relaying traffic and helping debug problems (compression gives me no
  overhead, which is nice).

  Channels are identified by the PGP public key of their respective owner.
  Channel operators are not allowed to specify their own channel numbers
  because of collisions between channels.
 */

#include "../main.h"
#include "../util.h"
#include "../settings.h"
#include "../file.h"
#include "../id/id.h"
#include "tv.h"
#include "tv_channel.h"
#include "tv_frame.h"
#include "tv_patch.h"
#include "tv_window.h"

/*
  channel and window arrays are created OTF with id_array::all_of_type,
  an internal list of all types should be made to speed up the run time
 */

static void tv_render_all(){
	std::vector<uint64_t> all_windows =
		id_array::all_of_type("tv_window_t");
	for(uint64_t i = 0;i < all_windows.size();i++){
		tv_window_t *window =
			(tv_window_t*)id_array::ptr_data(all_windows[i]);
		if(window == nullptr){
			print("window is nullptr", P_ERR);
			continue;
		}
		tv_channel_t *channel =
			(tv_channel_t*)id_array::ptr_data(window->get_channel_id());
		if(channel == nullptr){
			print("channel is nullptr", P_ERR);
			continue;
		}
		tv_frame_t *frame =
			(tv_frame_t*)id_array::ptr_data(channel->get_latest_frame_id());
		if(frame == nullptr){
			print("frame is nullptr", P_ERR);
		}
		
	}
}

void tv_loop(){
}

void tv_init(){
	function_vector.push_back(tv_loop);
}

/*
  tv::chan: channel functions. Does not directly interface with the channels
  themselves, but does operations on the static list (count, next, prev, rand).

  flags:
  TV_CHAN_STREAMING: only use channels that are currently streaming. 
  Streaming is a very broad definition, but should be an independent
  variable inside of the tv_channel_t type
  TV_CHAN_NO_AUDIO: only video only streams
  TV_CHAN_NO_VIDEO: only audio only streams
*/

/*
  tv::chan::count: returns a channel count of all channels. Channels that are
  not currently streaming are co
 */

uint64_t tv::chan::count(uint64_t flags){
	uint64_t retval = 0;
	std::vector<uint64_t> channel_id_list =
		id_array::all_of_type("tv_channel_t");
	for(uint64_t i = 0;i < channel_id_list.size();i++){
		try{
			tv_channel_t *channel =
				(tv_channel_t*)id_array::ptr_data(channel_id_list[i]);
			if((flags & TV_CHAN_STREAMING) && channel->is_streaming()){
				retval++;
				continue;
			}
			if((flags & TV_CHAN_NO_AUDIO) && !channel->is_audio()){
				retval++;
				continue;
			}
			if((flags & TV_CHAN_NO_VIDEO) && !channel->is_video()){
				retval++;
				continue;
			}
		}catch(std::runtime_error e){
			continue;
		}
	}
	return retval;
}

/*
  TODO: implement actual sorting of channels on the fly. Possibly
  embed that sorting into the PGP API?
*/

uint64_t tv::chan::next(uint64_t id, uint64_t flags){
	std::vector<uint64_t> all_channels =
		id_array::all_of_type("tv_channel_t");
	for(uint64_t i = 0;i < all_channels.size();i++){
		tv_channel_t *channel = (tv_channel_t*)id_array::ptr_data(all_channels[i]);
		if(channel == nullptr){
			continue;
		}
		const bool streaming =
			channel->is_streaming() == (flags & TV_CHAN_STREAMING);
		const bool audio =
			(!channel->is_audio()) == (flags & TV_CHAN_NO_AUDIO);
		const bool video =
			(!channel->is_video()) == (flags & TV_CHAN_NO_VIDEO);
		// checks for matches, not actual true statements
		if(!(streaming && audio && video)){
			all_channels.erase(all_channels.begin()+i);
			i--;
		}
	}
	const std::vector<uint64_t> pgp_sorted =
		id_array::sort_by_pgp_pubkey(
		        all_channels);
	for(uint64_t i = 0;i < pgp_sorted.size();i++){
		if(pgp_sorted[i] == id){
			if(i != pgp_sorted.size()-2){
				return pgp_sorted[i+1];
			}
		}
	}
	return 0;
}

uint64_t tv::chan::prev(uint64_t id, uint64_t flags){
	std::vector<uint64_t> all_channels =
		id_array::all_of_type("tv_channel_t");
	for(uint64_t i = 0;i < all_channels.size();i++){
		tv_channel_t *channel = (tv_channel_t*)id_array::ptr_data(all_channels[i]);
		if(channel == nullptr){
			continue;
		}
		const bool streaming =
			channel->is_streaming() == (flags & TV_CHAN_STREAMING);
		const bool audio =
			(!channel->is_audio()) == (flags & TV_CHAN_NO_AUDIO);
		const bool video =
			(!channel->is_video()) == (flags & TV_CHAN_NO_VIDEO);
		// checks for matches, not actual true statements
		if(!(streaming && audio && video)){
			all_channels.erase(all_channels.begin()+i);
			i--;
		}
	}
	const std::vector<uint64_t> pgp_sorted =
		id_array::sort_by_pgp_pubkey(
		        all_channels);
	for(uint64_t i = 0;i < pgp_sorted.size();i++){
		if(pgp_sorted[i] == id){
			if(i != 1){
				return pgp_sorted[i-1];
			}
		}
	}
	return 0;
}

uint64_t tv::chan::rand(uint64_t flags){
	uint64_t channel_count = count(flags);
	uint64_t id_from_start = true_rand(0, channel_count-1);
	std::vector<uint64_t> channel_id =
		id_array::all_of_type("tv_channel_t");
	return channel_id.at(id_from_start);
}
