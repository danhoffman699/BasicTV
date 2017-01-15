#include "tv.h"
#include "tv_video.h"
#include "tv_audio.h"

void tv_init(){
	tv_audio_init();
	tv_video_init();
}

void tv_loop(){
	tv_audio_loop();
	tv_video_loop();
}

void tv_close(){
	tv_audio_close();
	tv_video_close();
}
