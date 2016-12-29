#include "../id/id.h"
#ifndef TV_AUDIO_H
#define TV_AUDIO_H
#include "SDL2/SDL_mixer.h"
#include "algorithm" // std::reverse

#define TV_AUDIO_DEFAULT_SAMPLING_RATE 44100
#define TV_AUDIO_DEFAULT_BIT_DEPTH 16
/*
  Size of chunk set to speakers at the same time.
  If it is set too high, latencies will be a problem
  If it is set too low, too much computing time will be used

  256 is pretty low, but I think even Raspberry Pis can handle it
*/
#define TV_AUDIO_DEFAULT_CHUNK_SIZE 256

/*
  This contains information about the tv_frame_audio_t stream, Mix_Chunks for
  each of them, and information about the state of the mixing channel in SDL2
 */
struct tv_audio_channel_t{
private:
	// everything in chunk_vector is in the SDL_mixer
	std::vector<std::pair<id_t_, Mix_Chunk*> > chunk_vector =
	{{0, nullptr}};
	void update_add_new(); // adding new IDs
	void update_del_old(); // deleting old IDs and Mix_Chunks
	void update_gen_chunks(); // adding new Mix_Chunks
public:
	tv_audio_channel_t(id_t_ seed_id);
	~tv_audio_channel_t();
	void update(); // generates new Mix_Chunk data
	void destroy_all();
};
// audio output logic, input logics are handled in tv_dev_* files
extern void tv_audio_init();
extern void tv_audio_loop();
extern void tv_audio_close();
#endif
