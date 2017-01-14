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
}

static std::vector<std::tuple<id_t_, uint32_t, uint64_t> > net_proto_generate_bandwidth_index(){
	// encryption ID, number of sockets, total throughput
	std::vector<std::tuple<id_t_, uint32_t, uint64_t> > retval;
	
	return retval;
}

void net_proto_loop_initiate_all_connections(){
	// list of encryption IDs and their associated sockets
	std::vector<std::pair<id_t_, std::vector<id_t_> > > socket_index =
		net_proto_generate_proto_socket_index();
	std::vector<std::tuple<id_t_, uint32_t, uint64_t> > bandwidth_index =
		net_proto_generate_bandwidth_index();
	for(uint64_t i = 0;i < socket_index.size();i++){
		uint64_t bandwidth = 0;
		for(uint64_t b = 0;b < socket_index[i].second.size();b++){
			net_proto_socket_t *proto_socket =
				PTR_DATA(socket_index[i].second[b],
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
			// Fetch the bandwidth from the socket (when that is
			// implemented) and add it to bandwidth variable
		}
	}
	return;
}
