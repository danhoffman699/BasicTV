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

  Returns a vector with the cite ID and all sockets with that cite ID
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
			proto_socket->id.get_encrypt_pub_key_id();
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
		std::pair<id_t_, std::vector<id_t_> > pushed_data =
			std::make_pair(
				cite_id,
				std::vector<id_t_>(
				{proto_socket->id.get_id()}));
		retval.push_back(pushed_data);
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

/*
  If Tor (or another proxy) is being used, and we have multiple sockets that
  can route the information (or "circuits"), then we can establish multiple
  redundant sockets over all of these sockets. There is a set download speed
  for each socket, and when that is exceeded in a certain interval (say 10kbps
  for Tor), then establish another connection on another socket. 
 
  All connections are made assuming a Tor-like setup (no inbound TCP connecions
  are allowed, holepunching is allowed), and a new net_peer_t is created and
  associated with a net_proto_socket_t.
  
  It is assumed throughout this program that two different sockets with matching
  encryption schemes are just two seperate connections to the same physical
  computer, and statistically likely requests to one socket is just as likely
  on the other.

  If BasicTV isn't configured through settings.cfg to use Tor or to use a
  list of different proxies, then establishing multiple sockets to a peer
  is useless.
 */

std::vector<id_t_> net_proto_masquerade_connections(){
	// // list of encryption IDs and their associated sockets
	// std::vector<std::pair<id_t_, std::vector<id_t_> > > proto_socket_index =
	// 	net_proto_generate_proto_socket_index();
	// std::vector<std::pair<id_t_, std::vector<std::pair<id_t_, std::vector<std::pair<uint64_t, uint64_t> > > > > > bandwidth_index =
	// 	net_proto_generate_bandwidth_index_from_proto_socket_index(
	// 		proto_socket_index);
	// std::vector<id_t_> retval;
	// for(uint64_t i = 0;i < proto_socket_index.size();i++){
	// }
	// return retval;
	return std::vector<id_t_>();
}

/*
  The limit set in settings.cfg for socket creation is intentionally low to
  make things work on crappy routers. The first "hard" limit for myself is
  ulimit restricting the number of open file descriptors (keep reading for work
  around).

  File descriptors and network sockets overlap in all areas except for proxies.
  More sockets can be created than what ulimit and SDL2 can by running multiple
  sockets over SOCKS. 

  TODO: seamlessly masquerade multiple net_socket_t over a SOCKS connection
  somehow
 */

void net_proto_loop_initiate_all_connections(){
	std::vector<id_t_> peer_list;
	return;
}
