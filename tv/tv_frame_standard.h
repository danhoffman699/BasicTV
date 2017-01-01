#include "../id/id_api.h"
#ifndef TV_FRAME_STANDARD_H
#define TV_FRAME_STANDARD_H
#define TV_FRAME_DEFAULT_FREQ 60
#define TV_FRAME_DEFAULT_TTL (1/TV_FRAME_DEFAULT_FREQ)
#define TV_FRAME_DEP_SIZE 256
class tv_frame_standard_t{
private:
	uint64_t start_time_micro_s = 0;
	uint32_t ttl_micro_s = 0;
	uint64_t frame_entry = 0;
	// dependencies on other frames, read in order and used for
	// Opus and VP9 encoding
	std::array<id_t_, TV_FRAME_DEP_SIZE> dep = {{0}};
public:
	tv_frame_standard_t();
	~tv_frame_standard_t();
	void list_virtual_data(data_id_t *id);
	void set_standard(uint64_t start_time_micro_s_,
			  uint32_t ttl_micro_s_,
			  uint64_t frame_entry_);
	void get_standard(uint64_t *start_time_micro_s_,
			  uint32_t *ttl_micros_,
			  uint64_t *frame_entry_);
	void add_dep(id_t_ id_);
	void del_dep(id_t_ id_);
	bool valid(uint64_t timestamp_micro_s);
	uint64_t get_start_time_micro_s(){return start_time_micro_s;}
	uint64_t get_ttl_micro_s(){return ttl_micro_s;}
	uint64_t get_end_time_micro_s(){return get_start_time_micro_s()+get_ttl_micro_s();}
};
template<typename T>
id_t_ tv_frame_scroll_to_time(T data, uint64_t play_time){
	while(data != nullptr){
		const uint64_t start_time_micro_s =
			data->get_start_time_micro_s();
		const uint64_t end_time_micro_s =
			data->get_end_time_micro_s();
		const uint64_t ttl_micro_s =
			data->get_ttl_micro_s();
		const bool stay =
			BETWEEN(start_time_micro_s,
				play_time,
				end_time_micro_s);
		id_t_ new_id = 0;
		if(stay){
			return data->id.get_id();
		}else{
			const bool go_forward =
				(play_time > end_time_micro_s);
			if(go_forward){
				new_id = data->id.get_next_linked_list();
			}else{
				new_id = data->id.get_prev_linked_list();
			}
		}
		data_id_t *new_id_ptr =
			id_api::array::ptr_id(new_id, "");
		if(new_id_ptr != nullptr){
			data = (T)new_id_ptr->get_ptr();
		}else{
			data = nullptr;
		}
	}
	// only out that isn't valid is data = PTR_ID...
	return 0;
}
#endif
