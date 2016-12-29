#include "tv_audio.h"
#include "tv.h"

/*
  TODO: implement a universal time state that all streams can sync up with
  (tv_time_t?)
 */

// tv_audio_channel_t is simple enough to stay in this file

static void tv_audio_wave_big_endian(std::vector<uint8_t> *retval, const char *data){
	// strings don't appear to have any fancy work
	retval->insert(retval->end(), data, data+strlen(data));
}

static void tv_audio_wave_big_endian(std::vector<uint8_t> *retval, uint32_t data){
	// reverse byte order, but little endian bits
	std::vector<uint8_t> data_tmp(&data, &data+sizeof(data));
	std::reverse(data_tmp.begin(), data_tmp.end());
	retval->insert(retval->end(), data_tmp.begin(), data_tmp.end());
}

static void tv_audio_wave_big_endian(std::vector<uint8_t> *retval, uint16_t data){
	std::vector<uint8_t> data_tmp(&data, &data+sizeof(data));
	std::reverse(data_tmp.begin(), data_tmp.end());
	retval->insert(retval->end(), data_tmp.begin(), data_tmp.end());
}

static std::vector<uint8_t> tv_audio_get_wav_data(tv_frame_audio_t *frame){
	std::vector<uint8_t> retval;
	tv_audio_wave_big_endian(&retval, "RIFF");
	tv_audio_wave_big_endian(&retval, (uint32_t)frame->get_data().size());
	tv_audio_wave_big_endian(&retval, "WAVE");
	tv_audio_wave_big_endian(&retval, "fmt ");
	retval.push_back(16);
	retval.push_back(0);
	retval.push_back(0);
	retval.push_back(0);
	retval.push_back(1); // uncompressed PCM
	retval.push_back(0);
	retval.push_back(1); // channel count
	retval.push_back(0);
	tv_audio_wave_big_endian(&retval, (uint32_t)frame->get_sampling_freq());
	tv_audio_wave_big_endian(&retval, (uint32_t)frame->get_sampling_freq()*1*frame->get_bit_depth()/8);
	retval.push_back((1*frame->get_bit_depth()/8)); // block align
	retval.push_back(0);
	retval.push_back(frame->get_bit_depth()); // bits per sample
	retval.push_back(0);
	tv_audio_wave_big_endian(&retval, "data");
	tv_audio_wave_big_endian(&retval, (uint32_t)frame->get_data().size());
	// insert the data here
	retval.insert(
		retval.end(),
		frame->get_data().begin(),
		frame->get_data().end());
	return retval;
}

tv_audio_channel_t::tv_audio_channel_t(id_t_ seed_id){
	chunk_vector.push_back(
		std::make_pair(
			seed_id,
			nullptr));
	update();
}

tv_audio_channel_t::~tv_audio_channel_t(){

}

void tv_audio_channel_t::update_add_new(){
	if(chunk_vector.size() == 0){
		print("can't update without at least one id", P_ERR);
	}
	uint64_t snd_prefetch_len =
		std::stoi(
			settings::get_setting(
				"snd_prefetch_len"));
	const uint64_t end_micro_s =
		get_time_microseconds()+snd_prefetch_len;
	uint64_t latest_time_micro_s =
		0;
	while(latest_time_micro_s < end_micro_s){
		tv_frame_audio_t *new_frame =
			PTR_DATA(chunk_vector.end()->first,
				 tv_frame_audio_t);
		if(new_frame == nullptr){
			/*
			  I got nothing to go off of, so this is the end
			 */
			print("detected end of stream", P_NOTE);
			break;
		}
		chunk_vector.push_back(
			std::make_pair(
				new_frame->id.get_id(),
				nullptr)); // generating is done elsewhere
		latest_time_micro_s =
			new_frame->get_end_time_micro_s();
	}
}

void tv_audio_channel_t::update_del_old(){
	const uint64_t cur_time_micro_s =
		get_time_microseconds();
	for(uint64_t i = 0;i < chunk_vector.size();i++){
		tv_frame_audio_t *cur_frame =
			PTR_DATA(chunk_vector[i].first,
				 tv_frame_audio_t);
		if(cur_frame == nullptr){
			continue;
		}
		tv_frame_audio_t *next_frame =
			PTR_DATA(cur_frame->id.get_next_linked_list(),
				 tv_frame_audio_t);
		if(next_frame == nullptr){
			continue;
		}
		const uint64_t cur_frame_end_time_micro_s =
			cur_frame->get_end_time_micro_s();
		const uint64_t next_frame_end_time_micro_s =
			next_frame->get_end_time_micro_s();
		if(next_frame_end_time_micro_s >= cur_time_micro_s){
			/*
			  There has to be at least one old entry to prevent
			  blanking everything
			 */
			break;
		}
		if(cur_frame_end_time_micro_s < cur_time_micro_s){
			delete[] chunk_vector[i].second->abuf;
			chunk_vector[i].second->abuf = nullptr;
			delete chunk_vector[i].second;
			chunk_vector[i].second = nullptr;
			chunk_vector.erase(
				chunk_vector.begin()+i);
			i--;
		}
	}
}

