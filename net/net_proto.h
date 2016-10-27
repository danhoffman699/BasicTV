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


// max length of a IPv4 mapped IPv6 address plus one for a NULL

struct net_peer_t{
private:
	std::array<uint8_t, NET_IP_MAX_SIZE> ip = {{0}};
	uint16_t port = 0;
	std::array<uint8_t, BITCOIN_WALLET_LENGTH> bitcoin_wallet = {{0}};
public:
	/*
	  Should never be networked, so having a special constructor is OK until
	  I start exporting data somehow to improve networking
	 */
	data_id_t id;
	/*
	  IPaddress is passed because it is easy to do so, it isn't actually
	  stored as one. It is stored in two variables and converted OTF to
	  ip_addr to easily network peer information across the internet
	 */
	net_peer_t();
	~net_peer_t();
	void set_ip_addr(IPaddress ip_addr_);
};

/*namespace net_proto{
	void init();
	void loop();
	void close();
	};*/

void net_proto_init();
#endif
