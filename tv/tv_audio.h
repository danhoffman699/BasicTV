#include "../id/id.h"
#ifndef TV_AUDIO_H
#define TV_AUDIO_H
#include <SDL2/SDL_mixer.h>
#include <algorithm> // std::reverse

#define TV_AUDIO_DEFAULT_SAMPLING_RATE 44100
#define TV_AUDIO_DEFAULT_BIT_DEPTH 16
/*
  Size of chunk set to speakers at the same time.
  If it is set too high, latencies will be a problem
  If it is set too low, too much computing time will be used

  256 is pretty low, but I think even Raspberry Pis can handle it
*/
#define TV_AUDIO_DEFAULT_CHUNK_SIZE 256

extern void tv_audio_init();
extern void tv_audio_loop();
extern void tv_audio_close();
#endif
