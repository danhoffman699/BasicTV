#include "net_proto_outbound_connections.h"

/*
  Start off by opening as many sockets as possible, and if not all of the
  sockets can be opened at the same time, cycle through the low inbound traffic
  ones to see which ones are the LEAST important. When those are deleted either
  below a certain number or a certain speed, then add different ones. Send
  requests for IDs along the new ones and the ones that receive data back would
  be far higher than the rest, securing that connection. Factor in multiple RSA
  IDs into account, and possibly multithread requests along those sockets.

  TODO: meter inbound and outbound traffic in the lifetime of a socket (and
  more fine-grained stats as well, just not needed right now)
 */

/*
  Don't initiate all connections, but initiate all connections that should be
  initiated at this time (statistically).
*/

static std::vector<std::pair<id_t_, std::vector<id_t_> > > net_proto_generate_proto_socket_index(){
	std::vector<std::pair<id_t_, std::vector<id_t_> > > retval;
	std::vector<id_t_> proto_sockets =
		id_api::cache::get("net_proto_socket_t");
	for(uint64_t i = 0;i < proto_sockets.size();i++){
		net_proto_socket_t *proto_socket =
			PTR_DATA(proto_sockets[i],
				 net_proto_socket_t);
		if(proto_socket == nullptr){
			continue;
		}
		const id_t_ cite_id =
			proto_socket->id.get_encrypt_cite_id();
		bool append = false;
		for(uint64_t r = 0;r < retval.size();r++){
			if(cite_id == retval[i].first){
				retval[i].second.push_back(cite_id);
				append = true;
			}
		}
		if(append){
			break;
		}
		retval.push_back(
			std::make_pair(
				cite_id,
				std::vector<id_t_>(
					proto_socket->id.get_id())));
	}
	return retval;
}

// vector of the encryption ID and all of the stats information for all sockets

static std::vector<std::pair<id_t_, std::vector<std::pair<id_t_, std::vector<std::pair<uint64_t, uint64_t> > > > > > net_proto_generate_bandwidth_index_from_proto_socket_index(
	std::vector<std::pair<id_t_, std::vector<id_t_> > > socket_index){
	// encryption ID, number of sockets, total throughput
	std::vector<std::pair<id_t_, std::vector<std::pair<id_t_, std::vector<std::pair<uint64_t, uint64_t> > > > > > retval;
	for(uint64_t i = 0;i < socket_index.size();i++){
		// sockets with the same encryption ID
		std::vector<std::pair<id_t_, std::vector<std::pair<uint64_t, uint64_t> > > > socket_id_set;
		for(uint64_t s = 0;s < socket_index[i].second.size();s++){
			net_proto_socket_t *proto_socket =
				PTR_DATA(socket_index[i].second[s],
					 net_proto_socket_t);
			if(proto_socket == nullptr){
				continue;
			}
			net_socket_t *socket =
				PTR_DATA(proto_socket->get_socket_id(),
					 net_socket_t);
			if(socket == nullptr){
				continue;
			}
			stat_sample_set_t *inbound_sample_set =
				PTR_DATA(socket->get_inbound_stat_sample_set_id(),
					 stat_sample_set_t);
			if(inbound_sample_set == nullptr){
				continue;
			}
			std::pair<id_t_, std::vector<std::pair<uint64_t, uint64_t> > > tmp_stat =
				std::make_pair(
					proto_socket->id.get_id(),
					inbound_sample_set->get_samples());
			socket_id_set.push_back(tmp_stat);
		}
		std::pair<id_t_, std::vector<std::pair<id_t_, std::vector<std::pair<uint64_t, uint64_t> > > > > encrypt_id_pair;
		encrypt_id_pair.first = socket_index[i].first;
		encrypt_id_pair.second = socket_id_set;
		retval.push_back(encrypt_id_pair);
	}
	return retval;
}

void net_proto_loop_initiate_all_connections(){
	// list of encryption IDs and their associated sockets
	std::vector<std::pair<id_t_, std::vector<id_t_> > > proto_socket_index =
		net_proto_generate_proto_socket_index();
	return;
}
