#include "net_proto.h"
#include "net_proto_peer.h"

net_peer_t::net_peer_t() : id(this, __FUNCTION__){
	list_virtual_data(&id); // net_ip_t
	id.add_data(&(bitcoin_wallet[0]), BITCOIN_WALLET_LENGTH);
}

net_peer_t::~net_peer_t(){
}

void net_peer_t::set_bitcoin_wallet(std::array<uint8_t, BITCOIN_WALLET_LENGTH> bitcoin_wallet_){
	bitcoin_wallet = bitcoin_wallet_;
}

std::array<uint8_t, BITCOIN_WALLET_LENGTH> net_peer_t::get_bitcoin_wallet(){
	return bitcoin_wallet;
}
