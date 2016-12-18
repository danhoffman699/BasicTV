#include "../main.h"
#include "../util.h"
#include "../lock.h"
#include "../settings.h"
#include "net.h"
#include "net_proto.h"
#include "net_socket.h"
#include "../id/id_api.h"

/*
  TODO: actually make a version of this that is compliant to the spec.
 */

static uint64_t incoming_id = 0;

#define WRITE_DATA_META(data) retval.insert(retval.end(), &data, &data+sizeof(data))

static std::vector<uint8_t> net_proto_write_packet_metadata(
	net_proto_standard_size_t data_size,
	net_proto_standard_ver_t ver_major,
	net_proto_standard_ver_t ver_minor,
	net_proto_standard_ver_t ver_patch,
	net_proto_standard_macros_t compression_macros,
	net_proto_standard_macros_t other_macros,
	net_proto_standard_unused_t unused){
	std::vector<uint8_t> retval;
	// payload of the entire structure (not counting any metadata)
	WRITE_DATA_META(data_size);
	// version data is optional, omitted with 0.0.0 (or anything)
	WRITE_DATA_META(ver_major);
	WRITE_DATA_META(ver_minor);
	WRITE_DATA_META(ver_patch);
	// no current use for the following
	WRITE_DATA_META(other_macros);
	WRITE_DATA_META(unused);
	return retval;
}


#define READ_DATA_META(ptr)						\
	if(ptr != nullptr){						\
		if(offset + sizeof(*ptr) > data_length){		\
			print("packet metadata too large", P_ERR);	\
		}else{							\
			memcpy(ptr, &data[0], sizeof(*ptr));		\
		}							\
	}								\
	offset += sizeof(*ptr);						\

static void net_proto_read_packet_metadata(uint8_t *data,
					   uint32_t data_length, // input data
					   net_proto_standard_size_t *data_size, // payload data
					   net_proto_standard_ver_t *ver_major,
					   net_proto_standard_ver_t *ver_minor,
					   net_proto_standard_ver_t *ver_patch,
					   net_proto_standard_macros_t *other_macros,
					   net_proto_standard_unused_t *unused){
	uint64_t offset = 0;
	READ_DATA_META(data_size);
	READ_DATA_META(ver_major);
	READ_DATA_META(ver_minor);
	READ_DATA_META(ver_patch);
	READ_DATA_META(other_macros);
	READ_DATA_META(unused);
}


static void net_proto_loop_accept_conn(net_socket_t *incoming_socket){
	TCPsocket tmp_tcp_socket = SDLNet_TCP_Accept(incoming_socket->get_tcp_socket());
	if(tmp_tcp_socket != nullptr){
		print("added a new socket", P_NOTE);
		net_socket_t *tmp_socket = new net_socket_t;
		tmp_socket->set_tcp_socket(tmp_tcp_socket);
		net_peer_t *peer = new net_peer_t;
		peer->add_socket_id(tmp_socket->id.get_id());
	}
}

// reads from all net_socket_t, for testing only
static void net_proto_loop_dummy_read(){
	std::vector<uint64_t> all_sockets =
		id_api::cache::get("net_socket_t");
	for(uint64_t i = 0;i < all_sockets.size();i++){
		net_socket_t *socket_ =
			PTR_DATA(all_sockets[i], net_socket_t);
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
				P_V_E((uint64_t)incoming_data[0], P_NOTE);
			}
		}
	}
}

/*
  If the current position is the start of metadata and the entire segment has
  been read, then read all of the information, append it to the vector, and
  repeat this process.

  If the current position is the start of metadata and the entire segment has
  not been read, then return nothing (hanging is bad behavior).

  If the current position is not the start of metadata, then read back until 
  metadata has been found, and apply the logic of the first or second 
  paragraph. If no metadata is found, return nothing.
 */

std::vector<std::vector<uint8_t> > net_proto_get_struct_segments(net_socket_t *socket){
	return {};
}

static void net_proto_loop_read_all_valid_data(net_socket_t *socket){
	throw std::runtime_error("not finished implementing");
	if(unlikely(socket == nullptr)){
		return;
		// should never happen, already check in the parent
	}
	std::vector<std::vector<uint8_t> > struct_segments =
		net_proto_get_struct_segments(socket);
	for(uint64_t i = 0;i < struct_segments.size();i++){
		net_proto_standard_size_t data_size = 0;
		net_proto_standard_macros_t other_macros = 0;
		net_proto_read_packet_metadata(
			struct_segments[i].data(),
			struct_segments[i].size(),
			&data_size,
			nullptr,
			nullptr,
			nullptr,
			&other_macros,
			nullptr);
		i += NET_PROTO_META_LENGTH;
		// chuck the generated string into the ID API
	}
}

void net_proto_loop_handle_inbound_requests(){
	net_socket_t *tmp_socket = nullptr;
	std::vector<uint64_t> peer_id_list =
		id_api::cache::get("net_peer_t");
	for(uint64_t i = 0;i < peer_id_list.size();i++){
		net_peer_t *peer =
			PTR_DATA(peer_id_list[i], net_peer_t);
		for(uint64_t s = 0;s < NET_PROTO_MAX_SOCKET_PER_PEER;s++){
			tmp_socket =
				PTR_DATA(peer->get_socket_id(s),
					 net_socket_t);
			if(likely(tmp_socket == nullptr)){
				continue;
			}
			net_proto_loop_read_all_valid_data(tmp_socket);
			// std::vector<uint8_t> packet_size_vector =
			// 	tmp_socket->recv(4, NET_SOCKET_RECV_NO_HANG);
			// uint32_t packet_size =
			// 	*((uint32_t*)&(packet_size_vector[0]));
			// // do some sanity checks on this
			// std::vector<uint8_t> packet_data =
			// 	tmp_socket->recv(packet_size,
			// 			 NET_SOCKET_RECV_NO_HANG);
			
		}
	}
}

void net_proto_loop_handle_outbound_requests(){}

void net_proto_loop_handle_all_requests(){
	net_proto_loop_handle_inbound_requests();
	net_proto_loop_handle_outbound_requests();
}

void net_proto_loop(){
	/*
	  TODO: finish net_socket_t work before implementing
	 */
	net_socket_t *incoming_socket =
		PTR_DATA(incoming_id, net_socket_t);
	if(incoming_socket == nullptr){
		print("incoming_socket == nullptr", P_ERR);
	}
	net_proto_loop_accept_conn(incoming_socket);
	net_proto_loop_dummy_read();
	//net_proto_loop_handle_all_requests();
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
			uint32_t level = P_WARN;
			if(settings::get_setting("socks_strict") == "true"){
				level = P_ERR;
			}
			print("unable to configure SOCKS", level);
		}
	}else{
		print("SOCKS has been disabled", P_NOTE);
		incoming->connect({"", tmp_port});
	}
}

void net_proto_close(){
	// doesn't do anything, GC takes care of all data types
	// All data types should destroy any internal data
}
