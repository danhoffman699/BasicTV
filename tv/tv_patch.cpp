#include "../main.h"
#include "../util.h"
#include "tv.h"
#include "tv_channel.h"
#include "tv_patch.h"

tv_patch_t::tv_patch_t() : id(this, __FUNCTION__){
	id.add_data(&(location[0]), 65536*sizeof(location[0]));
	id.add_data(&(value[0]), 65536*sizeof(value[0]));
	id.add_id(&frame_id, sizeof(frame_id));
}

tv_patch_t::~tv_patch_t(){
}
