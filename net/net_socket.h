#include "../id/id.h"
#include "net_const.h"
#ifndef NET_SOCKET_H
#define NET_SOCKET_H
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

/*
  net_socket_t: Manages network sockets. Socket is stored inside of this file. 
  Using torsocks should work with this program, as there are no plans for UDP
  packets whatsoever, and this program isn't doing anything too technical.

  TCP sockets should be able to have multiple chunks of information sent over
  them, but vandals and malicious nodes that mis-represent the data should be
  detected and punished somehow (perhaps have a sanity checker for all values
  built into the ID API, so invalid numbers can be detected?).
  
  TODO: implement multiple Tor circuits at one time. It would be great because
  the decentralized nature works better when multiple TCP streams are being used
  at the same time (to spread information, receiving information won't really
  matter after the ID index is used efficiently).
 */

/*
  IP addresses and port numbers shouldn't exist on their own, but instead in
  std::pair entries
 */

/*
  net_socket_t has an internal cache of received data. This is currently
  only used for verifying the validity of DEV_CTRL_1s for packet
  headers (could get chopped off half-way through). However, assuming it is
  sufficiently large enough, it can be very useful for other tasks (saving data
  that seemed to have started halfway through).

  In order to access this information, you will need to pass a negative 
  value to byte_count, and the information starting from that point to the 
  end of the old_buffer will be returned. This function might try and
  receive any data from sockets and push it to the end of local_buffer, but
  that's only because I don't want packets dropping.
 */

/*
  Since all data sent over net_socket_t (not relating to the networking
  protocol) is a struct, and they are all interpreted in the same way, then
  add the manipulating and extracting inside of the net_socket_t, so it is more
  easily accessible in all parts of the program (bootstrapping, namely)
 */

#define NET_SOCKET_TCP NET_PEER_TCP
#define NET_SOCKET_UDP NET_PEER_UDP

#define NET_SOCKET_OLD_BUFFER_SIZE 1024

struct net_socket_t{
private:
	uint64_t status = 0;
	// buffer of read and old data, in order
	// only to check to see if data was ever written
	uint32_t buffer_written_memory = 0;
	std::array<uint8_t, NET_SOCKET_OLD_BUFFER_SIZE> old_buffer = {{0}};
	// buffer of unread data, should rename to new_buffer
	std::vector<uint8_t> local_buffer;
	// IP and ports of clients
	std::pair<std::string, uint16_t> client_conn;
	std::pair<std::string, uint16_t> socks_conn;
	// raw sockets and SSL data
	BIO *socket = nullptr;
	SSL_CTX *ctx = nullptr;
	SSL *ssl = nullptr;
	// SOCKS user ID
	std::array<uint8_t, 5> socks_user_id_str = {{0}};
	// chunks of data sent over the network as they were sent by the
	// sender, useful for gathering data from one socket specifically
	std::vector<std::vector<uint8_t> > chunks;

	void socket_check();
	void process_chunks();
	void send(std::vector<uint8_t> data);
	std::vector<uint8_t> recv(int64_t byte_count = 0, uint64_t flags = 0);
	void enable_socks(std::pair<std::string, uint16_t> socks_info, std::pair<std::string, uint16_t> conn_info);
	void disable_socks();
public:
	data_id_t id;
	net_socket_t();
	~net_socket_t();
	// general socket stuff, proxy-agnostic
	bool is_alive();
	uint64_t get_status();
	void connect(std::pair<std::string, uint16_t> conn_info,
		     std::pair<std::string, uint16_t> socks_info,
		     uint8_t proto);
	void disconnect();
	// enable/disable SOCKS proxy, set as a flag in status
	// throws on error
	void set_tcp_socket(TCPsocket);
	TCPsocket get_tcp_socket();
	std::pair<std::string, uint16_t> get_client_conn();
	std::pair<std::string, uint16_t> get_socks_conn();
	bool activity();
};

#endif
