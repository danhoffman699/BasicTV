#ifndef TV_FRAME_STANDARD_H
#define TV_FRAME_STANDARD_H
#define TV_FRAME_DEFAULT_FREQ 60
#define TV_FRAME_DEFAULT_TTL (1/TV_FRAME_DEFAULT_FREQ)
class tv_frame_standard_t{
private:
	uint64_t start_time_micro_s = 0;
	uint32_t ttl_micro_s = 0;
	uint64_t frame_entry = 0;
	std::array<uint64_t, 256> dep = {{0}};
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
	bool valid(uint64_t timestamp_micro_s);
};
#endif
