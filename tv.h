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
#define TV_FRAME_SIZE 65536
// there is no reason to keep tv_patch_t's, as they are immediately
// used and new tv_frame_t's are generated
/*
  8K TVs in 4x4 should be good enough, actual limit is
  the stack size
 */
#define TV_FRAME_SIZE (7680*4320*16)

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
struct tv_window_t{
private:
	uint8_t x_pos = 0; // start counting at 0
	uint8_t y_pos = 0;
	uint64_t channel_id = 0;
public:
};

struct tv_patch_t{
private:
	// location of the byte to change
	std::array<uint64_t, STD_ARRAY_LENGTH> location = {};
	// new value of the byte
	std::array<uint64_t, STD_ARRAY_LENGTH> value = {};
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
	tv_patch_t(uint64_t frame_id_);
	~tv_patch_t();
	uint64_t get_frame_id();
	void set_frame_id(uint64_t frame_id_);
};

struct tv_frame_t{
private:
	// raw data from the frame, X(XRES) + Y for the value
	/*
	  TODO: Change this into uint8_t and add multiple
	  frames in the same variable to send out more 
	  color information (8-bit is okay for dial-up and
	  really slow connections)
	 */
	std::array<uint64_t, TV_FRAME_SIZE> frame = {};
	// channel ID
	uint64_t channel_id = 0;
	/* 
	   frame_number, increment by one every time.
	 */
	uint64_t frame_number = 0;
	uint16_t x_res = 0;
	uint16_t y_res = 0;
public:
	data_id_t id;
	tv_frame_t();
	~tv_frame_t();
	void set_pixel(uint64_t x, uint64_t y, uint64_t value);
	uint64_t get_pixel(uint64_t x, uint64_t y);
};

struct tv_channel_t{
private:
	/*
	  TODO: define a more legitimate type for PGP public keys than
	  a std::array with an arbitrary value
	 */
	std::array<uint8_t, PGP_PUBKEY_SIZE> pubkey = {};
	std::array<uint64_t, 1024> frame_list_id = {};
public:
	data_id_t id;
	tv_channel_t();
	~tv_channel_t();
	uint64_t get_frame_id(uint64_t backstep);
};

void tv_init();

namespace tv{
	namespace chan{
		/*
		  Channel information, metadata, ordering, and the like comes
		  from this. Actual playback is in layout::window
		 */
		uint64_t count();
		uint64_t next(uint64_t id);
		uint64_t next(std::array<uint8_t, PGP_PUBKEY_SIZE> pubkey);
		uint64_t prev(uint64_t id);
		uint64_t prev(std::array<uint8_t, PGP_PUBKEY_SIZE> pubkey);
		uint64_t rand();
		// get streaming information
	}
	namespace layout{
		void set_size(uint8_t x, uint8_t y);
		std::pair<uint8_t, uint8_t> get();
		namspace window{
			// all windows ought to be the same res
			std::pair<uint32_t, uint32_t> get_res();
			uint64_t get_chan_id(uint8_t x, uint8_t y);
			std::array<uint8_t, PGP_PUBKEY_SIZE> get_chan_pubkey(uint8_t x, uint8_t y);
			void set_chan_id(uint64_t id, uint8_t x, uint8_t y);
			void set_chan_pubkey(std::array<uint8_t, PGP_PUBKEY_SIZE> pubkey, uint8_t x, uint8_t y);
		}
		namespace monitor{
			// unused, but would be pretty cool to have
			// a multimonitor TV setup
		}
	}
}

#endif
