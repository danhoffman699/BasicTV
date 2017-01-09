#ifndef NET_PROTO_PEER_H
#define NET_PROTO_PEER_H
/*
  net_peer_t: information about a network peer

  A network peer, in this context, is any unique IP/port/protocol combination.
  The following instances would constitute multiple net_peer_t data types:
  1. Multithreaded Tor circuits connecting to one computer
  2. One client with multiple protocols (TCP and UDP connections)

  RSA keypairs that sign the net_peer_t data can be changed for anonymity 
  conerns, but one needs to be in place. 

  Optimizations can be made to prevent redundant queries if the same RSA public
  key ID has been used. There is nothing stopping one user from creating two
  seperate nodes with the same RSA public key and hosting seperate content, thus
  making the seperate sockets point to different bits of data.

  One "sane" reason for having an RSA key distributed is to more reliably
  forward data onto the network. If one node goes down, their IP/port
  combination is no longer reachable. If the second node recognizes this, they
  can continue where the other node left off and publish the data. However,
  in such a case, it would make more sense to "anonymize" the RSA keypair so
  there is no confusion between the two nodes anyhow (depending on how this
  optimization is implemented). HOWEVER, this could fork the entire linked
  list over into two different systems, so this isn't defined behavoir (yet).

  Multiple sockets can be opened for one peer at any given time. 

  The net_peer_t also contains information about the connection configuration.
  The primary use is publicizing open TCP ports (manually or UPnP), open UDP 
  ports, and other useful information about initating a connection (through
  the net_con_req_t type).
*/

/*
  All communication between nodes on this network is encrypted with the public
  key of the net_peer_t data type in a TLS style manner. If there is no reply to
  requests, then delete the local version (either offline or invalid).

  This is done for three reasons:
  1. Making net_peer_t requests less taxing on the network by not forwarding
  useless information (one of the most common bulk requests)
  2. Making data requests less taxing on the network by not making useless
  requests for data
  3. Mitigating a DoS attack by spamming invalid net_peer_ts on the network
  3. Making a more accurate estimate of the activity on the network
*/

#define NET_IP_RAW_SIZE 16

#define NET_PEER_TCP 0
#define NET_PEER_UDP 1

// assume this is using IPv4
#define NET_PEER_IPV6 (1 >> 2)

/*
  Manually opened port. A node with no other connections has to first connect
  to a node with an open TCP/UDP port, so having forwarded ports are vital
  to the health of the network.

  Nodes that have too few net_peer_t entries with known open ports might resort
  to trying to directly connect to all of them using both TCP and UDP, although
  this is both bad behavior and not planned (also should never happen).
*/
#define NET_PEER_PORT_OPEN (1 >> 3)

/*
  Punchable with the supplied protocol
*/
#define NET_PEER_PUNCHABLE (1 >> 4)

#define NET_PEER_NAT_ADDRESS_PORT_RESTRICTED (1 >> 6)

struct net_peer_t{
private:
	// IPv6-able, but not used because of SDL2 not supporting it
	std::array<uint8_t, NET_IP_RAW_SIZE> ip = {{0}};
	uint16_t port = 0;
	uint8_t flags = 0;
	std::array<uint64_t, NET_PROTO_MAX_SOCKET_PER_PEER> socket = {{0}};
	std::array<uint8_t, BITCOIN_WALLET_LENGTH> bitcoin_wallet = {{0}};
public:
	data_id_t id;
	/*
	  IPaddress is passed because it is easy to do so, it isn't actually
	  stored as one. It is stored in two variables and converted OTF to
	  ip_addr to easily network peer information across the internet
	 */
	net_peer_t();
	~net_peer_t();
	void set_ip_addr(uint32_t ipv4, uint16_t port);
	void set_ip_addr(std::array<uint8_t, NET_IP_RAW_SIZE> ipv4, uint16_t port);
	void add_socket_id(uint64_t socket_);
	uint64_t get_socket_id(uint32_t entry_);
	void del_socket_id(uint64_t socket_);
};

typedef net_peer_t net_proto_peer_t;
#endif
