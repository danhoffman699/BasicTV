#include "tv_menu.h"
#include "tv_frame.h"

// defined as an 4x8, only two colors for now
// the last two are for characters that reach below the line, making the
// practical size 4x6
static std::array<std::array<std::array<bool, 4>, 8>, 256> char_data = {{{{{{0}}}}}};

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
			{{1, 1, 1, 1}}		
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
			{{1, 1, 1, 1}},
			{{1, 0, 0, 1}},
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
			{{1, 1, 1, 1}},
			{{1, 0, 1, 1}},
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
	char_data['0'] = {{
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
			{{0, 1, 0, 0}},
			{{0, 1, 0, 0}},
			{{0, 1, 0, 0}},
			{{0, 1, 0, 0}},
			{{0, 1, 0, 0}}
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
			{{1, 1, 0, 1}},
			{{1, 1, 1, 1}},
			{{0, 1, 0, 1}}
		}};
	char_data['X'] = {{
			{{1, 0, 0, 1}},
			{{0, 1, 1, 0}},
			{{0, 0, 1, 0}},
			{{0, 1, 0, 0}},
			{{0, 1, 1, 0}},
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
}

tv_menu_t::tv_menu_t() : id(this, __FUNCTION__){
	id.add_data(&frame_id, sizeof(frame_id));
	id.add_id(&frame_id, 1);
	frame_id = (new tv_frame_t)->id.get_id();
}

tv_menu_t::~tv_menu_t(){
}

void tv_menu_t::update_frame(){
	tv_frame_t *frame =
		PTR_DATA(frame_id, tv_frame_t);
	if(never(frame == nullptr)){
		print("menu frame is a nullptr", P_CRIT);
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
	update_frame();
}

uint64_t tv_menu_t::get_frame_id(){
	return frame_id;
}
