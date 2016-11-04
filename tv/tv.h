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
  8K TVs in 4x4 should be good enough, actual limit is
  the stack size
 */

#define TV_FRAME_SIZE (7680*4320*16*4)

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

namespace tv{
	namespace chan{
		/*
		  Channel information, metadata, ordering, and the like comes
		  from this. Actual playback is in layout::window
		 */
		uint64_t count(uint64_t flags = 0);
		// there shouldn't be a case where the ID isn't available, but
		// the PGP is and this information is needed
		uint64_t next(uint64_t id, uint64_t flags = 0);
		uint64_t prev(uint64_t id, uint64_t flags = 0);
		uint64_t rand(uint64_t flags = 0);
	};
	namespace layout{
		void set_size(uint8_t x, uint8_t y);
		std::pair<uint8_t, uint8_t> get();
		namespace window{
			// all windows ought to be the same res
			std::pair<uint32_t, uint32_t> get_res();
			uint64_t get_chan_id(uint8_t x, uint8_t y);
			std::array<uint8_t, PGP_PUBKEY_SIZE> get_chan_pubkey(uint8_t x, uint8_t y);
			void set_chan_id(uint64_t id, uint8_t x, uint8_t y);
			void set_chan_pubkey(std::array<uint8_t, PGP_PUBKEY_SIZE> pubkey, uint8_t x, uint8_t y);
		};
		namespace monitor{
			// unused, but would be pretty cool to have
			// a multimonitor TV setup
		};
	};
};

#endif
