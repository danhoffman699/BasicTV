#include "net_proto.h"
#include "net_proto_request.h"
#include "net_proto_socket.h"
#include "../../id/id_api.h"

net_proto_request_t::net_proto_request_t(): id(this, __FUNCTION__){
	// ids
	id.add_data(&(ids), 65536);
	// modiciation incrementors
	id.add_data(&(mod), 65536);
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
	for(uint64_t i = 0;i < ids_.size();i++){
		uint64_t mod_inc = 0;
		data_id_t *id_ =
			PTR_ID_FAST(ids_[i], );
		if(id_ != nullptr){
			mod_inc = id_->get_mod_inc();
		}
		mod.push_back(mod_inc);
	}
}

std::vector<id_t_> net_proto_request_t::get_ids(){
	if(ids.size() != mod.size()){
		print("id and mod vectors aren't the same size", P_ERR);
	}
	return ids;
}

std::vector<uint64_t> net_proto_request_t::get_mod(){
	if(ids.size() != mod.size()){
		print("id and mod vectors aren't the same size", P_ERR);
	}
	return mod;
}

void net_proto_request_t::set_proto_socket_id(id_t_ socket_id_){
	socket_id = socket_id_;
}

id_t_ net_proto_request_t::get_proto_socket_id(){
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

/*
  Goes through all of the net_proto_socket_t entries and run a stats algorithm
  on their input samples to sort sockets from most preferable to least
  preferable. All sockets will be on this list

  TODO: randomize equal probability sockets (ones where no data is given), since
  requests for type cache is oldest to newest

  TODO: take ping times and quality of connection into account, but we need a
  good way of prioritizing information

  TODO: when linked lists with multiple neighbors are implemented, add that to 
  this search function to increase
 */

/*
  TODO: Optimize this further and set all matching sockets (sockets whose peer's
  ID fingerprint matches) to the highest probability out of all of them. This
  allows a slight hint from connection quality to take over (especially nice for
  multiple Tor connections).
 */

void net_proto_request_t::update_probs(){
	prob_of_id.clear();
	std::vector<id_t_> all_net_proto_sockets =
		id_api::cache::get(
			"net_proto_socket_t");
	for(uint64_t i = 0;i < ids.size();i++){
		std::vector<std::pair<id_t_, uint16_t> > socket_prob_vector;
		for(uint64_t c = 0;c < all_net_proto_sockets.size();c++){
			net_proto_socket_t *proto_socket =
				PTR_DATA(all_net_proto_sockets[c],
					 net_proto_socket_t);
			if(proto_socket == nullptr){
				continue;
			}
			std::pair<id_t_, uint16_t> prob_pair;
			prob_pair.first = all_net_proto_sockets[c];
			prob_pair.second =
				proto_socket->get_prob_of_id(
					all_net_proto_sockets[c]);
			socket_prob_vector.push_back(
				prob_pair);
		}
		// TODO: spread the highest probability of one socket across
		// all sockets that share the same fingerprint right here
	}
}
