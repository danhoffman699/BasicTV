#include "../../id/id.h"
#ifndef NET_PROTO_META_H
#define NET_PROTO_META_H
typedef uint32_t net_proto_standard_size_t;
typedef uint8_t net_proto_standard_ver_t;
typedef uint8_t net_proto_standard_macros_t;
typedef uint32_t net_proto_standard_unused_t;

#define NET_PROTO_STANDARD_DATA_LENGTH (4+1+1+1+1+4)

struct net_proto_standard_data_t{
private:
public:
	net_proto_standard_size_t size = 0;
	net_proto_standard_ver_t ver_major = 0;
	net_proto_standard_ver_t ver_minor = 0;
	net_proto_standard_ver_t ver_patch = 0;
	net_proto_standard_macros_t macros = 0;
	net_proto_standard_unused_t unused = 0;
};

extern std::vector<uint8_t> net_proto_write_packet_metadata(
        net_proto_standard_data_t data);

extern void net_proto_read_packet_metadata(
	uint8_t *data,
	uint32_t data_length,
	net_proto_standard_data_t *standard_data);
	
#endif
