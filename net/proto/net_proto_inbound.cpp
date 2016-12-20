#include "net_proto_inbound.h"
// meta has apply and unapply functions for DEV_CTRL_1
#include "net_proto_meta.h"

static std::vector<uint8_t> net_proto_read_struct_segment(uint8_t *data,
							  uint64_t data_size){
	// size is the rest of data, not the size of the read data
	net_proto_standard_size_t size = 0;
	net_proto_standard_ver_t ver_major = 0;
	net_proto_standard_ver_t ver_minor = 0;
	net_proto_standard_ver_t ver_patch = 0;
	net_proto_standard_macros_t macros = 0;
	net_proto_standard_unused_t unused = 0;
	net_proto_read_packet_metadata(data,
				       data_size,
				       &size,
				       &ver_major,
				       &ver_minor,
				       &ver_patch,
				       &macros,
				       &unused);
	if(unused != 0){
		print("received a packet that utilized currently unused space, you might be running an old version", P_WARN);
	}
	if(data_size < size){
		// not enough data has been read yet
		return {};
	}
	return std::vector<uint8_t>(data, data+size);
}

/*
  Reads all readable data from packet buffer, and returns a vector of the
  RAW strings from the socket (needs to be iterated over with the unapply
  function)

  This code hasn't been tested at all, and should be cleaned up a lot before
  actual use takes place

  All of the struct segments should be ran through a decoder to remove the
  extra DEV_CTRL_1 entries. The extra characters that are written should be
  taken into account when computing the payload length.
 */

#define EFFECTIVE_LENGTH() (buffer.size()-(i+1)-NET_PROTO_META_LENGTH)

static std::vector<std::vector<uint8_t> > net_proto_get_struct_segments(net_socket_t *socket){
	std::vector<std::vector<uint8_t> > retval;
	std::vector<uint8_t> buffer =
		socket->recv(
			-socket->get_backwards_buffer_size(),
			NET_SOCKET_RECV_NO_HANG);
	uint64_t i = 0;
	// we don't know how nested it is
	for(;i < buffer.size();i++){
		if(buffer[i] != NET_PROTO_DEV_CTRL_1){
			break;
		}
	}
	for(;i < buffer.size();i++){
		try{
			if(buffer.at(i-1) != NET_PROTO_DEV_CTRL_1 &&
			   buffer.at(i+0) == NET_PROTO_DEV_CTRL_1 &&
			   buffer.at(i+1) != NET_PROTO_DEV_CTRL_1){
				net_proto_standard_size_t payload_size = 0;
				net_proto_read_packet_metadata(&(buffer[i]),
							       buffer.size()-i,
							       &payload_size,
							       nullptr,
							       nullptr,
							       nullptr,
							       nullptr,
							       nullptr);
				if(EFFECTIVE_LENGTH() < payload_size){
					/*
					  Unless the read buffer becomes large
					  enough, I need to precisely measure
					  how much data should be read and only
					  read that much to prevent chopping off
					  additional metadata
					 */
					std::vector<uint8_t> new_data =
						socket->recv(
							payload_size-EFFECTIVE_LENGTH(),
							NET_SOCKET_RECV_NO_HANG);
					buffer.insert(
						buffer.end(),
						new_data.begin(),
						new_data.end());
				}
				if(payload_size <= EFFECTIVE_LENGTH()){
					retval.push_back(
						net_proto_read_struct_segment(
							&(buffer[i+1]),
							payload_size));
				}
				i += payload_size;
			}
		}catch(...){}
	}
	return retval;
}

/*
  Fetches all incoming data and handle it
 */

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
			/*
			  Ideally, store all of this on a global std::vector
			  and create worker threads to parse through it with
			  locks.
			 */
			std::vector<std::vector<uint8_t> > segments =
				net_proto_get_struct_segments(tmp_socket);
			for(uint64_t seg = 0;seg < segments.size();seg++){
				id_api::array::add_data(
					net_proto_unapply_dev_ctrl(
						std::vector<uint8_t>(
							segments[seg].begin()+NET_PROTO_META_LENGTH,
							segments[seg].end()
							)
						)
					);
			}
		}
	}
}

/*
  Accepts connections from a given socket. There is no net_socket_t interface
  for accepting connections (but it wouldn't be a bad idea), so just grab the
  TCPsocket type from the variable and use that.
 */

void net_proto_loop_accept_conn(net_socket_t *incoming_socket){
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
void net_proto_loop_dummy_read(){
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

