#include "main.h"
#include "util.h"
#include "compress.h"

#define COMPRESS_TMP_SANE_SIZE (1024*1024)

// compression_level is currently unused

std::vector<uint8_t> compressor::to_xz(std::vector<uint8_t> input,
				     uint8_t compression_level){
	uint64_t retval_size = (input.size()*1.1)+12;
	uint8_t *retval_char = new uint8_t[retval_size];
	switch(compress(
		       retval_char,
		       &retval_size,
		       &(input[0]),
		       input.size())){
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
	std::vector<uint8_t> retval;
	for(uint64_t i = 0;i < retval_size;i++){ // changed by compress()
		retval.push_back(retval_char[i]);
	}
	delete[] retval_char;
	retval_char = nullptr;
	return retval;
}

std::vector<uint8_t> compressor::from_xz(std::vector<uint8_t> input){
	uint64_t retval_size = COMPRESS_TMP_SANE_SIZE;
	uint8_t *retval_char = new uint8_t[COMPRESS_TMP_SANE_SIZE]; // does zlib take care of that?
	switch(uncompress(retval_char,
			  &retval_size,
			  &(input[0]),
			  input.size())){
	case Z_OK:
		break;
	case Z_MEM_ERROR:
		print("out of memory", P_ERR);
		break;
	case Z_BUF_ERROR:
		print("output buffer wasn't large enough", P_ERR);
		break;
	}
	std::vector<uint8_t> retval;
	for(uint64_t i = 0;i < retval_size;i++){
		retval.push_back(retval_char[i]);
	}
	delete[] retval_char;
	retval_char = nullptr;
	return retval;
}
