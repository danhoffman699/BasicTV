#include "tv_api.h"
#include "tv_channel.h"
#include "tv_frame.h"
#include "tv_window.h"
#include "tv_patch.h"
#include "tv_menu.h"
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
		id_api::cache::get("tv_channel_t");
	for(uint64_t i = 0;i < channel_id_list.size();i++){
		try{
			tv_channel_t *channel =
				PTR_DATA(channel_id_list[i], tv_channel_t);
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
		id_api::cache::get("tv_channel_t");
	for(uint64_t i = 0;i < all_channels.size();i++){
		tv_channel_t *channel = PTR_DATA(all_channels[i], tv_channel_t);
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
		id_api::array::sort_by_pgp_pubkey(
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
		id_api::cache::get("tv_channel_t");
	for(uint64_t i = 0;i < all_channels.size();i++){
		tv_channel_t *channel =
			PTR_DATA(all_channels[i], tv_channel_t);
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
		id_api::array::sort_by_pgp_pubkey(
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
		id_api::cache::get("tv_channel_t");
	return channel_id.at(id_from_start);
}

