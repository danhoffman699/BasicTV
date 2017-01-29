#include "net_proto_inbound_data.h"
// meta has apply and unapply functions for DEV_CTRL_1
#include "../net_proto_meta.h"
#include "../net_proto_socket.h"
#include "../net_proto_request.h"
#include "../../../util.h"
#include "../../../id/id_api.h"

// static std::vector<uint8_t> net_proto_read_struct_segment(uint8_t *data,
// 							  uint64_t data_size){
// 	// size is the rest of data, not the size of the read data
// 	net_proto_standard_size_t size = 0;
// 	net_proto_standard_ver_t ver_major = 0;
// 	net_proto_standard_ver_t ver_minor = 0;
// 	net_proto_standard_ver_t ver_patch = 0;
// 	net_proto_standard_macros_t macros = 0;
// 	net_proto_standard_unused_t unused = 0;
// 	net_proto_read_packet_metadata(data,
// 				       data_size,
// 				       &size,
// 				       &ver_major,
// 				       &ver_minor,
// 				       &ver_patch,
// 				       &macros,
// 				       &unused);
// 	if(unused != 0){
// 		print("received a packet that utilized currently unused space, you might be running an old version", P_WARN);
// 	}
// 	if(data_size < size){
// 		// not enough data has been read yet
// 		return {};
// 	}
// 	return std::vector<uint8_t>(data, data+size);
// }

// /*
//   Reads all readable data from packet buffer, and returns a vector of the
//   RAW strings from the socket (needs to be iterated over with the unapply
//   function)

//   This code hasn't been tested at all, and should be cleaned up a lot before
//   actual use takes place

//   All of the struct segments should be ran through a decoder to remove the
//   extra DEV_CTRL_1 entries. The extra characters that are written should be
//   taken into account when computing the payload length.

//   TODO: offset the work of importing to the net_socket_t. Assuming all
//   sockets created follow this rule of only sending this sort of information,
//   that would make everything nicer (I can't think of any other case right now)
//  */

// #define EFFECTIVE_LENGTH() (buffer.size()-(i+1)-NET_PROTO_META_LENGTH)

// static std::vector<std::vector<uint8_t> > net_proto_get_struct_segments(net_socket_t *socket){
// 	std::vector<std::vector<uint8_t> > retval;
// 	std::vector<uint8_t> buffer =
// 		socket->recv(
// 			-socket->get_backwards_buffer_size(),
// 			NET_SOCKET_RECV_NO_HANG);
// 	uint64_t i = 0;
// 	// we don't know how nested it is
// 	for(;i < buffer.size();i++){
// 		if(buffer[i] != NET_PROTO_DEV_CTRL_1){
// 			break;
// 		}
// 	}
// 	for(;i < buffer.size();i++){
// 		try{
// 			if(buffer.at(i-1) != NET_PROTO_DEV_CTRL_1 &&
// 			   buffer.at(i+0) == NET_PROTO_DEV_CTRL_1 &&
// 			   buffer.at(i+1) != NET_PROTO_DEV_CTRL_1){
// 				net_proto_standard_size_t payload_size = 0;
// 				net_proto_read_packet_metadata(&(buffer[i]),
// 							       buffer.size()-i,
// 							       &payload_size,
// 							       nullptr,
// 							       nullptr,
// 							       nullptr,
// 							       nullptr,
// 							       nullptr);
// 				if(EFFECTIVE_LENGTH() < payload_size){
// 					/*
// 					  Unless the read buffer becomes large
// 					  enough, I need to precisely measure
// 					  how much data should be read and only
// 					  read that much to prevent chopping off
// 					  additional metadata
// 					 */
// 					std::vector<uint8_t> new_data =
// 						socket->recv(
// 							payload_size-EFFECTIVE_LENGTH(),
// 							NET_SOCKET_RECV_NO_HANG);
// 					buffer.insert(
// 						buffer.end(),
// 						new_data.begin(),
// 						new_data.end());
// 				}
// 				if(payload_size <= EFFECTIVE_LENGTH()){
// 					retval.push_back(
// 						net_proto_read_struct_segment(
// 							&(buffer[i+1]),
// 							payload_size));
// 				}
// 				i += payload_size;
// 			}
// 		}catch(...){}
// 	}
// 	return retval;
// }

// /*
//   Fetches all incoming data and handle it
//  */

// void net_proto_loop_handle_inbound_data(){
// 	net_socket_t *tmp_socket = nullptr;
// 	std::vector<uint64_t> peer_id_list =
// 		id_api::cache::get("net_peer_t");
// 	for(uint64_t i = 0;i < peer_id_list.size();i++){
// 		net_peer_t *peer =
// 			PTR_DATA(peer_id_list[i], net_peer_t);
// 		for(uint64_t s = 0;s < NET_PROTO_MAX_SOCKET_PER_PEER;s++){
// 			tmp_socket =
// 				PTR_DATA(peer->get_socket_id(s),
// 					 net_socket_t);
// 			if(likely(tmp_socket == nullptr)){
// 				continue;
// 			}
// 			/*
// 			  Ideally, store all of this on a global std::vector
// 			  and create worker threads to parse through it with
// 			  locks.
// 			 */
// 			std::vector<std::vector<uint8_t> > segments =
// 				net_proto_get_struct_segments(tmp_socket);
// 			for(uint64_t seg = 0;seg < segments.size();seg++){
// 				id_api::array::add_data(
// 					net_proto_unapply_dev_ctrl(
// 						std::vector<uint8_t>(
// 							segments[seg].begin()+NET_PROTO_META_LENGTH,
// 							segments[seg].end()
// 							)
// 						)
// 					);
// 			}
// 		}
// 	}
// }


