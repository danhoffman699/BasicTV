#include "../rsa.h"
#ifndef TV_API_H
#define TV_API_H
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
			std::array<uint8_t, RSA_MAX_KEY_LENGTH/8> get_chan_pubkey(uint8_t x, uint8_t y);
			void set_chan_id(uint64_t id, uint8_t x, uint8_t y);
			void set_chan_pubkey(std::array<uint8_t, RSA_MAX_KEY_LENGTH> pubkey, uint8_t x, uint8_t y);
		};
		namespace monitor{
			// unused, but would be pretty cool to have
			// a multimonitor TV setup
		};
	};
};

#endif
