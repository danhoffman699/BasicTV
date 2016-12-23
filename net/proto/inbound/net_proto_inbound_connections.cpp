#include "net_proto_inbound.h"
#include "net_proto_inbound_connections.h"

/*
  Follows internal flow chart
*/

std::vector<std::pair<std::string, uint16_t> > seeds = {
	{"96.35.0.163", 58486} // not seriously being ran yet
};

/*
  Don't directly import net_peer_t, but instead open a connection, request
  all net_peer_t data, and destroy the local version (their local, encrypted
  net_peer_t should be sent over)
*/
static void net_proto_bootstrap_add_seeds(){
	if(seeds.size() == 0){
		print("there are no hard-coded seeds", P_ERR);
	}
	net_socket_t *boot_socket =
		new net_socket_t;
	for(uint64_t i = 0;i < seeds.size();i++){
		try{
			boot_socket->connect(seeds[i], NET_SOCKET_TCP);
			while(boot_socket->is_active() == false){
				// wait for net_peer_t to be sent
				ms_sleep(50);
			}
			
		}catch(...){
			print("cannot query seed", P_ERR);
		}
	}
	delete boot_socket;
	boot_socket = nullptr;
}

static void net_proto_bootstrap(){
	std::vector<id_t_> con_req_ids =
		id_api::cache::get("net_proto_con_req_t");
	while(con_req_ids.size() == 0){
		net_proto_bootstrap_add_seeds();
		con_req_ids =
			id_api::cache::get("net_proto_con_req_t");
	}
	for(uint64_t i = 0;i < con_req_ids.size();i++){
		
	}
}

void net_proto_loop_accept_all_connections(net_socket_t *incoming_socket){
	TCPsocket tmp_tcp_socket = SDLNet_TCP_Accept(incoming_socket->get_tcp_socket());
	if(tmp_tcp_socket != nullptr){
		// actually do something
	}
}
