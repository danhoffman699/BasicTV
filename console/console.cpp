#include "console.h"

console_t::console_t() : id(this, __FUNCTION__){
}

console_t::~console_t(){
}

void console_t::set_socket_id(id_t_ socket_id_){
	socket_id = socket_id_;
}

void console_t::execute(std::vector<std::string> cmd_vector){
	LIST_CMD(reg_set_const);
	LIST_CMD(reg_set_table);
	LIST_CMD(reg_copy);
	LIST_CMD(reg_clear);
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
	auto pos = working_input.end();
	while((pos = std::find(working_input.begin(),
			       working_input.end(),
			       (uint8_t)'\r')) != working_input.end()){
		std::string input_full(working_input.begin(),
				       pos);
		std::vector<std::string> cmd_vector;
		for(uint64_t i = 0;i < input_full.size();i++){
			if(input_full[i] == ' '){
				cmd_vector.push_back(
					std::string(
						&(input_full[0]),
						&(input_full[i-1])));
				input_full.erase(
					input_full.begin(),
					input_full.begin()+i);
				i = 0;
			}
			P_V_S(cmd_vector[cmd_vector.size()-1], P_SPAM);
		}
		execute(cmd_vector);
	}
}

/*
  Manages sockets
 */

static id_t_ console_socket_id = ID_BLANK_ID;

void console_init(){
	net_socket_t *inbound_socket =
		new net_socket_t;
	inbound_socket->set_net_ip("", 59000, NET_IP_VER_4);
	inbound_socket->connect();
	console_socket_id =
		inbound_socket->id.get_id();
}

static void console_accept_connections(){
	net_socket_t *socket =
		PTR_DATA(console_socket_id,
			 net_socket_t);
	if(socket == nullptr){
		print("socket is a nullptr", P_ERR);
	}
	TCPsocket tmp_socket =
		socket->get_tcp_socket();
	TCPsocket new_socket =
		nullptr;
	if((new_socket = SDLNet_TCP_Accept(tmp_socket)) != nullptr){
		print("accepting a new console connection", P_NOTE);
		console_t *console_new =
			new console_t;
		net_socket_t *console_socket =
			new net_socket_t;
		console_socket->set_tcp_socket(new_socket);
		console_socket->send("BasicTV console\n");
		console_new->set_socket_id(console_socket->id.get_id());
	}
}

void console_loop(){
	console_accept_connections();
	std::vector<id_t_> console_entries =
		id_api::cache::get(
			"console_t");
	for(uint64_t i = 0;i < console_entries.size();i++){
		console_t *console_tmp =
			PTR_DATA(console_entries[i],
				 console_t);
		if(console_tmp == nullptr){
			continue;
		}
		console_tmp->run();
	}
}

void console_close(){
}
