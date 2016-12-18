#ifndef NET_PEER_T
#define NET_PEER_T
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

#endif
