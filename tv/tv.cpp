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
#include "tv_menu.h"

#define TEST_FRAME_SIZE 120
#define WINDOW_X_RES 1280
#define WINDOW_Y_RES 720

/*
  TODO: clean up all of this code. I made it in a hurry to use as proof
  that it all works fine. Most of what is here doesn't break anything
  low level or would otherwise need large fixes, but it just needs
  to be cleaned up a bit (and tv_frame as well)
 */

/*
  channel and window arrays are created OTF with id_api::cache::get
 */

static SDL_Window *sdl_window = nullptr;

// this surface should fit the dimensions of the frame, let SDL
// handle the resizing (at least for now)

static SDL_Surface* tv_render_frame_to_surface_copy(tv_frame_t *frame){
	const uint64_t width = frame->get_x_res();
	const uint64_t height = frame->get_y_res();
	const uint8_t depth = frame->get_bpc()*4; // includes alpha
	const uint64_t red_mask = frame->get_red_mask();
	const uint64_t green_mask = frame->get_green_mask();
	const uint64_t blue_mask = frame->get_blue_mask();
	const uint64_t alpha_mask = frame->get_alpha_mask();
	// TODO: actually stretch it to fit the frame
	// older approach, very slow
	SDL_Surface *surface =
		SDL_CreateRGBSurface(0,
				     width,
				     height,
				     depth,
				     red_mask,
				     green_mask,
				     blue_mask,
				     alpha_mask);
	if(unlikely(surface == nullptr)){
		print((std::string)"surface is a nullptr:" + SDL_GetError(), P_ERR);
	}
	for(uint64_t x = 0;x < surface->w;x++){
		for(uint64_t y = 0;y < surface->h;y++){
			uint8_t *pixel_byte = (uint8_t*)surface->pixels + y * surface->pitch +
				x * surface->format->BytesPerPixel;
			const std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color =
				convert::color::bpc(
					frame->get_pixel(x, y), 8);
			//  revert back once frame works reliably
			pixel_byte[0] = 
				(uint8_t)std::get<0>(color);
			pixel_byte[1] =
				(uint8_t)std::get<1>(color);
			pixel_byte[2] = 
				(uint8_t)std::get<2>(color);
		}
	}
	return surface;
}

static uint32_t tv_render_get_frame_sdl_enum(tv_frame_t *frame){
	uint32_t pixel_format_enum = 0;
	if(frame->get_alpha_mask() != 0){
		print("tv_frame_t doesn't support alpha", P_ERR);
	}
	switch(frame->get_bpc()){
	case 8:
		if(frame->get_red_mask() == 0x0000FF &&
		   frame->get_green_mask() == 0x00FF00 &&
		   frame->get_blue_mask() == 0xFF0000){
			pixel_format_enum = SDL_PIXELFORMAT_BGR24;
		}else if(frame->get_red_mask() == 0xFF0000 &&
			 frame->get_green_mask() == 0x00FF00 &&
			 frame->get_blue_mask() == 0x0000FF){
			pixel_format_enum = SDL_PIXELFORMAT_RGB24;
		}
		break;
	default:
		print("non-standard BPC, rendering is going to be slower", P_SPAM);
		/*case 5:
		if(frame->get_red_mask() == 0b0000000000011111 &&
		   frame->get_green_mask() == 0b0000001111100000 &&
		   frame->get_blue_mask() == 0b0111110000000000){
			pixel_format_enum = SDL_PIXELFORMAT_RGB555;
		}else if(frame->get_red_mask() == 0b0111110000000000 &&
			 frame->get_green_mask() == 0b0000001111100000 &&
			 frame->get_blue_mask() == 0b0000000000011111){
			pixel_format_enum = SDL_PIXELFORMAT_BGR555;
		}
		break;*/
	}
	return pixel_format_enum;
}

