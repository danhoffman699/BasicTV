#include "main.h"
#include "util.h"
#include "lock.h"
#include "net_proto.h"
#include "settings.h"

static TCPsocket server_socket;

static std::vector<net_client_t> client_vector;

net_client_t::net_client_t(TCPsocket socket_, std::string ip_, uint16_t port){
	socket = socket_;
	if(SDLNet_ResolveHost(&ip_addr, ip_.c_str(), port) == -1){
		PRINT("cannot resolve host", P_ERR);
	}
}

net_client_t::~net_client_t(){
	SDLNet_TCP_Close(socket);
}

static void net_proto_process_incoming_connections(){
	TCPsocket new_socket;
	while(!(new_socket = SDLNet_TCP_Accept(server_socket))){
		PRINT((std::string)"cannot accept TCP connection"+
		      SDL_GetError(), P_ERR);
	}
}

std::vector<uint8_t> net_proto_fetch_chunk(uint32_t size){
	// optimize call for smaller chunks?
	std::vector<uint8_t> retval;
	uint8_t *tmp = new uint8_t[size];
	SDLNet_TCP_Recv(server_socket, tmp, size);
	retval = std::vector<uint8_t>(tmp, tmp+size);
	delete[] tmp;
	tmp = nullptr;
	return retval;
}

#define ENCRYPTION 0
#define COMPRESSED 1

static void net_proto_process_incoming_data(){
	std::vector<uint8_t> metadata = net_proto_fetch_chunk(4);
	const uint8_t flags_byte = metadata[0];
	bool flags[8] = {0};
	flags[ENCRYPTION] = flags_byte & NET_PROTO_FLAG_ENCRYPTION;
	flags[COMPRESSED] = flags_byte & NET_PROTO_FLAG_COMPRESSED;
	const uint32_t raw_data_size =
		(metadata[1]) | (metadata[2] << 8) | (metadata[3] << 16);
	std::vector<uint8_t> data = net_proto_fetch_chunk(raw_data_size);
	if(flags[COMPRESSED]){
		PRINT((std::string)"compression is not configured", P_CRIT);
		// set data_size to size of uncompressed data
	}else{
	}
	
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
