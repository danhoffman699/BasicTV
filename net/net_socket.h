#include "../id/id.h"
#include "net_const.h"
#ifndef NET_SOCKET_H
#define NET_SOCKET_H
#include "SDL2/SDL_net.h"

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

#define NET_SOCKET_USE_SOCKS (1 << 0)

struct net_socket_t{
private:
	uint8_t status = 0;
	std::vector<uint8_t> local_buffer;
	// IP and ports of clients
	std::pair<std::string, uint16_t> client_conn;
	std::pair<std::string, uint16_t> socks_conn;
	// raw socket for SDL
	TCPsocket socket = nullptr;
	// SOCKS user ID
	std::array<uint8_t, 5> socks_user_id_str = {{0}};
	void socket_check();
public:
	data_id_t id;
	net_socket_t();
	~net_socket_t();
	// general socket stuff, proxy-agnostic
	bool is_alive();
	uint8_t get_status();
	void connect(std::pair<std::string, uint16_t> conn_info);
	void disconnect();
	void send(std::vector<uint8_t> data);
	std::vector<uint8_t> recv(uint64_t byte_count = 0, uint64_t flags = 0);
	std::vector<uint8_t> recv_all_buffer();
	// enable/disable SOCKS proxy, set as a flag in status
	// throws on error
	void enable_socks(std::pair<std::string, uint16_t> socks_info, std::pair<std::string, uint16_t> conn_info);
	void disable_socks();
	void set_tcp_socket(TCPsocket);
	TCPsocket get_tcp_socket();
	std::pair<std::string, uint16_t> get_client_conn();
	std::pair<std::string, uint16_t> get_socks_conn();
	bool activity();
	// fetch buffer sizes
	uint64_t get_buffer_size();
};
#endif
