#include "tv_frame_standard.h"
#include "tv_frame_video.h"
#ifndef TV_MENU_H
#define TV_MENU_H
#include "array"
#define TV_MENU_TEXT_LENGTH 64
// TODO: define this in settings, not as macros
#define TV_MENU_WIDTH 16
#define TV_MENU_HEIGHT 16
#define TV_MENU_ORIENT_LEFT 0
#define TV_MENU_ORIENT_MIDDLE 1
#define TV_MENU_ORIENT_CENTER 1
#define TV_MENU_ORIENT_RIGHT 2
struct tv_menu_entry_t{
private:
	std::string text;
	uint8_t orientation = TV_MENU_ORIENT_LEFT;
	void (*function)(void) = nullptr;
public:
	data_id_t id;
	tv_menu_entry_t();
	~tv_menu_entry_t();
	uint8_t get_orientation();
	void set_orientation(uint8_t orientation_);
	void set_text(std::string text_);
	std::string get_text();
	void set_function(void (*function_)(void));
	void run_function();
};
struct tv_menu_t{
private:
	std::array<uint64_t, 64> entry = {{0}};
	uint16_t highlighed = 0;
	uint64_t frame_id = 0;
	void update_frame();
public:
	data_id_t id;
	tv_menu_t();
	~tv_menu_t();
	void set_menu_entry(uint16_t entry_,
			    std::string string);
	std::string get_menu_entry(uint16_t entry);
	void set_highlighed(uint16_t highlighted_);
	uint64_t get_frame_id();
};
#endif
