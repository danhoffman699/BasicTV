#include "tv_dev_audio.h"
#include "tv_dev.h"

// SDL_Mixer can't do microphones, so I have to use the full
// ALSA stack, which is too complex for now

tv_dev_audio_t::tv_dev_audio_t() : id(this, __FUNCTION__){
	list_virtual_data(&id);
}

tv_dev_audio_t::~tv_dev_audio_t(){
}