static SDL_Surface* tv_render_frame_to_surface_ptr(tv_frame_t *frame){
	const uint64_t width = frame->get_x_res();
	const uint64_t height = frame->get_y_res();
	const uint8_t bpc = frame->get_bpc();
	uint32_t pixel_format_enum =
		tv_render_get_frame_sdl_enum(frame);
	if(pixel_format_enum == 0){
		print("no known SDL analog for frame, copying", P_WARN);
		return nullptr;
	}
	SDL_Surface *retval =
		SDL_CreateRGBSurface(0,
				     width,
				     height,
				     bpc*3,
				     0,
				     0,
				     0,
				     0);
	if(unlikely(retval == nullptr)){
		print((std::string)"unable to generate surface:" + SDL_GetError(), P_ERR);
	}
	retval = SDL_ConvertSurfaceFormat(retval, pixel_format_enum, 0);
	if(unlikely(retval == nullptr)){
		print((std::string)"cannot convert surface to desired format:" + SDL_GetError(), P_ERR);
	}
	if(unlikely(SDL_LockSurface(retval) < 0)){
		print((std::string)"unable to lock surface:"+SDL_GetError(), P_ERR);
	}
	retval->pixels = frame->get_pixel_data_ptr();
	SDL_UnlockSurface(retval);
	return retval;
}

/*
  This is a lot easier to generate than a test card, and looks a lot
  better as well. 
 */

static tv_frame_t *tv_frame_gen_xor_frame(uint64_t x_, uint64_t y_, uint8_t bpc){
	tv_frame_t *frame = new tv_frame_t;
	frame->reset(x_,
		     y_,
		     TV_FRAME_DEFAULT_BPC,
		     TV_FRAME_DEFAULT_RED_MASK,
		     TV_FRAME_DEFAULT_GREEN_MASK,
		     TV_FRAME_DEFAULT_BLUE_MASK,
		     0,
		     1000*1000,
		     1,
		     1,
		     1);
	if(unlikely(bpc != 8)){
		print("BPC is not supported", P_ERR);
	}
	for(uint64_t y = 0;y < y_;y++){
		for(uint64_t x = 0;x < x_;x++){
			frame->set_pixel(x,
					 y,
					 std::make_tuple(
						 (x^y)&255,
						 (x^y)&255,
						 (x^y)&255,
						 8));
		}
	}
	return frame;
}

static SDL_Rect tv_render_gen_window_rect(tv_window_t *window,
					  SDL_Surface *surface){
	SDL_Rect window_rect;
	if(window->get_pos() != TV_WINDOW_CT){
		print("unsupported window position", P_CRIT);
	}
	window_rect.w = surface->w;
	window_rect.x = 0;
	window_rect.h = surface->h;
	window_rect.y = 0;
	return window_rect;
}

static bool tv_frame_valid(tv_frame_t *frame){
	const uint64_t curr_time_micro_s =
		get_time_microseconds();
	const uint64_t frame_end_time_micro_s =
		frame->get_end_time_micro_s();
	if(curr_time_micro_s > frame_end_time_micro_s){
		print("frame is old, re-running with new", P_SPAM);
		return false;
	}else{
		print("frame is current, exiting loop", P_SPAM);
		return true;
	}

}

static uint64_t tv_render_id_of_last_valid_frame(uint64_t current){
	// TODO: add support for patches
	std::vector<uint64_t> frame_linked_list =
		id_api::array::get_forward_linked_list(current, 0);
	for(uint64_t i = 0;i < frame_linked_list.size();i++){
		tv_frame_t *frame =
			PTR_DATA(frame_linked_list[i], tv_frame_t);
		if(tv_frame_valid(frame)){
			return frame_linked_list[i];
		}
	}
	return current;
}

