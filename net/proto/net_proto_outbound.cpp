#include "net_proto_outbound.h"
#include "net_proto_dev_ctrl.h" // apply dev_ctrl function
#include "net_proto_meta.h" // apply meta function

void net_proto_loop_handle_outbound_requests(){
	std::vector<uint64_t> all_requests =
		id_api::cache::get("net_request_t");
	for(uint64_t i = 0;i < all_requests.size();i++){
		net_request_t *request =
			PTR_DATA(all_requests[i],
				 net_request_t);
		if(request == nullptr){
			print("request is a nullptr", P_WARN);
			continue;
		}
		net_socket_t *socket =
			PTR_DATA(request->get_socket_id(),
				 net_socket_t);
		if(socket == nullptr){
			print("socket is a nullptr", P_WARN);
			continue;
		}
		// if this isn't an outbound request
		if(request->is_local() == false){
			std::vector<uint8_t> all_export = {{0}};
			for(uint64_t i = 0;i < NET_REQUEST_MAX_LENGTH;i++){
				data_id_t *tmp_id =
					id_api::array::ptr_id(
						request->get_id(i),
						"");
				if(tmp_id == nullptr){
					// tmp_id is probably zero
					print("unable to facilitate request", P_WARN);
					continue;
				}
				std::vector<uint8_t> data =
					tmp_id->export_data(); // compresses
				all_export.insert(
					all_export.end(),
					data.begin(),
					data.end());
			}
			/*
			  networking code might want to be refactored to handle
			  such large buffers, but it needs to be that way for
			  other reasons. Once the networking code can handle 
			  arbitrarially large chunks of data, it would make
			  sense to do it that way.
			 */
			all_export =
				net_proto_apply_dev_ctrl(
					all_export);
			std::vector<uint8_t> metadata =
				net_proto_write_packet_metadata(
					all_export.size(),
					VERSION_MAJOR,
					VERSION_MINOR,
					VERSION_REVISION,
					0,
					0); // TODO: make version optional
			all_export.insert(all_export.begin(),
					  metadata.begin(),
					  metadata.end());
			socket->send(all_export);
		} // other ones need to be taken care of in net_proto_import
	}
}
