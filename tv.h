#include "id.h"
#include "pgp.h"
#ifndef TV_H
#define TV_H
#include "utility"

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
  A channel has to exist inside of a window at all times. When moving from
  a lower resolution to a higher resolution display, the top-left channel
  is always kept and the lower-right channels are cropped off or added upon.
  The position of the window is defined in an X and Y plane
 */
/*struct tv_window_t{
private:
	uint8_t x_pos = 0; // start counting at 0
	uint8_t y_pos = 0;
	uint64_t channel_id = 0;
public:
	uint8_t get_x_pos();
	void get_x_pos(uint8_t x_pos_);
	uint8_t get_y_pos();
	void get_y_pos(uint8_t y_pos_);
	
	};*/
/*
  tv_patch_t: patching the frame data. This CANNOT alter
  the bit depth or any settings with the tv_frame_t type
  except for framerate, a new frame will have to be sent 
  over for that to happen. This is only meant to patch the 
  current frame and keep all of the settings

  TODO: implement audio patches
*/

struct tv_patch_t{
private:
	// location of the byte to change
	std::array<uint64_t, STD_ARRAY_LENGTH> location = {{0}};
	// new value of the byte
	std::array<uint64_t, STD_ARRAY_LENGTH> value = {{0}};
	// id to be applied to
	/*
	  TODO: define a minimum amount of frames that the 
	  client is guaranteed to keep, so tv_patch_t can
	  more easily reference different frames without
	  a handshake system to ensure that the client
	  has the data (also helps with distribution of content).
	 */
	uint64_t frame_id = 0;
	/*
	  Any useful boolean information:
	  lossy or lossless?
	  physically more information
	 */
	uint64_t info = 0;
public:
	data_id_t id;
	tv_patch_t();
	~tv_patch_t();
	uint64_t get_frame_id();
	void set_frame_id(uint64_t frame_id_);
};

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

/*
  tv_frame_t: individual frame with all of the settings as requested
  from tv_client_t
 */

struct tv_frame_t{
private:
	std::array<uint8_t, TV_FRAME_SIZE> frame = {{0}};
	std::array<uint8_t, TV_FRAME_AUDIO_SIZE> audio = {{0}};
	// channel ID
	uint64_t channel_id = 0;
	/*
	   frame_number, increment by one every time.
	 */
	uint64_t frame_number = 0;
	uint64_t x_res = 0;
	uint64_t y_res = 0;
	uint64_t unix_timestamp = 0;
	uint8_t frame_count = 0;
public:
	data_id_t id;
	tv_frame_t();
	~tv_frame_t();
	/*
	  most settings cannot be changed OTF
	 */
	void reset_frame(uint64_t x, uint64_t y);
	void set_pixel(uint64_t x, uint64_t y, uint64_t frame_number);
	uint64_t get_frame_number();
	uint64_t get_pixel(uint64_t x, uint64_t y);
	uint64_t get_x_res();
	uint64_t get_y_res();
	uint64_t get_timestamp();
};

/*
  tv_channel_t: channel information. contains information about the stream
  and frames (video only, audio only, etc.)
 */

#define TV_CHAN_STREAMING (1 << 0)
#define TV_CHAN_NO_AUDIO (1 << 1)
#define TV_CHAN_NO_VIDEO (1 << 2)

struct tv_channel_t{
private:
	std::array<uint8_t, PGP_PUBKEY_SIZE> pubkey = {{0}};
	std::array<uint64_t, TV_FRAME_RETAIN_SIZE> frame_list_id = {{0}};
	uint64_t status = 0;
public:
	data_id_t id;
	tv_channel_t();
	~tv_channel_t();
	uint64_t get_frame_id(uint64_t backstep);
	void set_frame_id(uint64_t id, uint64_t backstep);
	bool is_streaming();
	bool is_audio();
	bool is_video();
};

void tv_init();

namespace tv{
	namespace chan{
		/*
		  Channel information, metadata, ordering, and the like comes
		  from this. Actual playback is in layout::window
		 */
		uint64_t count(uint64_t flags = 0);
		uint64_t next(uint64_t id, uint64_t flags = 0);
		uint64_t next(std::array<uint8_t, PGP_PUBKEY_SIZE> pubkey, uint64_t flags = 0);
		uint64_t prev(uint64_t id, uint64_t flags = 0);
		uint64_t prev(std::array<uint8_t, PGP_PUBKEY_SIZE> pubkey, uint64_t flags = 0);
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
