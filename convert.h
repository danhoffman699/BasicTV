#ifndef CONVERT_H
#define CONVERT_H
#include "vector"
#include "string"
#include "array"

#include "endian.h"

/*
  convert: if the conversion is simple enough to be written in plain C/C++,
  it goes here. If it needs external libraries, or needs special access
  to static variables, then it shouldn't be here (unless, of course, they
  are here first)
 */

// TODO: implement some builtin functions for VC++ and other compilers
// before opting for the slowest method

// NBO: network byte order

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define __ORDER_LITTLE_ENDIAN__
#warning Assuming little endian
#elif __BYTE_ORDER == __BIG_ENDIAN
#define __ORDER_BIG_ENDIAN__
#warning Assuming big endian
#endif

#ifdef __ORDER_LITTLE_ENDIAN__

#ifdef __GNUC__

#define NBO_64 __builtin_bswap64
#define NBO_32 __builtin_bswap32
#define NBO_16 __builtin_bswap16

#define NBO_TO_NATIVE_16 __builtin_bswap16
#define NBO_TO_NATIVE_32 __builtin_bswap32
#define NBO_TO_NATIVE_64 __builtin_bswap64
#else

#error "no converting functions for NBO"

#endif

#elif __ORDER_BIG_ENDIAN__

#define NBO_64(a) (a)
#define NBO_32(a) (a)
#define NBO_16(a) (a)

#define NBO_TO_NATIVE_16(a) (a)
#define NBO_TO_NATIVE_32(a) (a)
#define NBO_TO_NATIVE_64(a) (a)
#endif

// mostly for time, but add the other ones
#define MILLI_PREFIX (0.001)
#define MICRO_PREFIX (0.000001)
#define MILLI MILLI_PREFIX
#define MICRO MICRO_PREFIX

#define METRIC(a, b) (a/b)

#define CROP_LSB(data, bits_out) (data & ~((~0) << bits_out))

namespace convert{
	namespace nbo{
		std::vector<uint8_t> to(std::vector<uint8_t>);
		std::vector<uint8_t> to(std::string);
		void to(uint8_t *, uint64_t);
		std::vector<uint8_t> from(std::vector<uint8_t>);
		std::vector<uint8_t> from(std::string);
		void from(uint8_t *, uint64_t);
	}
	namespace array{
		namespace type{
			std::array<uint8_t, 32> to(std::string);
			std::string from(std::array<uint8_t, 32>);
		}
	}
	namespace number{
		std::string to_binary(uint64_t);
		std::string to_hex(uint64_t);
	}
	/*
	  A color tuple is the RGB values plus the bytes per color. Bytes per 
	  color is used instead of bytes per pixel because, unlike SDL2 and
	  most other libaries, there is no native support for the alpha channel
	  (but that wouldn't be a bad idea for advanced menus)
	 */
	namespace color{
		uint64_t to(std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color);
		std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> from(uint64_t color,
								       uint8_t bpc);
		std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> bpc(std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color,
								      uint8_t new_bpp);
	}
};

#endif
