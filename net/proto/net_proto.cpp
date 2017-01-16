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

static uint64_t incoming_id = 0;

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
	uint16_t tmp_port = 0;
	try{
		tmp_port = (uint16_t)std::stoi(settings::get_setting("network_port"));
	}catch(std::exception e){
		print("cannot pull port from settings", P_ERR);
	}
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

net_proto_request_t::net_proto_request_t(): id(this, __FUNCTION__){
	// list of ids
	id.add_data(&(ids), NET_REQUEST_MAX_LENGTH);
	// only used for blacklist or whitelist
	id.add_data(&flags, 1);
}

net_proto_request_t::~net_proto_request_t(){
}

void net_proto_request_t::set_flags(uint8_t flags_){
	flags = flags_;
}

uint8_t net_proto_request_t::get_flags(){
	return flags;
}

void net_proto_request_t::set_ids(std::vector<id_t_> ids_){
	ids = ids_;
}

std::vector<id_t_> net_proto_request_t::get_ids(){
	return ids;
}

void net_proto_request_t::set_proto_socket_id(id_t_ socket_id_){
	socket_id = socket_id_;
}

uint64_t net_proto_request_t::get_proto_socket_id(){
	return socket_id;
}

std::array<uint8_t, TYPE_LENGTH> net_proto_request_t::get_type(){
	return type;
}

void net_proto_request_t::set_type(std::array<uint8_t, TYPE_LENGTH> type_){
	type = type_;
}

uint64_t net_proto_request_t::get_last_query_timestamp_micro_s(){
	return last_query_timestamp_micro_s;
}
