#ifndef TV_DEV_AUDIO_H
#define TV_DEV_AUDIO_H
#include "tv_dev.h"
class tv_dev_audio_t : public tv_dev_t{
private:
public:
	data_id_t id;
	tv_dev_audio_t();
	~tv_dev_audio_t();
	uint64_t update();
};

#endif
