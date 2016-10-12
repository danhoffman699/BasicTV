#ifndef NET_PROTO_H
#define NET_PROTO_H
#include "string"
#include "SDL2/SDL_net.h"
#define NET_PROTO_PACKET_IN (1 << 0)
#define NET_PROTO_PACKET_OUT (0 << 0)

#define NET_PROTO_FLAG_ENCRYPTION (1 << 8)
#define NET_PROTO_FLAG_COMPRESSED (1 << 9)

struct net_client_t{
private:
	TCPsocket socket;
	IPaddress ip_addr;
	std::vector<uint8_t> pgp_pub_key;
public:
	net_client_t(TCPsocket, std::string, uint16_t);
	~net_client_t();
};
namespace net_proto{
	void init();
	void loop();
	void close();
};
#endif
