#include "../../main.h"
#include "../../id/id_api.h"
#include "../../util.h"
#include "../net.h"
#include "../net_socket.h"
#include "net_proto_meta.h"
#include "net_proto_socket.h"

void net_proto_socket_t::set_socket_id(id_t_ socket_id_){
	socket_id = socket_id_;
	working_buffer.clear();
	if(buffer.size() != 0){
		print("valid buffers still exist, should have fetched the structs before setting new socket_id", P_WARN);
	}
	buffer.clear();
}

id_t_ net_proto_socket_t::get_socket_id(){
	return socket_id;
}

std::vector<std::vector<uint8_t> > net_proto_socket_t::get_buffer(){
	std::vector<std::vector<uint8_t> > retval;
	retval = buffer;
	buffer.clear();
	return retval;
}

// isn't used outside of this file (should be in meta though).

static std::pair<net_proto_standard_data_t, std::vector<uint8_t> >
net_proto_socket_read_struct_segment(std::vector<uint8_t> working_buffer){
	// size is the rest of data, not the size of the read data
	std::pair<net_proto_standard_data_t, std::vector<uint8_t> > retval;
	net_proto_read_packet_metadata(working_buffer.data(),
				       working_buffer.size(),
				       &retval.first);
	/*
	  I don't see a need to have a sanity check with this
	 */
	if(working_buffer.size() >= retval.first.size){
		uint8_t *start = working_buffer.data()+NET_PROTO_STANDARD_DATA_LENGTH;
		uint8_t *end = working_buffer.data()+NET_PROTO_STANDARD_DATA_LENGTH+retval.first.size;
		retval.second =
			std::vector<uint8_t>(start, end);
	}
	return retval;
}

void net_proto_socket_t::update(){
	net_socket_t *socket_ptr =
		PTR_DATA(socket_id,
			 net_socket_t);
	if(socket_ptr == nullptr){
		print("socket_ptr is a nullptr", P_ERR);
	}
	std::vector<uint8_t> net_data =
		socket_ptr->recv_all_buffer();
	working_buffer.insert(
		working_buffer.end(),
		net_data.begin(),
		net_data.end()); // should always add one byte
	std::pair<net_proto_standard_data_t, std::vector<uint8_t> > net_final;
	try{
		/*
		  TODO: If the first character is a DEV_CTRL_1, then work from
		  that. If it isn't (for some strange reason), then keep
		  deleting information until the first standalone DEV_CTRL_1 is
		  found.

		  It might be beneficial to add a "junk" byte before the
		  DEV_CTRL_1 to prove that it is a standalone, versus assuming
		  that we are at the start. The junk byte can be anything.
		  However, since TCP is very predictible, I think only malicious
		  clients will make this fall out of sync (safely)
		 */
		net_final =
			net_proto_socket_read_struct_segment(
				working_buffer);
	}catch(...){
		print("unrecognized metadata for packet", P_WARN);
		return;
	}
	if(net_final.second.size() > 0){
		// net_final.second size is the raw size from the socket, so it
		// includes all extra DEV_CTRL_1s, so it can be used directly to
		// clean up working_buffer
		const uint64_t actual_size =
			net_final.second.size()+NET_PROTO_STANDARD_DATA_LENGTH+1;
		// final 1 is for the first DEV_CTRL_1
		buffer.push_back(net_final.second);
		working_buffer.erase(
			working_buffer.begin(),
			working_buffer.begin()+actual_size);
		// fixes the double DEV_CTRL_1
		for(uint64_t i = 0;i < net_final.second.size()-1;i++){
			if(net_final.second[i] == NET_PROTO_DEV_CTRL_1){
				if(net_final.second[i+1] == NET_PROTO_DEV_CTRL_1){
					net_final.second.erase(net_final.second.begin()+i);
					for(;i < net_final.second.size();i++){
						if(net_final.second[i] != NET_PROTO_DEV_CTRL_1){
							break;
						}
					}
				}else{
					print("it seems like the size and the next packet metadata overlap, this shouldn't happen", P_ERR);
				}
			}
		}
	} // doesn't trip when the data isn't finished receiving
}
