#ifndef COMPRESS_H
#define COMPRESS_H
#include "vector"
#include "zlib.h"
namespace compressor{
	std::vector<uint8_t> to_xz(
		std::vector<uint8_t> input,
		uint8_t compression_level);
	std::vector<uint8_t> from_xz(
		std::vector<uint8_t> input);
};
#endif
