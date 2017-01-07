/*
  net_proto.h: The networking protocol
*/
#include "../net_const.h"
#ifndef NET_PROTO_H
#define NET_PROTO_H
#include "../../id/id.h"
#include "string"
#include "SDL2/SDL_net.h"

/*
  Multiple connections can be made to a single client perfectly fine. However,
  the node you are connecting to will register all of the sockets as individual
  peers. 

  Multiple connections doesn't make much sense unless you want to increase the
  throughput of the Tor network by opening more circuits. SOCKS proxies are 
  somewhat implemented, but the easy creating and destroying of Tor circuits 
  is not (shouldn't be hard).
 */

#define NET_PROTO_MAX_SOCKET_PER_PEER 16

// single identifies the beginning of the metadata, double is single, etc.
#define NET_PROTO_DEV_CTRL_1 0x11

// adjust this value
#define NET_PROTO_META_LENGTH 12

extern void net_proto_init();
extern void net_proto_loop();
extern void net_proto_close();

/*
  Since this is a vector, this should be a soft limit. I should increase this a bit.
 */

#define NET_REQUEST_MAX_LENGTH 65536

// this is the only use I can think of for the flags in net_request_

#define NET_REQUEST_BLACKLIST (1 << 0)
#define NET_REQUEST_WHITELIST (0 << 0)

/*
  TODO: implement requesting all of a type
 */

struct net_request_t{
private:
	// list of ids
	std::vector<id_t_> ids = {{0}};
	// only currently used for the blacklist/whitelist settings
	uint8_t flags = 0;
	// socket that received request, obviously a cached value
	uint64_t socket_id = 0;
	// is this outbound or inbound?
	bool local = false;
public:
	data_id_t id;
	net_request_t();
	~net_request_t();
	void set_socket_id(uint64_t socket_id_);
	uint64_t get_socket_id();
	void set_flags(uint8_t flags);
	uint8_t get_flags();
	void add_id(uint64_t id_);
	void del_id(uint64_t id_);
	uint64_t get_id(uint64_t entry);
	bool is_local();
};

#endif
#include "net_proto_peer.h"
