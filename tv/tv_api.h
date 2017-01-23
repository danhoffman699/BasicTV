#include "../encrypt/encrypt.h"
#include "../encrypt/encrypt_rsa.h"
#ifndef TV_API_H
#define TV_API_H
// flags are the following: TV_CHAN_STREAMING, TV_CHAN_AUDIO, TV_CHAN_VIDEO
// STREAMING is known by how old the associated ID is (should be updated often)
namespace tv{
	namespace chan{
		uint64_t count(uint64_t flags = 0);
		id_t_ next_id(id_t_ id, uint64_t flags = 0);
		id_t_ prev_id(id_t_ id, uint64_t flags = 0);
		id_t_ rand_id(id_t_ flags = 0);
	};
	// layout is defined through tv_window_t
};

#endif
