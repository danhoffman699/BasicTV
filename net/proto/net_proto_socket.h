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
  
  net_proto_socket_t handles holepunching. SOCKS and proxies, since they
  aren't dependent upon the protocol, are handled one level down at
  net_socket_t. Proxy identities are stored in net_proxy_t
 */

struct net_proto_socket_t{
private:
	id_t_ socket_id = 0;
	std::vector<uint8_t> working_buffer;
	// finalized buffer, removed DEV_CTRL_1, native endian, etc.
	std::vector<std::vector<uint8_t> > buffer;
	// timestamp of last update
	uint64_t last_update_micro_s = 0;
	std::vector<std::pair<uint64_t, id_t_> > id_log;
public:
	data_id_t id;
	void set_socket_id(id_t_ socket_id_);
	id_t_ get_socket_id();
	void send_id(id_t_ id_);
	void update();
	// probably don't need more granularity in the buffers
	std::vector<std::vector<uint8_t> > get_buffer();
};

#endif
