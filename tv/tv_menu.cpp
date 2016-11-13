#include "tv_menu.h"
#include "tv_frame.h"

/*
  tv_menu: a standardized menu setup. I want this program to be as simple,
  but as versatile as possible. I plan on hard coding all of the ASCII, and
  trying to make other small characters work (appending 'e' to the end of an
  umlatued word, esset to 'ss', etc.). I don't plan on supporting Russian, 
  Chinese, Korean, or any other wonky alphabets with hardcoding, but instead
  load TrueType fonts. However, there isn't anything I won't try to hard-code
  if somebody requests it.

  The glyph size is going to increase with more advanced symbols, but it should
  stay roughly the same for the rest of the ASCII alphabet. I don't plan on 
  having different sizes of the same font, but just increase the size of the 
  one font through blitting (the entire menu will be expanded out to the screen,
  and it starts as a 512x512 (64x64 characters at the planned max size of 8x8).
 */

// defined as an 4x8, only two colors for now
// the last two are for characters that reach below the line, making the
// practical size 4x6
#define GLYPH_X 4
#define GLYPH_X_BORDER 1
#define GLYPH_Y_MAIN 6
#define GLYPH_Y 8
#define GLYPH_Y_BORDER 1
static std::array<std::array<std::array<bool, GLYPH_X>, GLYPH_Y>, 256> char_data = {{{{{{0}}}}}};

static bool valid_char_data = false;

