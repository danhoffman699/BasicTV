#include "main.h"
#include "util.h"
#include "lock.h"
#include "net_proto.h"
#include "settings.h"

/*
  TODO: actually make a version of this that is compliant to the spec. There
  should be no main server, but a list of peers that forward PGP data.
 */

static TCPsocket incoming;

net_peer_t::net_peer_t(TCPsocket socket_, IPaddress ip_addr_) : id(this, __FUNCTION__){
	id.add_data(&(ip[0]), NET_IP_MAX_SIZE);
	/*
	  socket and ip_addr are derived at initialization
	  Some sort of export format will be created so
	  a personal dictionary of nodes can be used
	  */
	const char *ip_ = SDLNet_ResolveIP(&ip_addr_);
	if(ip_ == nullptr){
		print("ip_ is a nullptr", P_ERR);
		throw std::runtime_error("ip_");
	}
	memcpy(&(ip[0]), ip_, strlen(ip_));
	socket = socket_;
}

net_peer_t::~net_peer_t(){
	SDLNet_TCP_Close(socket);
}

static void net_proto_process_incoming_connections(){
	TCPsocket new_socket = SDLNet_TCP_Accept(incoming);
	if(new_socket == nullptr){
		PRINT((std::string)"cannot accept TCP connection"+
		      SDL_GetError(), P_ERR);
	}else{
		/*
		  TODO: sanity check IP and port to check for a DDoS of 
		  incoming TCP connections
		 */
		IPaddress *address =
			SDLNet_TCP_GetPeerAddress(new_socket);
		if(address == nullptr){
			print("new_socket is a nullptr", P_ERR);
			throw std::runtime_error("new_socket");
		}
		net_peer_t *peer_tmp =
			new net_peer_t(new_socket, *address);
	}
}

static void net_proto_loop(){
	net_proto_process_incoming_connections();
}

void net_proto_init(){
	uint16_t port = std::stoi(settings::get_setting("port"));
	IPaddress server_ip;
	if(SDLNet_ResolveHost(&server_ip, NULL, port) == -1){
		PRINT((std::string)"cannot resolve host:"+
		      SDLNet_GetError(), P_CRIT);
	}
	incoming = SDLNet_TCP_Open(&server_ip);
	if(!incoming){
		PRINT((std::string)"cannot open TCP port:"+
		      SDLNet_GetError(), P_CRIT);
	}
	return;
}

