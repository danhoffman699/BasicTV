#ifndef NET_PROTO_H
#define NET_PROTO_H
#include "id.h"
#include "string"
#include "SDL2/SDL_net.h"

/*
  net_peer_t: another peer in the BasicTV network. There is a limit
  of 65536 connected peers at any one time. If there are available slots
  and the original streamer is broadcasting their IP address, then create
  a connection directly to them. If they are not broadcasting that, then
  keep streaming it like it is

  TODO: add a UNIX timestamp to the encrypted protocol so I can get a rough
  estimate of networking delays (Y2k38 proof it)

  The PGP key for encrypting their packets is also in net_peer_t, and is
  generated from the PGP 
 */

#define NET_PEER_LIMIT 65536

// max length of a IPv4 mapped IPv6 address
#define NET_IP_MAX_SIZE 45

struct net_peer_t{
private:
	TCPsocket socket = nullptr;
	IPaddress ip_addr;
	std::array<uint8_t, NET_IP_MAX_SIZE> ip = {0};
	uint16_t port;
	std::array<uint8_t, PGP_PUBKEY_SIZE> pubkey = {0};
public:
	/*
	  Should never be networked, so having a special constructor is OK until
	  I start exporting data somehow to improve networking
	 */
	data_id_t id;
	net_peer_t(TCPsocket, std::string, uint16_t);
	~net_peer_t();
};

namespace net_proto{
	void init();
	void loop();
	void close();
};
#endif
