#include "../../id/id.h"
#include "../net_const.h"
#include "../net_socket.h"
#ifndef NET_PROTO_SOCKET_H
#define NET_PROTO_SOCKET_H
#include "SDL2/SDL_net.h"

/*
  net_proto_socket_t: handles protocol specific transcoding

  This should be the only socket type used for protocol communication
  (net_proto_socket_t is just a friendly wrapper to net_socket_t)

  net_proto_socket_t doesn't dive into nitty-gritty technical stuff (namely
  SOCKS proxies and TCP hole punching).
  
  SOCKS and other types of proxies will have a dedicated file for protocol
  level jargon. The same applies to UDP and TCP holepunching. Holepunching,
  in just the technical, bare-metal, definition, isn't implemented yet
  (net_con_req_t is not completed yet, but enough standards are in place to 
  make reliant functions and systems work OK).
 */

struct net_proto_socket_t{
private:
	id_t_ socket_id = 0;
	/*
	  I don't care about UDP for now, but had I bothered, I would have
	  included chronological IDs for sequential packets, and nested
	  that inside of a std::pair with an ID for the list of packets
	  in general (which would be in an std::vector of different packet
	  IDs).

	  It would have been this:
	  std::vector<std::pair<std::vector<std::pair<std::vector<uint8_t>, uint64_t> >, uint64_t> >

	  I like TCP (working_buffer is all data received in order)
	 */
	std::vector<uint8_t> working_buffer;
	// finalized buffer, removed DEV_CTRL_1, native endian, etc.
	std::vector<std::vector<uint8_t> > buffer;
	// timestamp of last update
	uint64_t last_update_micro_s = 0;
public:
	data_id_t id;
	void set_socket_id(id_t_ socket_id_);
	id_t_ get_socket_id();
	void update();
	// probbaly don't need more granularity in the buffers
	std::vector<std::vector<uint8_t> > get_buffer();
};

#endif
