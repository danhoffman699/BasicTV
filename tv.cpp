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
  because of collisions between channels. I don't really know what 
  should happen here.
 */

#include "main.h"
#include "util.h"
#include "settings.h"
#include "file.h"
#include "tv.h"
#include "id.h"

std::array<tv_window_t, TV_WINDOW_SIZE> window;
std::array<tv_channel_t, TV_CHANNEL_SIZE> window;

tv_channel_t::tv_channel_t() : id(this, __FUNCTION__){
	id.add_data(&(public_key[0]), PGP_PUBKEY_SIZE); // stored in bytes
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

void tv_init(){
	function_vector.push_back(tv::loop);
}
