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
#ifdef __ORDER_LITTLE_ENDIAN__
	switch(size){
	case 1:
		NBO_8(*data);
		return;
	case 2:
		NBO_16(*data);
		return;
	case 4:
		NBO_32(*data);
		return;
	case 8:
		NBO_64(*data);
		return;
	}
	const bool odd = !!(size & 1);
	for(uint64_t i = 0;i < size/2;i++){
		const uint64_t first = i;
		const uint64_t second = size-i;
		const uint8_t first_data = data[first];
		data[first] = NBO_8(second);
		data[second] = NBO_8(first_data);
	}
	if(unlikely(odd)){
		data[(size/2)+1] = NBO_8(data[(size/2)+1]);
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

std::string convert::number::to_hex(uint64_t data){
	std::string retval;
	for(uint64_t i = 0;i < 64;i+=4){
		const uint64_t character =
			(data >> i) & 0xF;
		if(character == 0){
			retval = '0' + retval;
		}else if(character <= 9){
			retval = (char)(character-1+'1') + retval;
		}else{
			retval = (char)(character-10+'A') + retval;
		}
	}
	return "0x" + retval;
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
