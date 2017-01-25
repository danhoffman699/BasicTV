#include "../id/id.h"
#include "../stats.h"
#include "net_const.h"
#include "net_ip.h"
#include "net_proxy.h"
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

struct net_socket_t : public net_ip_t{
private:
	uint8_t status = 0;
	std::vector<uint8_t> local_buffer;
	// raw socket for SDL
	TCPsocket socket = nullptr;
	void socket_check();
	id_t_ proxy_id = ID_BLANK_ID;
	id_t_ outbound_stat_sample_set_id = ID_BLANK_ID;
	id_t_ inbound_stat_sample_set_id = ID_BLANK_ID;
public:
	data_id_t id;
	net_socket_t();
	~net_socket_t();

	// general socket stuff
	bool is_alive();
	uint8_t get_status();
	void connect();
	void disconnect();
	
	// send and recv functions
	void send(std::vector<uint8_t> data);
	std::vector<uint8_t> recv(uint64_t byte_count = 0, uint64_t flags = 0);
	std::vector<uint8_t> recv_all_buffer();
	bool activity();

	// inbound and outbound stats
	id_t_ get_inbound_stat_sample_set_id();
	id_t_ get_outbound_stat_sample_set_id();
	void set_proxy_id(id_t_ proxy_id_);
	id_t_ get_proxy_id();

	// hacky stuff that should be streamlined and abstracted
	void set_tcp_socket(TCPsocket);
	TCPsocket get_tcp_socket();
};
#endif
