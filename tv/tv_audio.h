#ifndef TV_AUDIO_H
#define TV_AUDIO_H
#include "SDL2/SDL_mixer.h"
// audio output logic, input logics are handled in tv_dev_* files
extern void tv_audio_init();
extern void tv_audio_loop();
extern void tv_audio_close();
#endif
