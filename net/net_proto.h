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

  Device Control 1 is used to specify the start of a data structure. The
  following 8-bytes are defined as such:
  Bytes 1-4: size of the string
  Byte 5: Major version of the sending party
  Byte 6: Minor version of the sending party
  Byte 7: Patch version of the sending party
  Byte 8: Compression macro
  Byte 9: Other macros
  Byte 10-13: Unused (currently)

  The version broadcasting, although a part of the standard, is intentionally
  not required to be functional. Setting all versions to zero tells people
  that the information was stripped (privacy reasons). Version information
  might also be added in the net_peer_t data, but won't be required there
  as well.

  All of the structures should be backwards and forwards compatiable from
  the initial release (hasn't happened yet). Only new information can be
  appended to the structure in a new release, and this information cannot
  be vital to the usefulness of the data (making it only a networkable cache
  at best: nice, but impractical).

  Prepend DEV_CTRL to DEV_CTRL in the data (like backslashes), length of the 
  package header is static (8-bytes). This INCLUDES the packet header data
  itself (apply double to everything, and only remove the header identifier).

  If information is received out of sync, then wait until one of these
  segments have been received, then process that information. When it becomes
  stable enough, then assume they are malicious and (possibly) delist them from
  the peer list.

  TODO: add a UNIX timestamp to the encrypted protocol so I can get a rough
  estimate of networking delays (Y2k38 proof it)
 */

#define NET_PROTO_DEV_CTRL_1 0x11

#define NET_PROTO_META_LENGTH 13

// max length of a IPv4 mapped IPv6 address plus one for a NULL

struct net_peer_t{
private:
	std::array<uint8_t, NET_IP_MAX_SIZE> ip = {{0}};
	uint16_t port = 0;
	std::array<uint64_t, NET_PROTO_MAX_SOCKET_PER_PEER> socket = {{0}};
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
	void add_socket_id(uint64_t socket_);
	uint64_t get_socket_id(uint32_t entry_);
	void del_socket_id(uint64_t socket_);
};

/*namespace net_proto{
	void init();
	void loop();
	void close();
	};*/

extern void net_proto_init();
extern void net_proto_loop();
extern void net_proto_close();

#endif
