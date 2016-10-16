#include "main.h"
#include "util.h"
#include "lock.h"
#include "net_proto.h"
#include "settings.h"

/*
  TODO: actually make a version of this that is compliant to the spec. There
  should be no main server, but a list of peers that forward PGP data.
 */

static TCPsocket server_socket;

// define a maximum size of the network (albeit high)?
// If this value is set too low, then statistically all
// data should be received via fellow peers anyways

/*
  If there is no more space for client_vector, then do the following:

  Any peers running on the same IP address should be turned off, because they
  probably network with each other and don't offer anything different from
  each other

  Then take away peers with slow network connections (ping time should be the
  measuring factor). They can't relay information easily enough to bother
 */

static std::array<uint64_t, NET_PEER_LIMIT> peer = {0};

static std::vector<uint8_t> net_proto_fetch_chunk(uint32_t size){
	// optimize call for smaller chunks?
	std::vector<uint8_t> retval;
	uint8_t *tmp = new uint8_t[size];
	SDLNet_TCP_Recv(server_socket, tmp, size);
	retval = std::vector<uint8_t>(tmp, tmp+size);
	delete[] tmp;
	tmp = nullptr;
	return retval;
}

net_peer_t::net_peer_t(TCPsocket socket_, std::string ip_, uint16_t port){
	id.add_data(&(ip[0]), NET_IP_MAX_SIZE);
	id.add_data(&(pubkey[0]), PGP_PUBKEY_SIZE);
	/*
	  socket and ip_addr are derived at initialization
	  Some sort of export format will be created so
	  a personal dictionary of nodes can be used
	 */
	socket = socket_;
	if(SDLNet_ResolveHost(&ip_addr, ip_.c_str(), port) == -1){
		PRINT("cannot resolve host", P_ERR);
	}
}

net_peer_t::~net_peer_t(){
	SDLNet_TCP_Close(socket);
}

void net_peer_t::set_ip_socket(std:;string ip_addr_, TCPsocket socket_){
	ip_addr = ip_addr_;
	socket = socket_;
}

static void net_proto_process_incoming_connections(){
	TCPsocket new_socket = SDLNet_TCP_Accept(server_socket);
	if(new_socket == false){
		PRINT((std::string)"cannot accept TCP connection"+
		      SDL_GetError(), P_ERR);
	}else{
		for(uint64_t i = 0;i < NET_PEER_LIMIT;i++){
			if(!id_array::exists(peer[i])){
				net_peer_t *peer_tmp = new net_peer_t();
				peer[i] = peer_tmp.id->get_id();
				peer_tmp->set_ip_socket();
			}
		}
	}
}


static void net_proto_process_incoming_data(){
}

void net_proto::init(){
	uint16_t port = std::stoi(settings::get_setting("port"));
	IPaddress server_ip;
	if(SDLNet_ResolveHost(&server_ip, NULL, port) == -1){
		PRINT((std::string)"cannot resolve host:"+
		      SDLNet_GetError(), P_CRIT);
	}
	server_socket = SDLNet_TCP_Open(&server_ip);
	if(!server_socket){
		PRINT((std::string)"cannot open TCP port:"+
		      SDLNet_GetError(), P_CRIT);
	}
	return;
}

void net_proto::loop(){
	net_proto_process_incoming_connections();
	net_proto_process_incoming_data();
}

void net_proto::close(){
}
