/*
  input.cpp: the general input API. This gets fed information from either the
  IR or networking API on key macros. Modifier keys work on both the IR and
  networking systems. However, a lot of the features on the networking API
  (phone app, if it is made) would have more intuitive displays that downscale
  the menus into modifiers, setting numbers, among other keys. 

  TODO: define behavior for multiple conflicting streams of input
 */
#include "input.h"
#include "ir.h"
#include "util.h"

static std::vector<uint64_t> ir_vector;
static std::vector<uint64_t> keyboard_vector;

static void input_gather_from_keyboard(){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		if(event.type != SDL_KEYDOWN){
			continue;
		}
		switch(event.key.keysym.sym){
		case SDLK_ESCAPE:
			keyboard_vector.push_back(INPUT_KEY_ESCAPE);
			break;
		}
	}
}

static void input_gather_from_ir(){
}

static void input_run_input(std::vector<uint64_t> vector){
	for(uint64_t i = 0;i < vector.size();i++){
		switch(vector[i]){
		case INPUT_KEY_ESCAPE:
			print("terminating program", P_CRIT);
			exit(0); // really shouldn't be needed
			break;
		default:
			break;
		}
	}
}

void input_loop(){
	input_gather_from_keyboard();
	input_gather_from_ir();
	input_run_input(keyboard_vector);
}

void input_init(){
	SDL_Init(SDL_INIT_EVENTS);
}

