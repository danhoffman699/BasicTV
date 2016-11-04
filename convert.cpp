#include "convert.h"
#include "util.h"
#include "id/id.h"

std::vector<uint8_t> convert::nbo::to(std::vector<uint8_t> data){
#ifndef __ORDER_BIG_ENDIAN__
	const bool odd = !!(data.size() & 1);
	for(uint64_t i = 0;i < data.size()/2;i++){
		const uint64_t first = i;
		const uint64_t second = data.size()-i;
		const uint8_t first_data = data[first];
		data[first] = NBO_8(second);
		data[second] = NBO_8(first_data);
	}
	if(odd){
		data[(data.size()/2)+1] = NBO_8(data[(data.size()/2)+1]);
	}
#endif
	return data;
}

std::vector<uint8_t> convert::nbo::to(std::string data){
	std::vector<uint8_t> retval;
	for(uint64_t i = 0;i < data.size();i++){
		retval.push_back(data[i]);
	}
#ifndef __ORDER_BIG_ENDIAN__
	retval = convert::nbo::to(retval);
#endif
	return retval;
}

std::vector<uint8_t> convert::nbo::from(std::vector<uint8_t> data){
#ifndef __ORDER_BIG_ENDIAN__
	const bool odd = !!(data.size() & 1);
	for(uint64_t i = 0;i < data.size()/2;i++){
		const uint64_t first = i;
		const uint64_t second = data.size()-i;
		const uint8_t first_data = data[first];
		data[first] = NBO_TO_NATIVE_8(second);
		data[second] = NBO_TO_NATIVE_8(first_data);
	}
	if(odd){
		data[(data.size()/2)+1] = NBO_TO_NATIVE_8(data[(data.size()/2)+1]);
	}
#endif
	return data;
}

std::vector<uint8_t> convert::nbo::from(std::string data){
	std::vector<uint8_t> retval;
	for(uint64_t i = 0;i < data.size();i++){
		retval.push_back(data[i]);
	}
#ifndef __ORDER_BIG_ENDIAN__
	retval = convert::nbo::from(retval);
#endif
	return retval;
}

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
			retval += "1";
		}else{
			retval += "0";
		}
		data >>= 1;
	}
	return retval;
}

std::string convert::number::to_hex(uint64_t data){
	throw std::runtime_error("implement this later");
}

// of course, in RGB format (fourth being the BPC, bytes per color)

// mask here is just the first color, it shifts automatically down
// the line for each color (too little control?)
uint64_t convert::color::to(std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color){
	uint64_t retval = 0;
	const uint8_t bpc = std::get<3>(color);
	retval |= std::get<0>(color);
	retval |= std::get<1>(color) S_L bpc;
	retval |= std::get<2>(color) S_L (bpc*2);
	return retval;
}

std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> convert::color::from(uint64_t color, uint8_t bpc){
	std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> retval;
	const uint64_t bpc_mask = (1 S_L bpc)-1;
	std::get<0>(retval) = (color S_S (bpc*0)) & bpc_mask;
	std::get<1>(retval) = (color S_S (bpc*1)) & bpc_mask;
	std::get<2>(retval) = (color S_S (bpc*2)) & bpc_mask;
	return retval;
}

std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> convert::color::bpc(std::tuple<uint64_t, uint64_t, uint64_t, uint8_t> color,
								      uint8_t new_bpc){
	const uint8_t old_bpc = std::get<3>(color);
	std::get<0>(color) *= pow(2, new_bpc)/pow(2, old_bpc);
	std::get<1>(color) *= pow(2, new_bpc)/pow(2, old_bpc);
	std::get<2>(color) *= pow(2, new_bpc)/pow(2, old_bpc);
	return color;
}
