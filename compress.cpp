#include "main.h"
#include "util.h"
#include "compress.h"

/*
  TODO: get rid of COMPRESS_TMP_SANE_SIZE and actually use something that makes
  more sense.
 */

static void compressor_zlib_error_checker(int32_t retval){
	switch(retval){
	case Z_OK:
		break;
	case Z_MEM_ERROR:
		print("can't allocate memory", P_ERR);
		break;
	case Z_BUF_ERROR:
		print("output buffer wasn't large enough", P_ERR);
		break; // this is fixable
	default:
		print("unknown error", P_ERR);
		break;
	}
}

// TODO: actually use compression_level

std::vector<uint8_t> compressor::to_xz(std::vector<uint8_t> input,
				       uint8_t compression_level){
	uint64_t retval_size = (input.size()*1.1)+12;
	uint8_t *retval_char = new uint8_t[retval_size];
	compressor_zlib_error_checker(
		compress(
			retval_char,
			&retval_size,
			&(input[0]),
			input.size()));
	std::vector<uint8_t> retval;
	for(uint64_t i = 0;i < retval_size;i++){ // changed by compress()
		retval.push_back(retval_char[i]);
	}
	delete[] retval_char;
	retval_char = nullptr;
	return retval;
}

std::vector<uint8_t> compressor::from_xz(std::vector<uint8_t> input){
	uint64_t retval_size = input.size()*3;
	uint8_t *retval_char = new uint8_t[retval_size];
	compressor_zlib_error_checker(
		uncompress(
			retval_char,
			&retval_size,
			&(input[0]),
			input.size()));
	std::vector<uint8_t> retval;
	for(uint64_t i = 0;i < retval_size;i++){
		retval.push_back(retval_char[i]);
	}
	delete[] retval_char;
	retval_char = nullptr;
	return retval;
}
