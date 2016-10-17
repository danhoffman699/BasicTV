#ifndef NET_SOCKET_H
#define NET_SOCKET_H
/*
  net_socket_t: Manages network sockets. By and large, this operates as a 
  frontend to SDL_net. However, if Tor is enabled, this manages the Tor
  connections and systematically maps Tor connections to network peers.
  A good distribution is defined as having a similar network quality map
  to one another (on the clear-net), because high quality clients will always
  be prefered over slower ones, and having multiple Tor sockets with high
  quality network peers will mean that more information is securely transmitted
  over the Tor network. This is one of the many reasons why I wrote the streams
  on a data block system versus a conventional stream system.
 */

#include "SDL2/SDL_net.h"

struct net_tor_instance_t{
private:
	IPaddress tor_ip = 0;
	IPaddress global_ip = 0;
public:
	data_id_t id;
	net_tor_instance_t(IPaddress tor_ip);
	~net_tor_instance_t();
};

struct net_socket_t{
private:
	uint64_t tor_instance_id = 0;
	TCPsocket socket;
public:
	data_id_t id;
	net_socket_t();
	~net_socket_t();
	/*
	  Check if the TCPsocket is alive. Made to be used with the Tor sockets.

	  TODO: How can I create a new Tor identity from another program?
	  
	  TODO: Study the Tor spec and possibly don't keep sockets open all
	  of the time? Both sides should keep their net_peer_t and should
	  open sockets BY THE REQUESTER when data is needed. Close it when
	  it isn't used BY THE REQUESTER and possibly set a flag inside
	  of this data type saying that the socket isn't actually connected?
	 */
	bool is_alive();
	void set_tor_instance_id(uint64_t tor_instance_id_);
	void send(std::vector<uint8_t> data);
	std::vector<uint8_t> recv(uint64_t byte_count);
	// there should be no reason to get the socket
};

#endif
