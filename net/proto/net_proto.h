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

// check to see if the data exists, don't actually send it over
#define NET_REQUEST_ONLY_STATUS (1 << 1)

// we know it is local if there is no net_proto_socket_t ID bound to it

/*
  TODO: implement requesting all of a type
 */

/*
  net_request_t: requesting an ID from another peer

  This is one of a few types that is allowed to be sent first, currently the
  only one (another type is a symmetric key system, but that isn't implemented
  yet and isn't really needed right now, let's make that a version 2 network
  thing, possibly).

  If the socket_id is zero, then assume this request is local going out.

  There is no hard-defined upper limit on the list of ids, since a net_request_t
  is P2P, not a network wide datatype.

  If type is blank, and the request type is a blacklist, then don't reply. There
  is no reason to send out literally every piece of information we have.

  If type is blank, and the request type is a whitelist, then fill those as best
  as we can.

  if type is not blank, then fill them properly (whitelist or blacklist works),
  and send them back through the socket_id (non-zero).

  There are soft limits on what types of information can be sent over as all of
  a type:
  net_peer_t, encrypt_pub_key_t, and net_con_req_t are almost always allowed,
  since they are small and few in number (relative to other types)

  tv_channel_t is almost always allowed as well, as it is the seed for the
  linked lists

  tv_frame_*_t types are almost never allowed by a type, since they are so vast
  in number, and the statistical odds of returning 10% useful information is
  vanishingly small

  This is excluding instances where it can't be relayed at all for security
  reasons (encrypt_priv_key_t, namely), or is completely used locally and is
  either generated on the fly or has no use elsewhere (tv_menu_t, tv_window_t,
  net_socket_t/net_proto_socket_t, among others). It can be said that
  ID_DATA_NOEXPORT and ID_DATA_NONET override net_request_t entirely.

 */

struct net_proto_request_t{
private:
	std::array<uint8_t, TYPE_LENGTH> type = {{0}};
	std::vector<id_t_> ids = {{0}};
	uint8_t flags = 0;
	uint64_t socket_id = 0;
	uint64_t last_query_timestamp_micro_s = 0;
public:
	data_id_t id;
	net_proto_request_t();
	~net_proto_request_t();
	void set_proto_socket_id(uint64_t socket_id_);
	uint64_t get_proto_socket_id();
	void set_flags(uint8_t flags);
	uint8_t get_flags();
	std::vector<id_t_> get_ids();
	void set_ids(std::vector<id_t_> ids_);
	void set_type(std::array<uint8_t, TYPE_LENGTH> type_);
	std::array<uint8_t, TYPE_LENGTH> get_type();
	bool is_local();
	uint64_t get_last_query_timestamp_micro_s();
};

// should change this naming convention
typedef net_proto_request_t net_request_t;

#endif
#include "net_proto_peer.h"
