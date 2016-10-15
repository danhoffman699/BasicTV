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

#include "main.h"
#include "util.h"
#include "settings.h"
#include "file.h"
#include "tv.h"
#include "id.h"

std::array<uint64_t, TV_WINDOW_SIZE> window = {{0}};
// channels, because of their network-ability, are created
// on the spot with ID functions


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

tv_frame_t::tv_frame_t() : id(this, __FUNCTION__){
	id.add_data(&(frame[0]), sizeof(frame[0])*TV_FRAME_SIZE);
	id.add_data(&channel_id, sizeof(channel_id));
	id.add_id(&channel_id, sizeof(channel_id));
	id.add_data(&frame_number, sizeof(frame_number));
	id.add_data(&unix_timestamp, sizeof(unix_timestamp));
}

tv_frame_t::~tv_frame_t(){
}

/*
  NOTE:
  In order to vary the color depths for slower connections, there are actually
  three frames, each of them containing specific color data. The actual 
  implementation does the following checks (# of frames are independently 
  stored from the frame array, a black screen would be pretty bad):
  1 frame available: 8-bit color depth, use lookup table to make better use of
  the odd number system (two bits for a color is comically bad and wasteful)
  2 frames available: 16-bit color depth, first 8-bits (0xFF00) are the start
  and the next 8-bits (0x00FF) are the last. Five bits per color.
  3 frames available: 24-bit color depth, keep the order standardized. Eight
  bits per color
 */

/*
  TODO: implement
 */

void tv_frame_t::set_pixel(uint64_t x, uint64_t y, uint64_t value){
	
}

// frame only uses uint8_t for compression, uint64_t is used
// for internal use

uint64_t tv_frame_t::get_pixel(uint64_t x, uint64_t y){
}

uint64_t tv_frame_t::get_timestamp(){
	return unix_timestamp;
}

uint64_t tv_frame_t::get_x_res(){
	return x_res;
}

uint64_t tv_frame_t::get_y_res(){
	return y_res;
}

tv_patch_t::tv_patch_t() : id(this, __FUNCTION__){
	id.add_data(&(location[0]), 65536*sizeof(location[0]));
	id.add_data(&(value[0]), 65536*sizeof(value[0]));
	id.add_id(&frame_id, sizeof(frame_id));
}

tv_patch_t::~tv_patch_t(){
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
