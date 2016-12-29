#ifndef TV_FRAME_AUDIO_H
#define TV_FRAME_AUDIO_H
#include "tv_frame_standard.h"
#include "opus/opus.h"

#define TV_FRAME_AUDIO_DEFAULT_SAMPLING_RATE 48000
#define TV_FRAME_AUDIO_DEFAULT_BIT_DEPTH 24

#define TV_FRAME_AUDIO_DATA_SIZE (TV_FRAME_AUDIO_DEFAULT_SAMPLING_RATE*(TV_FRAME_AUDIO_DEFAULT_BIT_DEPTH/8))

#define TV_FRAME_AUDIO_DEFAULT_FORMAT TV_FRAME_AUDIO_FORMAT_OPUS

// shouldn't ever be the case
#define TV_FRAME_AUDIO_FORMAT_UNDEFINED 0
// At least use FLAC if you are looking for quality, this is for testing
#define TV_FRAME_AUDIO_FORMAT_RAW 1
// default, works pretty well, nice licensing
#define TV_FRAME_AUDIO_FORMAT_OPUS 2
// not implemented yet, but not a bad idea either
#define TV_FRAME_AUDIO_FORMAT_FLAC 3

// first 2-bits are reserved for the raw format

/*
  tv_frame_audio_t: snippet of audio

  Since compression is going to be king, then data is just the raw data (unless
  raw is used, then it is samples).

  Sound streams are going to be the first streams on to the network, since
  having uncompressed versions isn't going to saturate the nodes' bandwidth as
  much as uncompressed video will. However, compression is still a very high
  priority.

  I have no plan of using MP3 on the network as a widely supported protocol,
  mainly because of restrictive licensing. Opus is a great alternative (which
  ties in nicely with the planned VP9 compression). I can't imagine switching
  from MP3 to Opus will cause any major loss of quality (if you are concerned
  about quality, convert to Opus from FLAC, if that is an option).
*/

#define TV_FRAME_AUDIO_FORMAT_MASK 0b00000011

// no shift, other macros need shift operators
#define GET_TV_FRAME_AUDIO_FORMAT(fl) (fl & TV_FRAME_AUDIO_FORMAT_MASK)
#define SET_TV_FRAME_AUDIO_FORMAT(fl, fo) (fl &= ~TV_FRAME_AUDIO_FORMAT_MASK;fl |= fo & TV_FRAME_AUDIO_FORMAT_MASK)

/*
  tv_frame_audio_t should always be unsigned with syste byte order. Bit depth is
  the only variable that can change (16 is where SDL2 taps out)
*/

class tv_frame_audio_t : public tv_frame_standard_t{
private:
	uint8_t flags = 0;
	//std::array<uint8_t, TV_FRAME_AUDIO_DATA_SIZE> data = {{0}};
	std::vector<uint8_t> data;
	uint8_t bit_depth = 0;
	uint32_t sampling_freq = 0;
public:
	data_id_t id;
	tv_frame_audio_t();
	~tv_frame_audio_t();
	void get_type(uint64_t *sampling_freq_,
		      uint8_t *bit_depth_,
		      uint8_t *flags_);
	void set_type(uint64_t sampling_freq_,
		      uint8_t bit_depth_,
		      uint8_t flags_);
	uint8_t get_flags(){return flags;}
	uint8_t get_bit_depth(){return bit_depth;}
	uint32_t get_sampling_freq(){return sampling_freq;}
	std::vector<uint8_t> get_data();
	void set_data(std::vector<uint8_t> data_);
};
#endif