/*
  tv_frame_audio_t uses unsigned integers with system byte order (system byte
  order because it works the easiest
*/

static std::vector<uint8_t> tv_audio_convert_raw_bitrate(tv_frame_audio_t *tmp,
							 uint8_t bit_depth,
							 uint32_t sampling_freq){
	if(bit_depth == tmp->get_bit_depth() ||
	   sampling_freq == tmp->get_sampling_freq()){
		return tmp->get_data();
	}else{
		print("no conversion exists yet for this combination", P_ERR);
	}
}

void tv_audio_channel_t::update_gen_chunks(){
	for(uint64_t i = 0;i < chunk_vector.size();i++){
		if(chunk_vector[i].second != nullptr){
			continue;
		}
		tv_frame_audio_t *frame =
			PTR_DATA(chunk_vector[i].first,
				tv_frame_audio_t);
		if(frame == nullptr){
			continue;
		}
		std::vector<uint8_t> raw =
			frame->get_data();
		std::vector<uint8_t> final;
		Mix_Chunk *chunk_tmp = new Mix_Chunk;
		memset(chunk_tmp, 0, sizeof(Mix_Chunk));
		switch(GET_TV_FRAME_AUDIO_FORMAT(frame->get_flags())){
		case TV_FRAME_AUDIO_FORMAT_RAW:
			chunk_tmp->allocated = 1;
			chunk_tmp->alen = raw.size();
			chunk_tmp->abuf =
				new uint8_t[chunk_tmp->alen];
			chunk_tmp->volume = MIX_MAX_VOLUME;
			final = tv_audio_convert_raw_bitrate(
				frame,
				24,
				44100);
			break;
		case TV_FRAME_AUDIO_FORMAT_UNDEFINED:
		case TV_FRAME_AUDIO_FORMAT_OPUS:
		case TV_FRAME_AUDIO_FORMAT_FLAC:
			print("can't decode unsupported formats", P_ERR);
			break;
		}
		chunk_vector[i].second =
			chunk_tmp;
	}
}

/*
  This funcion detects which tv_frame_audio_t is valid, creates a Mix_Chunk
  if one hadn't already been created, pushes it to the local std::vector,
  adds it to a queue to be played in SDL, and repeats the process for frames
  within a certain time from now
 */

void tv_audio_channel_t::update(){
}

static uint32_t tv_audio_sdl_format_from_depth(uint8_t bit_depth){
	switch(bit_depth){
	case 24:
		print("24-bit sound output isn't supported yet, falling back to 16-bit", P_WARN);
	case 16:
		print("using unsigned 16-bit system byte order", P_NOTE);
		return AUDIO_U16SYS;
	case 8:
		print("using unsigned 8-bit system byte order (are you sure you want to do this?)", P_WARN);
		return AUDIO_U8; // no SYS in 8-bit, is it assumed?
	default:
		print("unknown bit depth for SDL conversion, using 16-bit", P_WARN);
		return AUDIO_U16SYS;
	}
}

void tv_audio_init(){
	SDL_Init(SDL_INIT_AUDIO);
	uint32_t output_sampling_rate = 0;
	uint8_t output_bit_depth = 0;
	uint32_t output_chunk_size = 0;
	try{
		output_sampling_rate =
			std::stoi(
				settings::get_setting(
					"snd_output_sampling_rate"));
		output_bit_depth =
			std::stoi(
				settings::get_setting(
					"snd_output_bit_depth"));
		output_chunk_size =
			std::stoi(
				settings::get_setting(
					"snd_output_chunk_size"));
	}catch(...){
		// no big problem, these values are sane (maybe not chunk size)
		print("cannot read sound settings from file, setting default", P_ERR);
		output_sampling_rate = TV_AUDIO_DEFAULT_SAMPLING_RATE;
		output_bit_depth = TV_AUDIO_DEFAULT_BIT_DEPTH;
		output_chunk_size = TV_AUDIO_DEFAULT_CHUNK_SIZE;
	}
	if(Mix_OpenAudio(output_sampling_rate,
			 tv_audio_sdl_format_from_depth(
				 output_bit_depth),
			 1,
			 output_chunk_size) < 0){
		print("cannot open audio:" + (std::string)(Mix_GetError()), P_ERR);
	}
}

/*
  I'm assuming that Mix_Chunk uses a 16-bit default depth
 */

static void tv_audio_test_sound(){
	Mix_Chunk *chunk =
		new Mix_Chunk;
	chunk->allocated = 0;
	chunk->alen = 44100*1;
	chunk->abuf = new uint8_t[chunk->alen];
	chunk->volume = MIX_MAX_VOLUME;
	memset(chunk->abuf, 0, chunk->alen);
	for(uint64_t i = 0;i < chunk->alen;i++){
		chunk->abuf[i] = true_rand(0, 255);
	}
	Mix_PlayChannel(-1, chunk, 0);
	print("playing channel", P_NOTE);
	sleep_ms(1000);
	delete[] chunk->abuf;
	chunk->abuf = nullptr;
	delete chunk;
	chunk = nullptr;
}

void tv_audio_loop(){
	tv_audio_test_sound();
}

void tv_audio_close(){
}
