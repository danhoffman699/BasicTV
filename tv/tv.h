#include "../id/id.h"
#include "../pgp.h"
#ifndef TV_H
#define TV_H
#include "utility"
#include "chrono"

// hopefully that is too low some day
#define TV_CHANNEL_SIZE 65536
// 4x4
#define TV_WINDOW_SIZE 16
// a pool of old tv_frame_ts are useful for reliable patching of
// new ones, and I have doubts that memory usage is as big of a
// problem as CPU usage (decrypting RSA)
#define TV_FRAME_BUFFER_SIZE 65536
/*
  This is the defined minimum amount of frames that a client is
  guaranteed to keep, since the ID subsystem can't garbage collect
  currently-referenced IDs. More tv_frame_t's might be stored, but 
  are referenced in DVR modes and possible extensions
 */
#define TV_FRAME_RETAIN_SIZE 1024
// there is no reason to keep tv_patch_t's, as they are immediately
// used and new tv_frame_t's are generated

/*
  Since I am actually using this right now, let's put the cap at 1080p.
  I plan to keep support for insane max resolutions, but I need to make
  a function that pre-allocates large contiguous chunks of memory. Once that
  is done, then this can go back up within reason

  I can also make different subsets of tv_frame_t, one for low quality, one for 
  current HD, and one for 4K streams and above, but that overcomplicates
  a lot of the code. What ought to happen is defining a second type of
  patch that goes inside of the frame array, and is referring to the first
  frame. Conventional patches should still exist for older frames.
 */

#define TV_FRAME_SIZE (1920*1080*4)

/*
  96KHz at 1/10 Hz
 */

#define TV_FRAME_AUDIO_SIZE (96000*10)

/*
  Lossless here means that the versions of the stream are the
  same on both versions, and the actual patch is not compressed
  This is not inherently the highest quality stream, just the
  same version of the same quality stream
 */
#define TV_PATCH_INFO_LOSSLESS (1)
#define TV_PATCH_INFO_LOSSY (0)


/*
  tv_client_t: settings for the stream, along with any information the
  server ought to know about the client
 */

/*
  A lot of the following macros are only ideas right now, and the rest
  of the program should be built before I bother with a lot of this
 */

// create two images, tinting specifics is client-specific
#define TV_CLIENT_SPECIAL_3D (1 << 0)
// compatible with the Oculus VR (or other systems)
#define TV_CLIENT_SPECIAL_VR (1 << 1)

struct tv_client_t{
private:
	uint64_t framerate = 0;
	uint64_t target_x_res = 0;
	uint64_t target_y_res = 0;
	// aspect ratio is derived from target_x_res
	uint64_t special = 0;
public:
	tv_client_t();
	~tv_client_t();
};


extern void tv_init();
extern void tv_loop();

#endif
#include "tv_api.h"
