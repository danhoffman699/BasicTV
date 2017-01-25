#include "../../main.h"
#include "../../util.h"
#include "../../lock.h"
#include "../../settings.h"
#include "../net.h"
#include "../net_socket.h"
#include "../../id/id_api.h"

#include "net_proto.h"
#include "net_proto_dev_ctrl.h"
#include "inbound/net_proto_inbound_connections.h"
#include "inbound/net_proto_inbound_data.h"
#include "outbound/net_proto_outbound_connections.h"
#include "outbound/net_proto_outbound_data.h"
#include "net_proto_meta.h"

static id_t_ incoming_id = ID_BLANK_ID;

void net_proto_loop(){
	net_socket_t *incoming_socket =
		PTR_DATA(incoming_id, net_socket_t);
	if(incoming_socket == nullptr){
		print("incoming_socket == nullptr", P_ERR);
	}
	// all things inbound
	net_proto_loop_handle_inbound_requests();
	net_proto_loop_accept_all_connections();
	// all things outbound
	net_proto_loop_handle_outbound_requests();
	net_proto_loop_initiate_all_connections();
}

void net_proto_init(){
	net_socket_t *incoming = new net_socket_t;
	incoming_id = incoming->id.get_id();
	// uint16_t tmp_port = 0;
	// try{
	// 	tmp_port = (uint16_t)std::stoi(settings::get_setting("network_port"));
	// }catch(std::exception e){
	// 	print("cannot pull port from settings", P_ERR);
	// }
	// TODO: reimplement this when the information is done
	// if(settings::get_setting("socks_enable") == "true"){
	// 	try{
	// 		std::string socks_proxy_ip = settings::get_setting("socks_proxy_ip");
	// 		uint16_t socks_proxy_port =
	// 			std::stoi(settings::get_setting("socks_proxy_port"));
	// 		if(socks_proxy_ip == ""){
	// 			throw std::runtime_error("");
	// 		}
	// 		incoming->enable_socks(
	// 			std::make_pair(socks_proxy_ip,
	// 				       socks_proxy_port),
	// 			std::make_pair("",
	// 				       tmp_port)
	// 			);
	// 	}catch(std::exception e){
	// 		uint32_t level = P_WARN;
	// 		if(settings::get_setting("socks_strict") == "true"){
	// 			level = P_ERR;
	// 		}
	// 		print("unable to configure SOCKS", level);
	// 	}
	// }else{
	// 	print("SOCKS has been disabled", P_NOTE);
	// 	incoming->connect({"", tmp_port});
	// }
}

void net_proto_close(){
	// doesn't do anything, GC takes care of all data types
	// All data types should destroy any internal data
}

