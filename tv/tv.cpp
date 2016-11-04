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
#include "../id/id_api.h"
#include "tv.h"
#include "tv_channel.h"
#include "tv_frame.h"
#include "tv_patch.h"
#include "tv_window.h"

/*
  TODO: clean up all of this code. I made it in a hurry to use as proof
  that it all works fine. Most of what is here doesn't break anything
  low level or would otherwise need large fixes, but it just needs
  to be cleaned up a bit (and tv_frame as well)
 */

/*
  channel and window arrays are created OTF with id_api::cache::get
 */

static SDL_Window *window = nullptr;
static SDL_Surface *window_surface = nullptr;

// this surface should fit the dimensions of the frame, let SDL
// handle the resizing (at least for now)

static SDL_Surface *tv_render_frame_to_surface(tv_frame_t *frame){
	const uint64_t width = frame->get_x_res();
	const uint64_t height = frame->get_y_res();
	const uint8_t depth = frame->get_bpc()*4; // includes alpha
	const uint64_t red_mask = frame->get_red_mask();
	const uint64_t green_mask = frame->get_green_mask();
	const uint64_t blue_mask = frame->get_blue_mask();	
	const uint64_t alpha_mask = frame->get_alpha_mask();
	P_V_S(convert::number::to_binary(red_mask), P_SPAM);
	P_V_S(convert::number::to_binary(green_mask), P_SPAM);
	P_V_S(convert::number::to_binary(blue_mask), P_SPAM);
	P_V_S(convert::number::to_binary(alpha_mask), P_SPAM);
	P_V(depth, P_SPAM);
	SDL_Surface *surface =
		SDL_CreateRGBSurface(0,
				     width,
				     height,
				     depth,
				     0,
				     0,
				     0,
				     0);
	if(surface == nullptr){
		print((std::string)"surface is nullptr:" + SDL_GetError(), P_ERR);
	}
	if(surface->format->BytesPerPixel != 4){
		P_V(surface->format->BytesPerPixel, P_WARN);
		print("surface created with improper BPP", P_CRIT);
		// can't render it
	}
	// TODO: actually stretch it to fit the frame
	for(uint64_t x = 0;x < surface->w;x++){
		for(uint64_t y = 0;y < surface->h;y++){
			uint8_t *pixel_byte = (uint8_t*)surface->pixels + y * surface->pitch +
				x * surface->format->BytesPerPixel;
			const std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color =
				convert::color::bpc(
					frame->get_pixel(x, y), 8);
			pixel_byte[0] = 255;
			pixel_byte[1] = 255;
			pixel_byte[2] = 255;
			//  revert back once frame works reliably
			/*pixel_byte[0] = 
				(uint8_t)std::get<0>(color);
			pixel_byte[1] =
				(uint8_t)std::get<1>(color);
			pixel_byte[2] = 
			(uint8_t)std::get<2>(color);*/
			//P_V(pixel_byte[0], P_SPAM);
			//P_V(pixel_byte[1], P_SPAM);
			//P_V(pixel_byte[2], P_SPAM);
		}
	}
	return surface;
}

static SDL_Rect tv_render_gen_window_rect(tv_window_t *window){
		SDL_Rect window_rect;
		window_rect.w = window->get_x_res();
		window_rect.x = window->get_x_pos();
		window_rect.h = window->get_y_res();
		window_rect.y = window->get_y_pos();
		P_V(window_rect.w, P_SPAM);
		P_V(window_rect.x, P_SPAM);
		P_V(window_rect.h, P_SPAM);
		P_V(window_rect.y, P_SPAM);
		return window_rect;
}

static void tv_render_all(){
	std::vector<uint64_t> all_windows =
		id_api::cache::get("tv_window_t");
	for(uint64_t i = 0;i < all_windows.size();i++){
		print("found a window", P_SPAM);
		tv_window_t *window = PTR_DATA(all_windows[i], tv_window_t);
		if(window == nullptr){
			print("window is nullptr", P_ERR);
			continue;
		}
		tv_channel_t *channel =
			PTR_DATA(window->get_channel_id(), tv_channel_t);
		if(channel == nullptr){
			print("channel is nullptr", P_ERR);
			continue;
		}
		tv_frame_t *frame =
			PTR_DATA(channel->get_latest_frame_id(), tv_frame_t);
		if(frame == nullptr){
			print("frame is nullptr", P_ERR);
		}
		if(window_surface == nullptr){
			print("window_surface is nullptr", P_ERR);
		}
		SDL_Surface *frame_surface =
			tv_render_frame_to_surface(frame);
		SDL_Rect window_rect =
			tv_render_gen_window_rect(window);
		if(SDL_BlitSurface(frame_surface,
				   NULL,
				   window_surface,
				   NULL) < 0){
			print((std::string)"couldn't blit surface:"+SDL_GetError(), P_CRIT);
		}else{
			print("surface blit without errors", P_SPAM);
		}
		SDL_FreeSurface(frame_surface);
		frame_surface = nullptr;
	}
	/*
	  All surfaces that have been used for rendering have been blitted
	  to the screen
	 */
	if(SDL_UpdateWindowSurface(window) < 0){
		print((std::string)"cannot update window:"+SDL_GetError(), P_CRIT);
	}else{
		print("updated window without errors", P_SPAM);
	}
}

void tv_loop(){
	tv_render_all();
}

static void tv_init_test_channel(){
	tv_window_t *window =
		new tv_window_t;
	tv_channel_t *channel =
		new tv_channel_t;
	tv_frame_t *frame =
		new tv_frame_t;
	window->set_channel_id(channel->id.get_id());
	channel->set_latest_frame_id(frame->id.get_id());
	frame->reset(640,
		     480,
		     8,
		     100,
		     1,
		     1,
		     1);
	// most of these aren't even used yet, but enough
	// are being used for an OK video implementation
	for(uint64_t x = 0;x < 640;x++){
		for(uint64_t y = 0;y < 480;y++){
			frame->set_pixel(x, y, std::make_tuple(0, 255, 0, 8));
		}
	}
}

void tv_init(){
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("BasicTV",
				  SDL_WINDOWPOS_CENTERED,
				  SDL_WINDOWPOS_CENTERED,
				  640,
				  480,
				  SDL_WINDOW_SHOWN);
	if(window == nullptr){
		print((std::string)"window is nullptr:"+SDL_GetError(), P_ERR);
	}
	window_surface = SDL_GetWindowSurface(window);
	// blank the screen black
	SDL_FillRect(
		window_surface,
		NULL,
		SDL_MapRGB(window_surface->format, 0, 0, 0));
	SDL_UpdateWindowSurface(window);
	tv_init_test_channel();
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
