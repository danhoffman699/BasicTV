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
};
#endif
