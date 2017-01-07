#include "../../main.h" // VERSION_*
#include "net_proto.h"
#include "net_proto_meta.h"

#define WRITE_DATA_META(data) retval.insert(retval.end(), &data, &data+sizeof(data))

std::vector<uint8_t> net_proto_write_packet_metadata(
        net_proto_standard_data_t data){
	std::vector<uint8_t> retval = {NET_PROTO_DEV_CTRL_1};
	WRITE_DATA_META(data.size);
	WRITE_DATA_META(data.ver_major);
	WRITE_DATA_META(data.ver_minor);
	WRITE_DATA_META(data.ver_patch);
	WRITE_DATA_META(data.macros);
	WRITE_DATA_META(data.unused);
	return retval;
}


#define READ_DATA_META(ptr)						\
	if(ptr != nullptr){						\
		if(offset + sizeof(*ptr) > data_length){		\
			print("metadata is too short", P_ERR);	\
		}else{							\
			memcpy(ptr, &data[0], sizeof(*ptr));		\
		}							\
	}								\
	offset += sizeof(*ptr);						\

void net_proto_read_packet_metadata(
	uint8_t *data,
	uint32_t data_length,
	net_proto_standard_data_t *standard_data){
	uint64_t offset = 1; // NET_PROTO_DEV_CTRL_1
	READ_DATA_META(&(standard_data->size));
	READ_DATA_META(&(standard_data->ver_major));
	READ_DATA_META(&(standard_data->ver_minor));
	READ_DATA_META(&(standard_data->ver_patch));
	READ_DATA_META(&(standard_data->macros));
	READ_DATA_META(&(standard_data->unused));
	if(standard_data->ver_major != VERSION_MAJOR ||
	   standard_data->ver_minor != VERSION_MINOR ||
	   standard_data->ver_patch != VERSION_REVISION){
		// worst case scenario is the unused data isn't used
		// sane defaults should be chosen for a zero
		print("decoding metadata from a different version", P_WARN);
		P_V(standard_data->ver_major, P_WARN);
		P_V(standard_data->ver_minor, P_WARN);
		P_V(standard_data->ver_patch, P_WARN);
		P_V(VERSION_MAJOR, P_WARN);
		P_V(VERSION_MINOR, P_WARN);
		P_V(VERSION_REVISION, P_WARN);
	}
	if(standard_data->unused != 0){
		print("unused space is being used, check for a new version", P_ERR);
	}
}
