#include "../id/id.h"
#ifndef TV_H
#define TV_H
#include "utility"
#include "chrono"

// hopefully that is too low some day
#define TV_CHANNEL_SIZE 65536
#define TV_WINDOW_SIZE 16
/* #define TV_FRAME_BUFFER_SIZE 65536 */
/* #define TV_FRAME_RETAIN_SIZE 1024 */

/*
  tv_client_t: Preferred settings for a stream
 */

struct tv_client_t{
private:
	uint64_t framerate = 0;
	uint64_t target_x_res = 0;
	uint64_t target_y_res = 0;
public:
	tv_client_t();
	~tv_client_t();
};


extern void tv_init();
extern void tv_loop();
extern void tv_close();

#endif
#include "tv_api.h"
