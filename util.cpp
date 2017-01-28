#include "main.h"
#include "util.h"
#include "file.h"
#include "lock.h"
#include "settings.h"
#include "net/net.h"
#include "convert.h"

std::vector<std::string> newline_to_vector(std::string data){
	std::vector<std::string> retval;
	unsigned long int old_pos = 0;
	for(unsigned int i = 0;i < data.size();i++){
		if(data[i] == '\n'){
			const std::string tmp =
				data.substr(old_pos, i-old_pos);
			old_pos = i+1; // skip newline
			retval.push_back(tmp);
		}
	}
	return retval;

}

void sleep_ms(int ms, bool force){
	if(force){
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}else{
		for(int i = 0;i < ms;i++){
			/*
			  when the program needs to quit quickly for whatever
			  reason, this stops the sleep and lets the program run.
			  maybe it should throw an exception?
			 */
			const auto sleep_time = std::chrono::milliseconds(1);
			std::this_thread::sleep_for(sleep_time);
			if(running == false){
				return;
			}
		}
	}
}

int search_for_argv(std::string value){
	for(int i = 0;i < argc;i++){
		if(std::strcmp(argv[i], value.c_str()) == 0){
			return i;
		}
	}
	return -1;
}

static std::string print_level_text(int level){
	switch(level){
	case P_SPAM:
		return "[SPAM]";
	case P_DEBUG:
		return "[DEBUG]";
	case P_NOTICE:
		return "[NOTICE]";
	case P_WARN:
		return "[WARN]";
	case P_ERR:
		return "[ERROR]";
	case P_CRIT:
		return "[CRITICAL]";
	}
	throw std::runtime_error("invalid print level");
}

static int print_level = P_SPAM;

void print(std::string data, int level, const char *func){
	if(print_level == P_SPAM){
		try{
			print_level =
				std::stoi(
					settings::get_setting(
						"print_level"));
		}catch(...){}
	}
	if(unlikely(level >= print_level)){
		std::string func_;
		if(func != nullptr){
			func_ = func;
		}
		std::cout << print_level_text(level) << " "
			  << " " << data << std::endl;
		if(level == P_CRIT){
			std::cerr << "CRITICAL ERROR" << std::endl;
		}
		if(level >= P_ERR){
			throw std::runtime_error(data);
		}
	}
}

/*
  Should I use a real-time currency conversion API or just use different basic
  APIs like this for each currency? I guess this is good enough until this has
  some major adoption, or at least interest, because adoption can be hindered.
 */

long double get_btc_rate(std::string currency){
	if(currency == "USD" || currency == "usd"){
		int stale_time = 30; // good baseline
		try{
			const std::string stale_str = 
				settings::get_setting("btc_rate_stale_time");
			stale_time = std::stoi(stale_str);
		}catch(std::exception e){
			pre_pro::exception(e, "get_btc_rate_settings", P_ERR);
		}
		const std::string str =
			net::get("https://blockchain.info/q/24hrprice",
				stale_time);
		long double retval = 0;
		try{
			retval = std::stold(str);
		}catch(std::exception e){
			pre_pro::exception(e, "get_btc_rate", P_ERR);
		}
		return retval;
	}else{
		print("your plebian currency isn't supported yet", P_CRIT);
		return 0; // complains about no retval
	}
}

/*
  TODO:	rewrite a lot of this
 */


static std::string to_lower(std::string a){
	std::string retval = a;
	for(int i = 0;i < (int)retval.size();i++){
		retval[i] = std::tolower(retval[i]);
	}
	return retval;
}

long double get_mul_to_btc(std::string currency){
	long double mul = -1;
	if(to_lower(currency) == "satoshi"){
		mul = 1.0/100000000;
	}else if(to_lower(currency) == "cbtc"){
		mul = 1.0/100;
	}else if(to_lower(currency) == "mbtc"){
		mul = 1.0/1000;
	}else if(to_lower(currency) == "ubtc" || to_lower(currency) == "bit"){
		mul = 1.0/1000000;
	}else if(to_lower(currency) == "btc"){
		mul = 1.0;
	}else{
		throw std::runtime_error("invalid currency");
	}
	if(mul == 0){
		throw std::logic_error("mul == 0");
	}
	if(mul < 0){
		throw std::logic_error("mul < 0");
	}
	return mul;
}

void pre_pro::unable(std::string from, std::string to, int level){
	print("unable to get " + to + " from " + from,  level);
}

void pre_pro::exception(std::exception e, std::string for_, int level){
	print((std::string)e.what() + " for " + for_, level); 
}

/*
  array_func::add: generic function for adding data to an std::array.
  Returns the current position on success
  Returns array_size*data_size on failure (falls beyond array, from zero)

  TODO: make this more readable?
 */

uint64_t array_func::add(void* array,
			 uint64_t array_size,
			 void *data,
			 uint64_t data_size){
	uint8_t *byte_array = (uint8_t*)array;
	// assume the type referenced in data_size is the same type used
	// in the array
	for(int64_t i = array_size*data_size;i >= 0;i -= data_size){
		bool blank = true;
		for(uint64_t byte = 0;byte < data_size-1;byte++){
			if(byte_array[i+byte] != 0){
				blank = false;
				break;
			}
		}
		if(blank){
			continue;
		}
		// i is the current position in bytes
		const uint64_t curr_byte_pos = i;
		memcpy(byte_array+curr_byte_pos+data_size, data, data_size);
		return curr_byte_pos/data_size;
	}
	return array_size*data_size;
}

void throw_on_null(void* ptr){
	if(ptr == nullptr){
		throw std::runtime_error(__FUNCTION__);
	}
}

uint64_t true_rand(uint64_t min, uint64_t max){
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> dist6(min, max);
	return dist6(rng);
}
uint64_t flip_bit_section(uint8_t begin, uint8_t end){
	if(unlikely(end == begin)){
		return 0;
		/*
		  This program actually flips all of the bits but the last one, 
		  carries them over one, and ORs a 1 at the end, before shifting
		  it again to the beginning. If there is no section, it would
		  falsely report the mask as one
		 */
	}
#ifdef IS_BIG_ENDIAN
	return ((((1 << end-begin-1)-1) << 1) | 1) << begin;
#else
	// SPARC64 didn't work with previous
	uint64_t retval = 0;
	for(uint64_t i = begin;i < end;i++){
		retval |= (1 << i);
	}
	return retval;
#endif
}

uint64_t get_time_microseconds(){
	const std::chrono::microseconds micro_s =
		std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::system_clock::now().time_since_epoch());
	return micro_s.count();
}

// get some solid variables for rw and locality?
void prefetch_range(void *addr, uint32_t range){
	const uint8_t *addr_ = (uint8_t*)addr;
	const uint8_t *end_ = addr_ + range;
	for(;addr_ < end_;addr_ += PREFETCH_STRIDE){
		prefetch(addr_, 0, 0);
	}
}

std::string fix_to_length(std::string string, uint64_t size){
	if(string.size() < size){
		string += std::string(size-string.size(), ' ');
	}
	return string;
}


std::string to_hex(uint8_t s){
	std::stringstream ss;
	ss << std::hex << (int32_t)s;
	return ss.str();
}

