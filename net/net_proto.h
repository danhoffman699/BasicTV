/*
  net_proto.h: The entire networking protocol
 */
#include "net_const.h"
#ifndef NET_PROTO_H
#define NET_PROTO_H
#include "../id/id.h"
#include "string"
#include "SDL2/SDL_net.h"

/*
  Only useful for multithreading Tor connections
 */
#define NET_PROTO_MAX_SOCKET_PER_PEER 16

/*
  net_peer_t: another peer in the BasicTV network. There is a limit
  of 65536 connected peers at any one time. If there are available slots
  and the original streamer is broadcasting their IP address, then create
  a connection directly to them. If they are not broadcasting that, then
  keep streaming it like it is

  All of the structures should be backwards and forwards compatiable from
  the initial release (hasn't happened yet). Only new information can be
  appended to the structure in a new release, and this information cannot
  be vital to the usefulness of the data (making it only a networkable cache
  at best: nice, but impractical).

  TODO: add a UNIX timestamp to the encrypted protocol so I can get a rough
  estimate of networking delays (Y2k38 proof it)
 */

//single identifies the beginning of the metadata, double is single, etc.
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
