#include "net_proto.h"
#include "net_proto_peer.h"

net_peer_t::net_peer_t() : id(this, __FUNCTION__){
	id.add_data(&(ip[0]), NET_IP_MAX_SIZE);
	id.add_data(&(port), sizeof(port));
	id.add_data(&(bitcoin_wallet[0]), BITCOIN_WALLET_LENGTH);
}

net_peer_t::~net_peer_t(){
}

void net_peer_t::set_ip_addr(uint32_t ip_, uint16_t port_){
	memset(&(ip[0]), 0, NET_IP_RAW_SIZE);
	memcpy(&(ip[0]), &ip_, 4);
	port = port_;
}

void net_peer_t::set_ip_addr(std::array<uint8_t, NET_IP_RAW_SIZE> ip_, uint16_t port_){
	memcpy(&(ip[0]), &(ip_[0]), NET_IP_RAW_SIZE);
	port = port_;
}

void net_peer_t::add_socket_id(uint64_t socket_){
	for(uint64_t i = 0;i < NET_PROTO_MAX_SOCKET_PER_PEER;i++){
		if(socket[i] == 0){
			socket[i] = socket_;
			break;
		}
	}
}

uint64_t net_peer_t::get_socket_id(uint32_t entry_){
	if(likely(entry_ < NET_PROTO_MAX_SOCKET_PER_PEER)){
		return socket[entry_];
	}else{
		return 0;
	}
}

void net_peer_t::del_socket_id(uint64_t socket_){
	for(uint64_t i = 0;i < NET_PROTO_MAX_SOCKET_PER_PEER;i++){
		if(socket[i] == socket_){
			socket[i] = 0;
			// not guaranteed to only be in here once
		}
	}
}
