/*
  net_proto.h: The entire networking protocol
 */
#include "net_const.h"
#ifndef NET_PROTO_H
#define NET_PROTO_H
#include "../id/id.h"
#include "string"
#include "SDL2/SDL_net.h"

// only useful for multithreading Tor connections
#define NET_PROTO_MAX_SOCKET_PER_PEER 16

// single identifies the beginning of the metadata, double is single, etc.
#define NET_PROTO_DEV_CTRL_1 0x11

// adjust this value
#define NET_PROTO_META_LENGTH 12

// max length of a IPv4 mapped IPv6 address plus one for a NULL

extern void net_proto_init();
extern void net_proto_loop();
extern void net_proto_close();


typedef uint32_t net_proto_standard_size_t;
typedef uint8_t net_proto_standard_ver_t;
typedef uint8_t net_proto_standard_macros_t;
//once net_proto_standard_unused_t actually suits a purpose, then remove
// this as a type and replace it with whatever
typedef uint32_t net_proto_standard_unused_t;

#endif
#include "net_peer.h"
