#include "net_proto.h"
#include "net_proto_con_req.h"

net_proto_con_req_t::net_proto_con_req_t() : id(this, __FUNCTION__){
	ADD_DATA(flags);
	ADD_DATA(first_peer_id);
	ADD_DATA(second_peer_id);
	ADD_DATA(third_peer_id);
	ADD_DATA(heartbeat_timestamp);
}

net_proto_con_req_t::~net_proto_con_req_t(){
}

void net_proto_con_req_t::set_ids(id_t_ first_peer_id_,
				  id_t_ second_peer_id_,
				  id_t_ third_peer_id_){
	first_peer_id = first_peer_id_;
	second_peer_id = second_peer_id_;
	third_peer_id = third_peer_id_;
}

void net_proto_con_req_t::set_ids(id_t_ *first_peer_id_,
				  id_t_ *second_peer_id_,
				  id_t_ *third_peer_id_){
	*first_peer_id_ = first_peer_id;
	*second_peer_id_ = second_peer_id;
	*third_peer_id_ = third_peer_id;
}

void net_proto_con_req_t::set_proto(uint8_t proto){
	flags &= 0b11111101;
	flags |= proto << 1;
}

uint8_t net_proto_con_req_t::get_proto(){
	return (flags >> 1) & 1;
}

void net_proto_con_req_t::set_con_type(uint8_t con_type){
	flags &= 0b11111110;
	flags |= con_type;
}

uint8_t net_proto_con_req_t::get_con_type(){
	return (flags >> 0) & 1;
}

void net_proto_con_req_t::set_heartbeat_timestamp(uint64_t heartbeat_timestamp_){
	heartbeat_timestamp = heartbeat_timestamp_;
}

uint64_t net_proto_con_req_t::get_heartbeat_timestamp(){
	return heartbeat_timestamp;
}
