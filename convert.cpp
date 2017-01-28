#include "convert.h"
#include "util.h"
#include "id/id.h"

// no reason why there can't just be convert::nbo::flip with all
// of the different data types

/*
  Instead of always doing things in 8-bit blocks, try and create
  larger blocks of data (16, 32, or 64), but that isn't a concern
  now.
*/

void convert::nbo::to(uint8_t *data, uint64_t size){
#ifdef IS_LITTLE_ENDIAN
	switch(size){
	case 2:
		*data = NBO_16(*data);
		return;
	case 4:
		*data = NBO_32(*data);
		return;
	case 8:
		*data = NBO_64(*data);
		return;
	}
	for(uint64_t i = 0;i < size/2;i++){
		const uint64_t first = i;
		const uint64_t second = size-i-1;
		data[first] ^= data[second];
		data[second] ^= data[first];
		data[first] ^= data[second];		
		// data[first] = second;
		// data[second] = first_data;
	}
#endif
}

void convert::nbo::from(uint8_t *data, uint64_t size){
	to(data, size);
}

std::vector<uint8_t> convert::nbo::to(std::vector<uint8_t> data){
	to(data.data(), data.size());
	return data;
}

std::vector<uint8_t> convert::nbo::from(std::vector<uint8_t> data){
	to(data.data(), data.size());
	return data;
}

std::vector<uint8_t> convert::nbo::to(std::string data){
	return to(std::vector<uint8_t>(
			  data.begin(),
			  data.end()));
}

std::vector<uint8_t> convert::nbo::from(std::string data){
	return to(std::vector<uint8_t>(
			  data.begin(),
			  data.end()));
}

// might make more sense to have these functions be the native

std::array<uint8_t, 32> convert::array::type::to(std::string data){
	std::array<uint8_t, 32> retval = {{0}};
	for(uint64_t i = 0;i < data.size();i++){
		retval[i] = data[i];
	}
	return retval;
}

std::string convert::array::type::from(std::array<uint8_t, 32> data){
	std::string retval;
	for(uint64_t i = 0;i < data.size();i++){
		if(data[i] == 0){
			break;
		}
		retval += (char)(data[i]);
	}
	return retval;
}

std::string convert::number::to_binary(uint64_t data){
	std::string retval;
	for(uint64_t i = 0;i < 64;i++){
		if((data & 1) != 0){
			retval = "1" + retval;
		}else{
			retval = "0" + retval;
		}
		if((i+1)%8 == 0){
			retval = " " + retval;;
		}
		data >>= 1;
	}
	return retval;
}

// of course, in RGB format (fourth being the BPC, bytes per color)

// mask here is just the first color, it shifts automatically down
// the line for each color (too little control?)
uint64_t convert::color::to(std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color){
	uint64_t retval = 0;
	const uint8_t bpc = std::get<3>(color);
	retval |= std::get<0>(color);
	retval |= std::get<1>(color) << bpc;
	retval |= std::get<2>(color) << (bpc*2);
	return retval;
}

std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> convert::color::from(uint64_t color, uint8_t bpc){
	std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> retval;
	const uint64_t bpc_mask = (1 >> bpc)-1;
	std::get<0>(retval) = (color >> (bpc*0)) & bpc_mask;
	std::get<1>(retval) = (color >> (bpc*1)) & bpc_mask;
	std::get<2>(retval) = (color >> (bpc*2)) & bpc_mask;
	return retval;
}

std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> convert::color::bpc(std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color,
								      uint8_t new_bpc){
	const uint8_t old_bpc = std::get<3>(color);
	if(old_bpc == new_bpc){
		return color;
	}
	double mul;
	if(new_bpc > old_bpc && unlikely(new_bpc-old_bpc <= 31)){
		mul = (uint32_t)((uint32_t)1 << (new_bpc-old_bpc));
	}else{
		mul = pow(2, new_bpc-old_bpc);
	}
	std::get<0>(color) *= mul;
	std::get<1>(color) *= mul;
	std::get<2>(color) *= mul;
	return color;
}

static std::string byte_to_hex(uint8_t byte){
	return (std::string)(to_hex(byte&4)) +
		(std::string)(to_hex(byte >> 4));
}

static uint8_t to_byte_char(int8_t hex){
	switch(hex){
	case '0':
		return 0;
	case '1':
		return 1;
	case '2':
		return 2;
	case '3':
		return 3;
	case '4':
		return 4;
	case '5':
		return 5;
	case '6':
		return 6;
	case '7':
		return 7;
	case '8':
		return 8;
	case '9':
		return 9;
	case 'a':
		return 10;
	case 'b':
		return 11;
	case 'c':
		return 12;
	case 'd':
		return 13;
	case 'e':
		return 14;
	case 'f':
		return 15;
	}
	print("invalid hex character", P_ERR);
	return 0;
}

// TODO: make cross endian

static uint8_t hex_to_byte(std::string hex){
	return to_byte_char(hex[0]) | (to_byte_char(hex[1]));
}

/*
  All hexadecimal is in big endian
*/

std::vector<uint8_t> convert::number::from_hex(std::string hex){
	std::vector<uint8_t> retval;
	for(uint64_t i = 0;i < (hex.size()/2);i++){
		retval.push_back(
			hex_to_byte(
				hex.substr(
					i*2, 2)));
	}
	retval = convert::nbo::to(retval);
	return retval;
}

std::string convert::number::to_hex(std::vector<uint8_t> byte){
	std::string retval;
	byte = convert::nbo::to(byte);
	for(uint64_t i = 0;i < byte.size();i++){
		retval +=
			byte_to_hex(byte[i]);
	}
	return retval;
}
	
std::string convert::array::id::to_hex(id_t_ id_){
	std::array<uint8_t, 32> hash_array =
		get_id_hash(id_);
	uint64_t uuid_num =
		get_id_uuid(id_);
	std::vector<uint8_t> uuid_vector(
		&uuid_num,
		&uuid_num+8);
	std::string retval =
		convert::number::to_hex(
			uuid_vector) +
		"-" +
		convert::number::to_hex(
			std::vector<uint8_t>(
				hash_array.begin(),
				hash_array.end()));
	return retval;
}

id_t_ convert::array::id::from_hex(std::string id_){
	id_t_ retval = ID_BLANK_ID;
	uint64_t pos_of_hyphen = id_.find_first_of("-");
	std::vector<uint8_t> uuid_raw =
		convert::number::from_hex(
			id_.substr(
				0, pos_of_hyphen));
	if(uuid_raw.size() != 8){
		print("invalid size for UUID", P_ERR);
	}
	uint64_t uuid = 0;
	memcpy(&uuid, uuid_raw.data(), 8);
	set_id_uuid(
		&retval,
		uuid);
	std::vector<uint8_t> hash =
		convert::number::from_hex(
			id_.substr(
				pos_of_hyphen, id_.size()));
	if(hash.size() != 32){
		print("invalid size for hash", P_ERR);
	}
	std::array<uint8_t, 32> hash_array;
	memcpy(&(hash_array[0]), &(hash[0]), 32);
	set_id_hash(
		&retval,
		hash_array);
	return retval;
}
