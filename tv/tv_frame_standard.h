#ifndef TV_FRAME_STANDARD_H
#define TV_FRAME_STANDARD_H
class tv_frame_standard_t{
private:
	// timestamp of the start time, just add
	// TTL to this to get end time
	uint64_t start_time_micro_s = 0;
	uint32_t ttl_micro_s = 0;
	// just a simple incrementer
	uint64_t frame_entry = 0;
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
	bool valid();
};
#endif
