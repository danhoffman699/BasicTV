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

#define GLYPH_X 6
#define GLYPH_Y 8

static std::array<std::array<std::array<bool, GLYPH_X>, GLYPH_Y>, 256> char_data = {{{{{{0}}}}}};

static bool valid_char_data = false;

static void init_char_data(){
	char_data['A'] = {{ 
			{{0, 0, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0, 0}}
		}};
	char_data['B'] = {{
			{{0, 1, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 0, 0}}
		}};
	char_data['C'] = {{
			{{0, 0, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 1, 0}}
		}};
	char_data['D'] = {{ 
			{{0, 1, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 1, 0}}
		}};
	char_data['E'] = {{
			{{0, 1, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 1, 1, 1, 0}}
		}};
	char_data['F'] = {{
			{{0, 1, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}}
		}};
	char_data['G'] = {{ 
			{{0, 1, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 1, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 1, 0}}
		}};
	char_data['H'] = {{ 
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}}
		}};
	char_data['I'] = {{ 
			{{0, 1, 1, 1, 1, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 1, 1, 1, 1, 0}}
		}};
	char_data['J'] = {{
			{{0, 1, 1, 1, 1, 0}},
			{{0, 0, 0, 1, 0, 0}},
			{{0, 0, 0, 1, 0, 0}},
			{{0, 1, 0, 1, 0, 0}},
			{{0, 1, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0, 0}}
		}};
	char_data['K'] = {{ 
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 1, 0, 0}},
			{{0, 1, 1, 1, 0, 0}},
			{{0, 1, 1, 1, 0, 0}},
			{{0, 1, 0, 1, 1, 0}},
			{{0, 1, 0, 0, 1, 0}}
		}};
	char_data['L'] = {{ 
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 1, 1, 1, 0}}
		}};
	char_data['M'] = {{ 
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}}
		}};
	char_data['N'] = {{ 
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 0, 1, 0}},
			{{0, 1, 1, 0, 1, 0}},
			{{0, 1, 0, 1, 1, 0}},
			{{0, 1, 0, 1, 1, 0}},
			{{0, 1, 0, 0, 1, 0}}
		}};
	char_data['O'] = {{ 
			{{0, 0, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 0, 1, 1, 0, 0}}
		}};
	char_data['P'] = {{
			{{0, 1, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 1, 0, 0, 0, 0}}
		}};
	char_data['Q'] = {{
			{{0, 0, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 1, 0}}
		}};
	char_data['R'] = {{
			{{0, 1, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 0, 0}},
			{{0, 1, 1, 0, 0, 0}},
			{{0, 1, 0, 1, 0, 0}},
			{{0, 1, 0, 0, 1, 0}}
		}};
	char_data['S'] = {{
			{{0, 0, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 0, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 0, 0, 0, 1, 0}},
			{{0, 0, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 0, 0}}
		}};
	char_data['T'] = {{
			{{0, 1, 1, 1, 1, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 0, 1, 0, 0, 0}}
		}};
	char_data['U'] = {{
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 0, 1, 1, 0, 0}}
		}};
	char_data['V'] = {{
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 0, 1, 0, 1, 0}},
			{{0, 0, 0, 1, 0, 0}}
		}};
	char_data['W'] = {{
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 1, 0}}
		}};
	char_data['X'] = {{
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}}
		}};
	char_data['Y'] = {{
			{{0, 1, 0, 0, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 0, 1, 1, 0, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 0, 1, 0, 0, 0}}
		}};
	char_data['Z'] = {{
			{{0, 1, 1, 1, 1, 0}},
			{{0, 0, 0, 1, 0, 0}},
			{{0, 0, 0, 1, 0, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 0, 1, 0, 0, 0}},
			{{0, 1, 1, 1, 1, 0}}
		}};
	char_data['.'] = {{
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 1, 1, 0}},
			{{0, 0, 0, 1, 1, 0}}
		}};
	char_data['!'] = {{
			{{0, 0, 0, 1, 1, 0}},
			{{0, 0, 0, 1, 1, 0}},
			{{0, 0, 0, 1, 1, 0}},
			{{0, 0, 0, 1, 1, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 1, 1, 0}}
		}};
	char_data['?'] = {{
			{{0, 1, 1, 1, 1, 0}},
			{{0, 1, 0, 0, 1, 0}},
			{{0, 0, 0, 1, 1, 0}},
			{{0, 0, 0, 1, 0, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 1, 0, 0}}
		}};
	char_data[':'] = {{
			{{0, 0, 0, 1, 1, 0}},
			{{0, 0, 0, 1, 1, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 0, 0, 0}},
			{{0, 0, 0, 1, 1, 0}},
			{{0, 0, 0, 1, 1, 0}}
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
					  uint64_t start_x,
					  uint64_t start_y){
	for(uint64_t x = 0;x < GLYPH_X;x++){
		for(uint64_t y = 0;y < GLYPH_Y;y++){
			const uint64_t frame_x =
				start_x+x;
			const uint64_t frame_y =
				start_y+y;
			const uint64_t glyph_x =
				x;
			const uint64_t glyph_y =
				y;
			const int8_t map_entry =
				char_data[glyph][glyph_y][glyph_x];
			if(map_entry == 0){
				continue;
			}
			frame->set_pixel(frame_x,
					 frame_y,
					 std::make_tuple(
						 255,
						 255,
						 255,
						 8));
		}
	}
}

static void tv_menu_get_glyph_dimensions(std::array<uint64_t, TV_MENU_TEXT_LENGTH> ids,
					 uint64_t *x,
					 uint64_t *y){
	for(uint64_t i = 0;i < ids.size();i++){
		tv_menu_entry_t *entry_ =
			PTR_DATA(ids[i], tv_menu_entry_t);
		if(entry_ == nullptr){
			*y = i;
			return;
		}
		if(entry_->get_text().size() > *x){
			*x = entry_->get_text().size();
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
	tv_menu_get_glyph_dimensions(entry,
				     &x_count,
				     &y_count);
	frame->reset(x_count*GLYPH_X,
		     y_count*GLYPH_Y,
		     TV_FRAME_DEFAULT_BPC,
		     TV_FRAME_DEFAULT_RED_MASK,
		     TV_FRAME_DEFAULT_GREEN_MASK,
		     TV_FRAME_DEFAULT_BLUE_MASK,
		     TV_FRAME_DEFAULT_ALPHA_MASK,
		     1000*1000,
		     1,
		     1, // don't have any audio
		     1);
	for(uint64_t y = 0;y < y_count;y++){
		tv_menu_entry_t *entry_ =
			PTR_DATA(entry[y], tv_menu_entry_t);
		CONTINUE_IF_NULL(entry_);
		const std::string text =
			entry_->get_text();
		for(uint64_t x = 0;x < text.size();x++){
			tv_menu_render_glyph_to_frame(text[x],
						      frame,
						      x*GLYPH_X,
						      y*GLYPH_Y);
		}
	}
}

void tv_menu_t::set_menu_entry(uint16_t entry_pos,
			       std::string string){
	if(never(entry_pos >= 64)){
		print("entry_pos is larger than the max", P_CRIT);
	}
	tv_menu_entry_t *entry_ = nullptr;
	if(entry[entry_pos] == 0){
		entry_ = new tv_menu_entry_t;
		entry[entry_pos] = entry_->id.get_id();
	}else{
		entry_ =
			PTR_DATA(entry[entry_pos], tv_menu_entry_t);
	}
	if(unlikely(entry_ == nullptr)){
		print("entry is nullptr", P_ERR);
	}
	entry_->set_text(string);
	update_frame();
}

std::string tv_menu_t::get_menu_entry(uint16_t entry_pos){
	if(never(entry_pos >= 64)){
		print("entry is larger than the max", P_CRIT);
	}
	tv_menu_entry_t *entry_ =
		PTR_DATA(entry[entry_pos], tv_menu_entry_t);
	if(entry_ == nullptr){
		print("not a valid entry", P_WARN);
		return "";
	}
	return entry_->get_text();
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

tv_menu_entry_t::tv_menu_entry_t() : id(this, __FUNCTION__){
}

tv_menu_entry_t::~tv_menu_entry_t(){}

void tv_menu_entry_t::set_text(std::string text_){
	text = text_;
}

std::string tv_menu_entry_t::get_text(){
	return text;
}

void tv_menu_entry_t::set_function(void (*function_)()){
	function = function_;
}

void tv_menu_entry_t::run_function(){
	if(likely(function != nullptr)){
		function();
	}
}