static void init_char_data(){
	char_data['A'] = {{
			{{0, 1, 1, 0}},
			{{1, 0, 0, 1}},
			{{1, 1, 1, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{0, 0, 0, 0}},
			{{0, 0, 0, 0}}
		}};
	char_data['B'] = {{
			{{1, 1, 1, 0}},
			{{1, 0, 0, 1}},
			{{1, 1, 1, 0}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 1, 1, 0}}
		}};
	char_data['C'] = {{
			{{0, 1, 1, 1}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{0, 1, 1, 1}}
		}};
	char_data['D'] = {{
			{{1, 1, 1, 0}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 1, 1, 1}}
		}};
	char_data['E'] = {{
			{{1, 1, 1, 1}},
			{{1, 0, 0, 0}},
			{{1, 1, 1, 1}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{1, 1, 1, 1}}
		}};
	char_data['F'] = {{
			{{1, 1, 1, 1}},
			{{1, 0, 0, 0}},
			{{1, 1, 1, 0}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}}
		}};
	char_data['G'] = {{
			{{1, 1, 1, 1}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{1, 0, 1, 1}},
			{{1, 0, 0, 1}},
			{{1, 1, 1, 1}}
		}};
	char_data['H'] = {{
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 1, 1, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}}
		}};
	char_data['I'] = {{
			{{1, 1, 1, 1}},
			{{0, 1, 0, 0}},
			{{0, 1, 0, 0}},
			{{0, 1, 0, 0}},
			{{0, 1, 0, 0}},
			{{1, 1, 1, 1}}
		}};
	char_data['J'] = {{
			{{1, 1, 1, 1}},
			{{0, 0, 1, 0}},
			{{0, 0, 1, 0}},
			{{1, 0, 1, 0}},
			{{1, 0, 1, 0}},
			{{0, 1, 0, 0}}
		}};
	char_data['K'] = {{
			{{1, 0, 0, 1}},
			{{1, 0, 1, 0}},
			{{1, 1, 1, 0}},
			{{1, 1, 1, 0}},
			{{1, 0, 1, 1}},
			{{1, 0, 0, 1}}
		}};
	char_data['L'] = {{
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{1, 1, 1, 1}}
		}};
	char_data['M'] = {{
			{{1, 0, 0, 1}},
			{{1, 1, 1, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}}
		}};
	char_data['N'] = {{
			{{1, 0, 0, 1}},
			{{1, 1, 0, 1}},
			{{1, 1, 0, 1}},
			{{1, 0, 1, 1}},
			{{1, 0, 1, 1}},
			{{1, 0, 0, 1}}
		}};
	char_data['O'] = {{
			{{0, 1, 1, 0}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{0, 1, 1, 0}}
		}};
	char_data['P'] = {{
			{{1, 1, 1, 0}},
			{{1, 0, 0, 1}},
			{{1, 1, 1, 0}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}},
			{{1, 0, 0, 0}}
		}};
	char_data['Q'] = {{
			{{0, 1, 1, 0}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 1, 0}},
			{{0, 1, 0, 1}}
		}};
	char_data['R'] = {{
			{{1, 1, 1, 0}},
			{{1, 0, 0, 1}},
			{{1, 1, 1, 0}},
			{{1, 1, 0, 0}},
			{{1, 0, 1, 0}},
			{{1, 0, 0, 1}}
		}};
	char_data['S'] = {{
			{{0, 1, 1, 1}},
			{{1, 0, 0, 0}},
			{{0, 1, 1, 0}},
			{{0, 0, 0, 1}},
			{{0, 0, 0, 1}},
			{{1, 1, 1, 0}}
		}};
	char_data['T'] = {{
			{{1, 1, 1, 1}},
			{{0, 0, 1, 0}},
			{{0, 0, 1, 0}},
			{{0, 0, 1, 0}},
			{{0, 0, 1, 0}},
			{{0, 0, 1, 0}}
		}};
	char_data['U'] = {{
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{0, 1, 1, 0}}
		}};
	char_data['V'] = {{
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{0, 1, 0, 1}},
			{{0, 0, 1, 0}}
		}};
	char_data['W'] = {{
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{1, 1, 1, 1}},
			{{1, 0, 0, 1}}
		}};
	char_data['X'] = {{
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{0, 1, 1, 0}},
			{{0, 1, 1, 0}},
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}}
		}};
	char_data['Y'] = {{
			{{1, 0, 0, 1}},
			{{1, 0, 0, 1}},
			{{0, 1, 1, 0}},
			{{0, 1, 0, 0}},
			{{0, 1, 0, 0}},
			{{0, 1, 0, 0}}
		}};
	char_data['Z'] = {{
			{{1, 1, 1, 1}},
			{{0, 0, 1, 0}},
			{{0, 0, 1, 0}},
			{{0, 1, 0, 0}},
			{{0, 1, 0, 0}},
			{{1, 1, 1, 1}}
		}};
	char_data['.'] = {{
			{{0, 0, 0, 0}},
			{{0, 0, 0, 0}},
			{{0, 0, 0, 0}},
			{{0, 0, 0, 0}},
			{{0, 0, 1, 1}},
			{{0, 0, 1, 1}}
		}};
	char_data['!'] = {{
			{{0, 0, 1, 1}},
			{{0, 0, 1, 1}},
			{{0, 0, 1, 1}},
			{{0, 0, 1, 1}},
			{{0, 0, 0, 0}},
			{{0, 0, 1, 1}}
		}};
	char_data['?'] = {{
			{{1, 1, 1, 1}},
			{{1, 0, 0, 1}},
			{{0, 0, 1, 1}},
			{{0, 0, 1, 0}},
			{{0, 0, 0, 0}},
			{{0, 0, 1, 0}}
		}};
	char_data[':'] = {{
			{{0, 0, 1, 1}},
			{{0, 0, 1, 1}},
			{{0, 0, 0, 0}},
			{{0, 0, 0, 0}},
			{{0, 0, 1, 1}},
			{{0, 0, 1, 1}}
		}};
	// probably shouldn't do this
	char_data['a'] = char_data['A'];
	char_data['b'] = char_data['B'];
	char_data['c'] = char_data['C'];
	char_data['d'] = char_data['D'];
	char_data['e'] = char_data['E'];
	char_data['f'] = char_data['F'];
	char_data['g'] = char_data['G'];
	char_data['h'] = char_data['H'];
	char_data['i'] = char_data['I'];
	char_data['j'] = char_data['J'];
	char_data['k'] = char_data['K'];
	char_data['l'] = char_data['L'];
	char_data['m'] = char_data['M'];
	char_data['n'] = char_data['N'];
	char_data['o'] = char_data['O'];
	char_data['p'] = char_data['P'];
	char_data['q'] = char_data['Q'];
	char_data['r'] = char_data['R'];
	char_data['s'] = char_data['S'];
	char_data['t'] = char_data['T'];
	char_data['u'] = char_data['U'];
	char_data['v'] = char_data['V'];
	char_data['w'] = char_data['W'];
	char_data['x'] = char_data['X'];
	char_data['y'] = char_data['Y'];
	char_data['z'] = char_data['Z'];
	valid_char_data = true;
}

