#include "tv_api.h"
#include "tv_channel.h"
#include "tv_frame_standard.h"
#include "tv_frame_video.h"
#include "tv_frame_audio.h"
#include "tv_window.h"
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
	std::vector<id_t_> channel_id_list =
		id_api::cache::get("tv_channel_t");
	for(uint64_t i = 0;i < channel_id_list.size();i++){
		try{
			tv_channel_t *channel =
				PTR_DATA(channel_id_list[i], tv_channel_t);
			if(flags & TV_CHAN_STREAMING && channel->is_streaming()){
				retval++;
				continue;
			}
			if((flags & TV_CHAN_AUDIO) && channel->is_audio()){
				retval++;
				continue;
			}
			if((flags & TV_CHAN_VIDEO) && channel->is_video()){
				retval++;
				continue;
			}
		}catch(std::runtime_error e){
			continue;
		}
	}
	return retval;
}

id_t_ tv::chan::next_id(id_t_ id, uint64_t flags){
	std::vector<id_t_> all_channels =
		id_api::cache::get("tv_channel_t");
	id_t_ retval = ID_BLANK_ID;
	bool swapped_pos = true;
	while(swapped_pos){
		swapped_pos = false;
		for(uint64_t i = 0;i < all_channels.size();i++){
			/*
			  Sort by encryption fingerprint, get the next
			 */
		}
	}
	return retval;
	
}

id_t_ tv::chan::prev_id(id_t_ id, uint64_t flags){
	std::vector<id_t_> all_channels =
		id_api::cache::get("tv_channel_t");
	return id;
}

id_t_ tv::chan::rand_id(uint64_t flags){
	uint64_t channel_count = count(flags);
	uint64_t id_from_start = true_rand(0, channel_count-1);
	std::vector<id_t_> channel_id =
		id_api::cache::get("tv_channel_t");
	return channel_id.at(id_from_start);
}

