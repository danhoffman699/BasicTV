#include "../main.h"
#include "../util.h"
#include "../lock.h"
#include "../settings.h"
#include "net.h"
#include "net_proto.h"
#include "net_socket.h"

/*
  TODO: actually make a version of this that is compliant to the spec. There
  should be no main server, but a list of peers that forward PGP data.
 */

static uint64_t incoming_id = 0;

net_peer_t::net_peer_t() : id(this, __FUNCTION__){
	id.add_data(&(ip[0]), NET_IP_MAX_SIZE);
	id.add_data(&(port), sizeof(port));
}

net_peer_t::~net_peer_t(){
}

void net_peer_t::set_ip_addr(IPaddress ip_addr_){
	const char *ip_str = SDLNet_ResolveIP(&ip_addr_);
	memcpy(&(ip[0]), ip_str, strlen(ip_str));
	port = ip_addr_.port;
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
	}
}

/*
  Compiles a list of all net_socket_t's and checks all of them for data. Reads
  one byte at a time and just outputs it to the screen. Isn't meant to be more
  than a test for net_socket_t
 */

static void net_proto_loop_dummy_read(){
	std::vector<uint64_t> all_sockets = id_array::all_of_type("net_socket_t");
	for(uint64_t i = 0;i < all_sockets.size();i++){
		net_socket_t *socket_ = (net_socket_t*)id_array::ptr_data(all_sockets[i]);
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
			}
		}
	}
}

static void net_proto_loop(){
	/*
	  TODO: finish net_socket_t work before implementing
	 */
	net_socket_t *incoming_socket = (net_socket_t*)id_array::ptr_data(incoming_id);
	if(incoming_socket == nullptr){
		print("incoming_socket == nullptr", P_ERR);
	}
	//if(incoming_socket->activity()){
		net_proto_loop_accept_conn(incoming_socket);
		//}
	net_proto_loop_dummy_read();
}

void net_proto_init(){
	net_socket_t *incoming = new net_socket_t;
	incoming_id = incoming->id.get_id();
	uint16_t tmp_port = 0;
	try{
		tmp_port = std::stoi(settings::get_setting("network_port"));
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
			uint64_t level = P_WARN;
			if(settings::get_setting("socks_strict") == "true"){
				level = P_ERR;
			}
			print("unable to configure SOCKS", level);
		}
	}else{
		print("SOCKS has been disabled", P_NOTE);
		incoming->connect({"", tmp_port});
	}
	function_vector.push_back(net_proto_loop);
}
