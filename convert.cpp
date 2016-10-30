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
		retval += (char)(data[i]);
	}
	return retval;
}
