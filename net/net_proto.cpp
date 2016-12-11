#include "../main.h"
#include "../util.h"
#include "../lock.h"
#include "../settings.h"
#include "net.h"
#include "net_proto.h"
#include "net_socket.h"
#include "../id/id_api.h"

/*
  TODO: actually make a version of this that is compliant to the spec.
 */

static uint64_t incoming_id = 0;

net_peer_t::net_peer_t() : id(this, __FUNCTION__){
	id.add_data(&(ip[0]), NET_IP_MAX_SIZE);
	id.add_data(&(port), sizeof(port));
	id.add_data(&(bitcoin_wallet[0]), BITCOIN_WALLET_LENGTH);
}

net_peer_t::~net_peer_t(){
}

void net_peer_t::set_ip_addr(IPaddress ip_addr_){
	const char *ip_str = SDLNet_ResolveIP(&ip_addr_);
	memcpy(&(ip[0]), ip_str, strlen(ip_str));
	port = ip_addr_.port;
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

#define READ_DATA_META(ptr) \
	if(ptr != nullptr){						\
		if(offset + sizeof(*ptr) > data_length){		\
			print("packet metadata too large", P_ERR);	\
		}else{							\
			memcpy(ptr, &data[0], sizeof(*ptr));		\
		}							\
	}								\
	offset += sizeof(*ptr);						\
	

static void net_proto_read_packet_metadata(uint8_t *data,
					   uint32_t data_length, // input data
					   uint32_t *data_size, // payload data
					   uint8_t *ver_major,
					   uint8_t *ver_minor,
					   uint8_t *ver_patch,
					   uint8_t *compression_macros,
					   uint8_t *other_macros,
					   uint32_t *unused){
	uint8_t offset = 0;
	READ_DATA_META(data_size);
	READ_DATA_META(ver_major);
	READ_DATA_META(ver_minor);
	READ_DATA_META(ver_patch);
	READ_DATA_META(compression_macros);
	READ_DATA_META(other_macros);
	READ_DATA_META(unused);
}

/*
  Facilitates incoming connections
 */

static void net_proto_loop_accept_conn(net_socket_t *incoming_socket){
	TCPsocket tmp_tcp_socket = SDLNet_TCP_Accept(incoming_socket->get_tcp_socket());
	if(tmp_tcp_socket != nullptr){
		print("added a new socket", P_NOTE);
		net_socket_t *tmp_socket = new net_socket_t;
		tmp_socket->set_tcp_socket(tmp_tcp_socket);
		net_peer_t *peer = new net_peer_t;
		peer->add_socket_id(tmp_socket->id.get_id());
	}
}

/*
  Compiles a list of all net_socket_t's and checks all of them for data. Reads
  one byte at a time and just outputs it to the screen. Isn't meant to be more
  than a test for net_socket_t
 */

static void net_proto_loop_dummy_read(){
	std::vector<uint64_t> all_sockets =
		id_api::cache::get("net_socket_t");
	for(uint64_t i = 0;i < all_sockets.size();i++){
		net_socket_t *socket_ =
			PTR_DATA(all_sockets[i], net_socket_t);
		if(socket_ == nullptr){
			print("socket is nullptr", P_ERR);
			continue;
		}
		if(socket_->get_client_conn().first == ""){
			// inbound
			continue;
		}
		if(socket_->activity()){
			print("detected activity on a socket", P_SPAM);
			std::vector<uint8_t> incoming_data;
			while((incoming_data = socket_->recv(1, NET_SOCKET_RECV_NO_HANG)).size() != 0){
				print(std::to_string((int)incoming_data[0]), P_NOTE);
				sleep_ms(1000);
			}
		}
	}
}

static void net_proto_loop_read_all_valid_data(net_socket_t *socket){
	if(unlikely(socket == nullptr)){
		return;
		// should never happen, already check in the parent
	}
	std::vector<uint8_t> all_current_data;
	std::vector<uint8_t> tmp;
	while((tmp = socket->recv(1, NET_SOCKET_RECV_NO_HANG)).size() > 0){
		// better functions need to be made to read the entire
		// local buffer into an array for bulk processing
		all_current_data.push_back(tmp[0]);
	}
	// Reads the data until it comes across a new (valid) metadata
	// add a space or carriage return or something to keep this from
	// returning halfway through a random occurance of DEV_CTRL_1
	// in the data (other half got chopped off)
	for(uint64_t i = 0;i < all_current_data.size()-NET_PROTO_META_LENGTH;i++){
		if(all_current_data[i] == NET_PROTO_DEV_CTRL_1 &&
		   all_current_data[i+1] != NET_PROTO_DEV_CTRL_1){
			uint32_t data_size = 0;
			// don't care about the versions
			uint8_t compression_macros = 0;
			uint8_t other_macros = 0;
			// don't care about the unused
			net_proto_read_packet_metadata(
				&all_current_data[i],
				all_current_data.size()-i,
				&data_size,
				nullptr,
				nullptr,
				nullptr,
				&compression_macros,
				&other_macros,
				nullptr);
			i += NET_PROTO_META_LENGTH;
			
		}
	}
}

void net_proto_loop_handle_inbound_requests(){
	net_socket_t *tmp_socket = nullptr;
	std::vector<uint64_t> peer_id_list =
		id_api::cache::get("net_peer_t");
	for(uint64_t i = 0;i < peer_id_list.size();i++){
		net_peer_t *peer =
			PTR_DATA(peer_id_list[i], net_peer_t);
		for(uint64_t s = 0;s < NET_PROTO_MAX_SOCKET_PER_PEER;s++){
			tmp_socket =
				PTR_DATA(peer->get_socket_id(s),
					 net_socket_t);
			if(likely(tmp_socket == nullptr)){
				continue;
			}
			net_proto_loop_read_all_valid_data(tmp_socket);
			// std::vector<uint8_t> packet_size_vector =
			// 	tmp_socket->recv(4, NET_SOCKET_RECV_NO_HANG);
			// uint32_t packet_size =
			// 	*((uint32_t*)&(packet_size_vector[0]));
			// // do some sanity checks on this
			// std::vector<uint8_t> packet_data =
			// 	tmp_socket->recv(packet_size,
			// 			 NET_SOCKET_RECV_NO_HANG);
			
		}
	}
}

void net_proto_loop_handle_outbound_requests(){}

void net_proto_loop_handle_all_requests(){
	net_proto_loop_handle_inbound_requests();
	net_proto_loop_handle_outbound_requests();
}

void net_proto_loop(){
	/*
	  TODO: finish net_socket_t work before implementing
	 */
	net_socket_t *incoming_socket =
		PTR_DATA(incoming_id, net_socket_t);
	if(incoming_socket == nullptr){
		print("incoming_socket == nullptr", P_ERR);
	}
	net_proto_loop_accept_conn(incoming_socket);
	net_proto_loop_dummy_read();
	//net_proto_loop_handle_all_requests();
}

void net_proto_init(){
	net_socket_t *incoming = new net_socket_t;
	incoming_id = incoming->id.get_id();
	uint16_t tmp_port = 0;
	try{
		tmp_port = (uint16_t)std::stoi(settings::get_setting("network_port"));
	}catch(std::exception e){
		print("cannot pull port from settings", P_ERR);
	}
	if(settings::get_setting("socks_enable") == "true"){
		try{
			std::string socks_proxy_ip = settings::get_setting("socks_proxy_ip");
			uint16_t socks_proxy_port =
				std::stoi(settings::get_setting("socks_proxy_port"));
			if(socks_proxy_ip == ""){
				throw std::runtime_error("");
			}
			incoming->enable_socks(
				std::make_pair(socks_proxy_ip, socks_proxy_port), std::make_pair("", tmp_port));
		}catch(std::exception e){
			uint32_t level = P_WARN;
			if(settings::get_setting("socks_strict") == "true"){
				level = P_ERR;
			}
			print("unable to configure SOCKS", level);
		}
	}else{
		print("SOCKS has been disabled", P_NOTE);
		incoming->connect({"", tmp_port});
	}
}

void net_proto_close(){
	// doesn't do anything, GC takes care of all data types
	// All data types should destroy any internal data
}