static void tv_render_frame_to_screen_surface(tv_frame_t *frame,
					      SDL_Surface *sdl_window_surface,
					      SDL_Rect sdl_window_rect){
	if(frame->get_alpha_mask() != 0){
		print("alpha mask is not supported at this time", P_CRIT);
	}
	bool dereference_pixel_data = true;
	SDL_Surface *frame_surface =
		tv_render_frame_to_surface_ptr(frame);
	if(frame_surface == nullptr){
		frame_surface =
			tv_render_frame_to_surface_copy(frame);
		dereference_pixel_data = false;
	}
	if(unlikely(SDL_BlitScaled(frame_surface,
				    NULL,
				    sdl_window_surface,
				    &sdl_window_rect) < 0)){
		print((std::string)"couldn't blit surface:"+SDL_GetError(), P_CRIT);
	}else{
		print("surface blit without errors", P_SPAM);
	}
	if(likely(dereference_pixel_data)){
		frame_surface->pixels = nullptr;
	}
	SDL_FreeSurface(frame_surface);
	frame_surface = nullptr;
}

static void tv_render_all(){
	std::vector<uint64_t> all_windows =
		id_api::cache::get("tv_window_t");
	for(uint64_t i = 0;i < all_windows.size();i++){
		print("found a window", P_SPAM);
		tv_window_t *window = PTR_DATA(all_windows[i], tv_window_t);
		CONTINUE_IF_NULL(window);
		tv_channel_t *channel =
			PTR_DATA(window->get_channel_id(), tv_channel_t);
		CONTINUE_IF_NULL(channel);
		tv_frame_t *frame = nullptr;
		try{
			frame = PTR_DATA(tv_render_id_of_last_valid_frame(
						 channel->get_latest_frame_id()),
					 tv_frame_t);
		}catch(...){
			print("tv_channel_t seed ID is invalid", P_ERR);
			continue;
		}
		if(unlikely(frame == nullptr)){
			print("frame is a nullptr", P_ERR);
		}
		SDL_Surface *sdl_window_surface =
			SDL_GetWindowSurface(sdl_window);
		if(unlikely(sdl_window_surface == nullptr)){
			print("sdl_window_surface is nullptr", P_ERR);
		}
		SDL_Rect sdl_window_rect = 
			tv_render_gen_window_rect(window,
						  sdl_window_surface);
		tv_render_frame_to_screen_surface(frame,
						  sdl_window_surface,
						  sdl_window_rect);
	}
	/*
	  All surfaces that have been used for rendering have been blitted
	  to the screen previously
	 */
	if(unlikely(SDL_UpdateWindowSurface(sdl_window) < 0)){
		print((std::string)"cannot update sdl_window:"+SDL_GetError(), P_CRIT);
	}else{
		print("updated sdl_window without errors", P_SPAM);
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
	window->set_channel_id(channel->id.get_id());
	std::array<tv_frame_t*, TEST_FRAME_SIZE> tmp_frames = {{nullptr}};
	tv_menu_t *menu = new tv_menu_t;
	menu->set_menu_entry(0, "Isn't this the sexiest font ever?");
	channel->set_latest_frame_id(menu->get_frame_id());
	//tv_frame_t *frame = tv_frame_gen_xor_frame(1920, 1080, 8);
	//channel->set_latest_frame_id(frame->id.get_id());
}

void tv_init(){
	SDL_Init(SDL_INIT_VIDEO);
	sdl_window = SDL_CreateWindow("BasicTV",
				      SDL_WINDOWPOS_CENTERED,
				      SDL_WINDOWPOS_CENTERED,
				      WINDOW_X_RES,
				      WINDOW_Y_RES,
				      SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(sdl_window == nullptr){
		print((std::string)"window is nullptr:"+SDL_GetError(), P_ERR);
	}
	// blank the screen black
	SDL_FillRect(
		SDL_GetWindowSurface(sdl_window),
		NULL,
		SDL_MapRGB(SDL_GetWindowSurface(sdl_window)->format, 0, 0, 0));
	SDL_UpdateWindowSurface(sdl_window);
	tv_init_test_channel();
}
