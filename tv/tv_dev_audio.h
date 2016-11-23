#ifndef TV_DEV_AUDIO_H
#define TV_DEV_AUDIO_H
#include "tv_dev.h"
#include "alsa/asoundlib.h"
// doesn't use open_dev or set_ioctl, not a conventional
// device file
#define TV_DEV_AUDIO_TYPE_UNDEF 0
#define TV_DEV_AUDIO_TYPE_OUTPUT 1
#define TV_DEV_AUDIO_TYPE_INPUT 2
class tv_dev_audio_t : public tv_dev_t{
private:
	snd_pcm_t *capture_handle = nullptr;
	snd_pcm_hw_params_t *hw_params = nullptr;
	uint8_t type = TV_DEV_AUDIO_TYPE_UNDEF;
	std::string filename;
public:
	data_id_t id;
	tv_dev_audio_t(std::string filename_, uint8_t type);
	~tv_dev_audio_t();
	void set_rate(uint64_t rate,
		      uint8_t depth);
	uint64_t update(uint64_t length_micro_s);
};

#endif
