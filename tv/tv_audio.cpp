#include "tv_audio.h"
#include "tv.h"

void tv_audio_init(){
	if(Mix_OpenAudio(44100, AUDIO_U16LSB, 1, 16) < 0){
		print("cannot open audio:" + (std::string)(Mix_GetError()), P_ERR);
	}
}

/*
  I'm assuming that SDL2_mixer's Mix_Chunk uses a native 24-bit depth
*/

static void tv_audio_test_sound(){
	Mix_Chunk *chunk =
		new Mix_Chunk;
	chunk->allocated = 0;
	chunk->alen = 44100*1;
	chunk->abuf = new uint8_t[chunk->alen];
	chunk->volume = 128;
	memset(chunk->abuf, 0, chunk->alen);
	for(uint64_t i = 0;i < chunk->alen;i++){
		chunk->abuf[i] = true_rand(0, 255);
	}
	Mix_PlayChannel(-1, chunk, 0);
	print("playing channel", P_NOTE);
	sleep_ms(1500);
	delete[] chunk->abuf;
	chunk->abuf = nullptr;
	delete chunk;
	chunk = nullptr;
}

void tv_audio_loop(){
	tv_audio_test_sound();
}

void tv_audio_close(){
}
