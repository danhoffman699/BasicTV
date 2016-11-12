#include "tv_frame.h"
#ifndef TV_MENU_H
#define TV_MENU_H
#include "array"
struct tv_menu_t{
private:
	std::array<std::string, 64> menu_entries;
	uint16_t highlighed = 0;
	uint64_t frame_id = 0;
	void update_frame();
public:
	data_id_t id;
	tv_menu_t();
	~tv_menu_t();
	void set_menu_entry(uint16_t entry,
			    std::string string);
	std::string get_menu_entry(uint16_t entry);
	void set_highlighed(uint16_t highlighted_);
	uint64_t get_frame_id();
};
#endif
