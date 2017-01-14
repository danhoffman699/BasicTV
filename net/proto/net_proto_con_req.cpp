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

uint8_t net_proto_con_req_t::get_flags(){
	return flags;
}

void net_proto_con_req_t::get_peer_ids(id_t_ *first_peer_id_,
				       id_t_ *second_peer_id_,
				       id_t_ *third_peer_id_){
	*first_peer_id_ = first_peer_id;
	*second_peer_id_ = second_peer_id;
	*third_peer_id_ = third_peer_id;
}

uint64_t net_proto_con_req_t::get_heartbeat_timestamp(){
	return heartbeat_timestamp;
}

void net_proto_con_req_t::set(uint8_t flags_,
			      id_t_ first_peer_id_,
			      id_t_ second_peer_id_,
			      id_t_ third_peer_id_,
			      uint64_t heartbeat_timestamp_){
	flags = flags_;
	first_peer_id = first_peer_id_;
	second_peer_id = second_peer_id_;
	third_peer_id = third_peer_id_;
	heartbeat_timestamp = heartbeat_timestamp_;
}
