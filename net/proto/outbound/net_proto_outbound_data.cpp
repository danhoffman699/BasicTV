#include "net_proto_outbound_data.h"
#include "../net_proto_dev_ctrl.h" // apply dev_ctrl function
#include "../net_proto_meta.h" // apply meta function

// sends all requests out in one network socket call

// sends net_request_ts out, responses are handled in inbound
void net_proto_loop_handle_outbound_requests(){
	std::vector<id_t_> net_proto_requests =
		id_api::cache::get("net_proto_request_t");
	for(uint64_t i = 0;i < net_proto_requests.size();i++){
		net_proto_request_t *proto_request =
			PTR_DATA(net_proto_requests[i],
				 net_proto_request_t);
		if(proto_request == nullptr){
			continue;
		}
		bool local = 0;
		if(proto_request->get_proto_socket_id() == 0){
			const uint64_t curr_timestamp_micro_s =
				get_time_microseconds();
			const uint64_t last_query_timestamp_micro_s =
				proto_request->get_last_query_timestamp_micro_s();
			uint64_t retry_interval_micro_s = 1000*10;
			try{
				retry_interval_micro_s =
					std::stoi(
						settings::get_setting(
							"net_proto_request_retry_interval_micro_s"));
			}catch(...){}
			if(curr_timestamp_micro_s-last_query_timestamp_micro_s > retry_interval_micro_s){
				/*
				  TODO: actually send the information out to 
				  peers

				  This isn't written now because I need to hoard
				  more stats from the inbound data before I can
				  make an informed decision on who to query
				  first.
				 */
			}
		}
	}
}
