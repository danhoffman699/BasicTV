#include "net_proto.h"
#include "net_proto_meta.h"

#define WRITE_DATA_META(data) retval.insert(retval.end(), &data, &data+sizeof(data))

std::vector<uint8_t> net_proto_write_packet_metadata(
	net_proto_standard_size_t data_size,
	net_proto_standard_ver_t ver_major,
	net_proto_standard_ver_t ver_minor,
	net_proto_standard_ver_t ver_patch,
	net_proto_standard_macros_t other_macros,
	net_proto_standard_unused_t unused){
	std::vector<uint8_t> retval = {NET_PROTO_DEV_CTRL_1};
	// payload of the entire structure (not counting any metadata)
	WRITE_DATA_META(data_size);
	// version data is optional, omitted with 0.0.0 (or anything)
	WRITE_DATA_META(ver_major);
	WRITE_DATA_META(ver_minor);
	WRITE_DATA_META(ver_patch);
	// no current use for the following
	WRITE_DATA_META(other_macros);
	WRITE_DATA_META(unused);
	return retval;
}


#define READ_DATA_META(ptr)						\
	if(ptr != nullptr){						\
		if(offset + sizeof(*ptr) > data_length){		\
			print("packet metadata too large", P_ERR);	\
		}else{							\
			memcpy(ptr, &data[0], sizeof(*ptr));		\
		}							\
	}								\
	offset += sizeof(*ptr);						\

void net_proto_read_packet_metadata(
	uint8_t *data,
	uint32_t data_length, // input data
	net_proto_standard_size_t *data_size, // payload data
	net_proto_standard_ver_t *ver_major,
	net_proto_standard_ver_t *ver_minor,
	net_proto_standard_ver_t *ver_patch,
	net_proto_standard_macros_t *other_macros,
	net_proto_standard_unused_t *unused){
	uint64_t offset = 1; // NET_PROTO_DEV_CTRL_1
	READ_DATA_META(data_size);
	READ_DATA_META(ver_major);
	READ_DATA_META(ver_minor);
	READ_DATA_META(ver_patch);
	READ_DATA_META(other_macros);
	READ_DATA_META(unused);
}
