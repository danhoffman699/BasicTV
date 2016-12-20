#include "../../id/id.h"
#ifndef NET_PROTO_META_H
#define NET_PROTO_META_
extern std::vector<uint8_t> net_proto_write_packet_metadata(
	net_proto_standard_size_t data_size,
	net_proto_standard_ver_t ver_major,
	net_proto_standard_ver_t ver_minor,
	net_proto_standard_ver_t ver_patch,
	net_proto_standard_macros_t compression_macros,
	net_proto_standard_macros_t other_macros,
	net_proto_standard_unused_t unused);

extern std::vector<uint8_t> net_proto_read_packet_metadata(
	uint8_t *data,
	uint32_t data_length,
	net_proto_standard_size_t *data_size,
	net_proto_standard_ver_t *ver_major,
	net_proto_standard_ver_t *ver_minor,
	net_proto_standard_ver_t *ver_patch,
	net_proto_standard_macros_t *compression_macros,
	net_proto_standard_macros_t *other_macros,
	net_proto_standard_unused_t *unused);

#endif
