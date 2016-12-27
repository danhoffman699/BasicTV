#include "../id/id.h"
#ifndef TV_H
#define TV_H
#include "utility"
#include "chrono"

#define TV_CHANNEL_SIZE 65536
#define TV_WINDOW_SIZE 16

extern void tv_init();
extern void tv_loop();
extern void tv_close();

#endif
#include "tv_api.h"
