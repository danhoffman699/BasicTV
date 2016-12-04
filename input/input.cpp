/*
  input.cpp: the general input API. This gets fed information from either the
  IR or networking API on key macros. Modifier keys work on both the IR and
  networking systems. However, a lot of the features on the networking API
  (phone app, if it is made) would have more intuitive displays that downscale
  the menus into modifiers, setting numbers, among other keys. 

  TODO: define behavior for multiple conflicting streams of input
 */
#include "../main.h"
#include "input.h"
#include "input_ir.h"
#include "../util.h"

static std::vector<uint64_t> ir_vector;
static std::vector<uint64_t> keyboard_vector;

static void input_gather_from_keyboard(){
	SDL_Event event;
	while(SDL_PollEvent(&event)){
		if(event.type != SDL_KEYDOWN){
			continue;
		}
		/*
		  Only need to catch non-alphanumerical keys
		 */
		switch(event.key.keysym.sym){
		case SDLK_ESCAPE:
			keyboard_vector.push_back(INPUT_KEY_ESCAPE);
			break;
		default:
			if(INPUT_IS_LETTER(event.key.keysym.mod) ||
			   INPUT_IS_NUMBER(event.key.keysym.mod)){
				keyboard_vector.push_back(
					event.key.keysym.mod);
			}
		}
	}
}

static void input_gather_from_ir(){
}

static void input_run_input(std::vector<uint64_t> vector){
	for(uint64_t i = 0;i < vector.size();i++){
		switch(vector[i]){
		case INPUT_KEY_ESCAPE:
			running = false;
			break;
		default:
			break;
		}
	}
}

void signal_handler(int signal){
	switch(signal){
	case SIGINT:
	case SIGTERM:
		exit(signal);
	}
}

void input_loop(){
	input_gather_from_keyboard();
	input_gather_from_ir();
	input_run_input(keyboard_vector);
}

void input_init(){
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	SDL_Init(SDL_INIT_EVENTS);
}

void input_close(){
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

input_dev_standard_t::input_dev_standard_t() : id(this, __FUNCTION__){
	ADD_DATA_ARRAY(queue, sizeof(queue[0]), INPUT_DEV_QUEUE_LENGTH);
}

uint8_t input_dev_standard_t::get_latest_entry(){
	uint8_t retval = queue[0];
	for(uint16_t i = 0;i < INPUT_DEV_QUEUE_LENGTH;i++){
		queue[i-1] = queue[i];
	}
	return retval;
}

void input_dev_standard_t::set_latest_entry(uint8_t entry_){
	for(uint16_t i = 0;i < INPUT_DEV_QUEUE_LENGTH;i++){
		if(queue[i] == 0){
			queue[i] = entry_;
			break;
		}
	}
}
