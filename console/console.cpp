#include "console.h"

console_t::console_t() : id(this, __FUNCTION__){
}

console_t::~console_t(){
}

void console_t::set_socket_id(id_t_ socket_id_){
	socket_id = socket_id_;
}

void console_t::execute(std::vector<std::string> cmd_vector){
	LIST_CMD(
}

void console_t::run(){
	net_socket_t *socket =
		PTR_DATA(socket_id,
			 net_socket_t);
	if(socket == nullptr){
		print("no valid socket", P_ERR);
	}
	std::vector<uint8_t> inbound_data =
		socket->recv_all_buffer();
	working_input.insert(
		working_input.end(),
		inbound_data.begin(),
		inbound_data.end());
	uint64_t pos = 0;
	while((pos = std::find(std::begin(working_input),
			       std::end(working_input),
			       '\r')) != std::end(working_input)){
		const std::string input_full(
			std::begin(working_input),
			pos);
		std::vector<std::string> cmd_vector;
		for(uint64_t i = 0;i < input_full.size();i++){
			if(input_full[i] == ' '){
				cmd_vector.push_back(
					std::string(
						input_full.begin(),
						i));
				input_full.erase(
					input_full.begin(),
					i);
				i = 0;
			}
			P_V_S(cmd_vector[cmd_vector.size()-1], P_SPAM)
		}
		execute(cmd_vector);
	}
}