// // reads from all net_socket_t, for testing only
// void net_proto_loop_dummy_read(){
// 	std::vector<uint64_t> all_sockets =
// 		id_api::cache::get("net_socket_t");
// 	for(uint64_t i = 0;i < all_sockets.size();i++){
// 		net_socket_t *socket_ =
// 			PTR_DATA(all_sockets[i], net_socket_t);
// 		if(socket_ == nullptr){
// 			print("socket is nullptr", P_ERR);
// 			continue;
// 		}
// 		if(socket_->get_client_conn().first == ""){
// 			// inbound
// 			continue;
// 		}
// 		if(socket_->activity()){
// 			print("detected activity on a socket", P_SPAM);
// 			std::vector<uint8_t> incoming_data;
// 			while((incoming_data = socket_->recv(1, NET_SOCKET_RECV_NO_HANG)).size() != 0){
// 				P_V_E((uint64_t)incoming_data[0], P_NOTE);
// 			}
// 		}
// 	}
// }

// // reads what we have, returns proper status depending on it

// void net_proto_loop_handle_inbound_requests(){
	
// }

/*
  TODO: actually delete net_request_t after the data came to prevent
  redundant queries
 */

static void net_proto_loop_dummy_read(){
}

static void net_proto_process_buffer_vector(net_proto_socket_t *proto_socket){
	std::vector<std::vector<uint8_t> > buffer_vector =
		proto_socket->get_buffer();
	for(uint64_t i = 0;i < buffer_vector.size();i++){
		try{
			id_t_ imported_data_id =
				id_api::array::add_data(buffer_vector[i]);
			proto_socket->add_id_to_log(imported_data_id);
			data_id_t *imported_data_ptr =
				PTR_ID(imported_data_id,
				       net_proto_request_t);
			if(imported_data_ptr != nullptr){
				net_proto_request_t *request =
					(net_proto_request_t*)imported_data_ptr->get_ptr();
				request->set_proto_socket_id(
					proto_socket->id.get_id());
			}
		}catch(...){
			// unable to parse that file
		}
	}
}

void net_proto_loop_handle_inbound_data(){
	std::vector<id_t_> proto_sockets =
		id_api::cache::get("net_proto_socket_t");
	for(uint64_t i = 0;i < proto_sockets.size();i++){
		net_proto_socket_t *proto_socket =
			PTR_DATA(proto_sockets[i],
				 net_proto_socket_t);
		if(proto_socket == nullptr){
			print("proto_socket is a nullptr", P_ERR);
		}
		proto_socket->update();
		net_proto_process_buffer_vector(
			proto_socket);
	}
}

/*
  malicious_to_send is technically redundant for security purposes, since
  any information with any security needs would have ID_DATA_NONET.
  However, this speeds things up, allows for a blacklist on peers who attempt
  this, and might prevent exporting one useless variable to eat up bandwidth.
  Also, security redundancy is pretty important.
 */

// for security reasons
static std::array<std::string, 2> malicious_to_send =
{"encrypt_priv_key_t",
 "net_proxy_t"};

// for DoS/DDoS reasons
static std::array<std::string, 3> malicious_to_bulk_send =
{"tv_frame_video_t",
 "tv_frame_audio_t",
 "tv_frame_caption_t"};

// net_con_req_t are in a different file
// receives net_proto_request_t, sends data out
void net_proto_loop_handle_inbound_requests(){
	std::vector<id_t_> net_proto_requests =
		id_api::cache::get("net_proto_request_t");
	for(uint64_t i = 0;i < net_proto_requests.size();i++){
		net_proto_request_t *request =
			PTR_DATA(net_proto_requests[i],
				 net_proto_request_t);
		if(request == nullptr){
			continue;
		}
		if(request->get_proto_socket_id() == ID_BLANK_ID){
			continue;
		}
		const std::string type =
			convert::array::type::from(
				request->get_type());
		const bool malicious_to_send_ =
			std::find(
				malicious_to_send.begin(),
				malicious_to_send.end(),
				type);
		if(malicious_to_send_){
			print("malicious to send period, not servicing", P_ERR);
		}
		const bool malicious_to_bulk_send_ =
			std::find(
				malicious_to_bulk_send.begin(),
				malicious_to_bulk_send.end(),
				type);
		const bool bulk_send =
			(convert::array::type::from(request->get_type()) != "") &&
			!(request->get_flags() & NET_REQUEST_BLACKLIST);
		if(bulk_send && malicious_to_bulk_send_){
			print("detected malicious activity on a bulk send, not servicing", P_ERR);
		}
		// TODO: actually create a proper response to this (?)
		net_proto_socket_t *curr_proto_socket =
			PTR_DATA(request->get_proto_socket_id(),
				 net_proto_socket_t);
		if(curr_proto_socket == nullptr){
			print("can't service to an invalid socket", P_NOTE);
		}
		std::vector<id_t_> serve_vector =
			request->get_ids();
		std::vector<uint64_t> mod_vector =
			request->get_mod();
		// getters assert they are at least equal
		for(uint64_t i = 0;i < serve_vector.size();i++){
			data_id_t *id_tmp =
				PTR_ID(serve_vector[i], );
			if(id_tmp == nullptr){
				print("can't service an ID I don't have", P_NOTE);
			}
			if(id_tmp->get_mod_inc() >= mod_vector[i]){
				curr_proto_socket->send_id(serve_vector[i]);
			}else{
				// can be spammed
				/*
				  I CAN'T QUERY A NODE BECAUSE OF THIS
				 */
				print("ID I have is out of date?", P_NOTE);
			}
		}
	}
}
