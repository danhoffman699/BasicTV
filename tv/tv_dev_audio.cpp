#include "tv_dev_audio.h"
#include "tv_dev.h"
#include "tv_frame_audio.h"

// SDL_Mixer can't do microphones, so I have to use the full
// ALSA stack, which is too complex for now

tv_dev_audio_t::tv_dev_audio_t(std::string filename_, uint8_t type_) : id(this, __FUNCTION__){
	list_virtual_data(&id);
	filename = filename_;
	type = type_;
}

tv_dev_audio_t::~tv_dev_audio_t(){
}

id_t_ tv_dev_audio_t::update(){
	tv_frame_audio_t *audio =
		new tv_frame_audio_t;
	return audio->id.get_id();
}
