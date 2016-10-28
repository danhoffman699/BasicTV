#include "convert.h"
#include "util.h"
#include "id/id.h"

std::vector<uint8_t> convert::nbo::to(std::vector<uint8_t> data){
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
	return data;
}

std::vector<uint8_t> convert::nbo::to(std::string data){
	std::vector<uint8_t> retval;
	for(uint64_t i = 0;i < data.size();i++){
		retval.push_back(data[i]);
	}
	return convert::nbo::to(retval);
}