tv_menu_t::tv_menu_t() : id(this, __FUNCTION__){
	id.add_data(&frame_id, sizeof(frame_id));
	id.add_id(&frame_id, 1);
	frame_id = (new tv_frame_t)->id.get_id();
	update_frame();
	if(unlikely(valid_char_data == false)){
		init_char_data();
	}
}

tv_menu_t::~tv_menu_t(){
}

static void tv_menu_render_glyph_to_frame(int8_t glyph,
					  tv_frame_t *frame,
					  uint64_t x_,
					  uint64_t y_){
	for(uint64_t x = 0;x < GLYPH_X;x++){
		for(uint64_t y = 0;y < GLYPH_Y;y++){
			const uint64_t frame_x =
				x+x_;
			const uint64_t frame_y =
				y+y_;
			const uint64_t glyph_x =
				x;
			const uint64_t glyph_y =
				y;
			const int8_t map_entry =
				char_data[glyph][glyph_y][glyph_x];
			std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color =
				std::make_tuple(0, 0, 0, 8);
			if(map_entry != 0){
				color = std::make_tuple(
					255,
					255,
					255,
					8);
			} // TODO: add alpha channel, at least in menu settings
			frame->set_pixel(frame_x, frame_y, color);
		}
	}
}

void tv_menu_t::update_frame(){
	tv_frame_t *frame =
		PTR_DATA(frame_id, tv_frame_t);
	if(never(frame == nullptr)){
		print("menu frame is a nullptr", P_CRIT);
	}
	uint64_t x_count = 0;
	uint64_t y_count = 0;
	for(uint64_t i = 0;i < 64;i++){
		if(menu_entries[i].size() == 0){
			y_count = i;
			break;
		}
		if(menu_entries[i].size() > x_count){
			x_count = menu_entries[i].size();
		}
	}
	
	const uint64_t x_res_mul =
		GLYPH_X + 2*GLYPH_X_BORDER;
	const uint64_t y_res_mul =
		GLYPH_Y + 2*GLYPH_Y_BORDER;
	frame->reset(x_count*x_res_mul,
		     y_count*y_res_mul,
		     TV_FRAME_DEFAULT_BPC,
		     TV_FRAME_DEFAULT_RED_MASK,
		     TV_FRAME_DEFAULT_GREEN_MASK,
		     TV_FRAME_DEFAULT_BLUE_MASK,
		     0,// no alpha mask needed
		     0, // not a typical frame, doesn't need a frame rate
		     1,
		     1, // don't have any audio
		     1);
	for(uint64_t line = 0;line < y_count;line++){
		const uint64_t y_pos =
			line*y_res_mul;
		for(uint64_t curr_char = 0;
		    curr_char < menu_entries[line].size();
		    curr_char++){
			const uint64_t x_pos =
				curr_char*x_res_mul;
			tv_menu_render_glyph_to_frame(menu_entries[line][curr_char],
						      frame,
						      x_pos,
						      y_pos);
		}
	}
}

void tv_menu_t::set_menu_entry(uint16_t entry,
			       std::string string){
	if(never(entry >= 64)){
		print("entry is larger than the max", P_CRIT);
	}
	menu_entries[entry] = string;
	update_frame();
}

std::string tv_menu_t::get_menu_entry(uint16_t entry){
	if(never(entry >= 64)){
		print("entry is larger than the max", P_CRIT);
	}
	return menu_entries[entry];
}

void tv_menu_t::set_highlighed(uint16_t highlighted_){
	if(never(highlighted >= 64)){
		print("highlighed_ is larger than the max", P_CRIT);
	}
	highlighed = highlighted_;
}

uint64_t tv_menu_t::get_frame_id(){
	return frame_id;
}
