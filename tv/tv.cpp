#include "tv.h"
#include "tv_video.h"
#include "tv_audio.h"

void tv_init(){
	tv_audio_init();
	// finalize video interface first
}

void tv_loop(){
	tv_audio_loop();
}

void tv_close(){
	tv_audio_close();
}
