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
#include "../id.h"
#include "tv.h"
#include "tv_channel.h"
#include "tv_frame.h"
#include "tv_patch.h"
#include "tv_window.h"

std::array<uint64_t, TV_WINDOW_SIZE> window = {{0}};
// channels, because of their network-ability, are created
// on the spot with ID functions




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
  Streaming is a very broad definition, but
  (set in the settings.cfg file, which hasn't been set up yet).
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
		data_id_t *channel_id =
			id_array::ptr(channel_id_list[i]);
		CONTINUE_IF_NULL(channel_id);
		tv_channel_t *channel =
			(tv_channel_t*)channel_id->get_ptr();
		CONTINUE_IF_NULL(channel);
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
	}
	return retval;
}
