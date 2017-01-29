#include "../main.h"
#include "../util.h"
#include "net.h"
#include "net_socket.h"
#include "../stats.h"
#include "../id/id_api.h"

net_socket_t::net_socket_t() : id(this, __FUNCTION__){
	id.add_data(&status, 8);
	id.noexp_all_data();
	id.nonet_all_data();
	outbound_stat_sample_set_id = (new stat_sample_set_t)->id.get_id();
	inbound_stat_sample_set_id = (new stat_sample_set_t)->id.get_id();
}

net_socket_t::~net_socket_t(){}

/*
  net_socket_t::socket_check: throws if the socket is null. 
  This is the only private function for net_socket_t
 */

void net_socket_t::socket_check(){
	if(socket == nullptr){
		print("socket is null", P_ERR);
		/*
		  Not only because of IP and port data, but also because
		  of guaranteed anonymity.
		 */
	}
}

uint8_t net_socket_t::get_status(){
	socket_check(); // should this be here?
	return status;
}

/*
  net_socket_t::is_alive: returns the status of the socket
 */

bool net_socket_t::is_alive(){
	return socket != nullptr;
}

/*
  net_socket_t::send: sends data on the current socket.
  Throws std::runtime_error on exception
 */

void net_socket_t::send(std::vector<uint8_t> data){
	socket_check();
	uint8_t *byte_ptr = new uint8_t[data.size()];
	for(uint64_t i = 0;i < data.size();i++){
		byte_ptr[i] = data[i];
	}
	const int64_t sent_bytes = SDLNet_TCP_Send(socket, byte_ptr, data.size());
	if(sent_bytes == -1){
		print("server port mismatch", P_ERR);
	}else if(sent_bytes > 0 && sent_bytes != (int64_t)data.size()){
		print("socket has closed", P_SPAM);
		disconnect();
	}
	print("sent " + std::to_string(sent_bytes) + " bytes", P_DEBUG);
	std::pair<uint64_t, uint64_t> data_point =
		std::make_pair(
			get_time_microseconds(),
			sent_bytes);
	stat_sample_set_t *outbound_sample_set =
		PTR_DATA(outbound_stat_sample_set_id,
			 stat_sample_set_t);
	if(outbound_sample_set != nullptr){
		outbound_sample_set->add_sample(
			data_point.first,
			data_point.second);
	}
	net_proxy_t *proxy =
		PTR_DATA(proxy_id,
			 net_proxy_t);
	if(proxy != nullptr){
		stat_sample_set_t *proxy_sample_set =
			PTR_DATA(proxy->get_proxy_stat_sample_set_id(),
				 stat_sample_set_t);
		if(proxy_sample_set != nullptr){
			proxy_sample_set->add_sample(
				data_point.first,
				data_point.second);
		}
	}
}

void net_socket_t::send(std::string data){
	std::vector<uint8_t> data_(data.c_str(), data.c_str()+data.size());
	send(data_);
}

/*
  net_socket_t::recv: reads byte_count amount of data. Flags can be passed
  NET_SOCKET_RECV_NO_HANG: check for activity on the socket

  This is guaranteed to return maxlen bytes, but the SDLNet_TCP_Recv function
  doesn't share that same behavior, so store all of the information in a local
  buffer and just read from that when recv is called.
 */

static void net_socket_recv_posix_error_checking(int32_t error){
#ifdef __linux
	if(error < 0){
		switch(error){
		case -ENOTCONN:
			print("not connected to socket", P_ERR);
			break;
#if EAGAIN == EWOULDBLOCK
		case -EAGAIN:
#else		       
		case -EAGAIN:
		case -EWOULDBLOCK:
#endif
			print("something something blocking", P_ERR);
			break;
		case -EPERM: // non-blocking socket and no data is received
			break;
		default:
			P_V(error, P_SPAM);
		}
	}
#endif
}

std::vector<uint8_t> net_socket_t::recv(uint64_t byte_count, uint64_t flags){
	uint8_t tmp_data = 0;
	// TODO: test to see if the activity() code works
	do{
		uint64_t data_received = 0;
		while(activity()){
			int32_t recv_retval = 0;
			if((recv_retval = SDLNet_TCP_Recv(socket, &tmp_data, 1)) > 0){
				local_buffer.push_back(tmp_data);
				data_received++;
			}else{
				net_socket_recv_posix_error_checking(recv_retval);
			}
		}
		if(data_received != 0){
			stat_sample_set_t *inbound_stat_sample_set =
				PTR_DATA(inbound_stat_sample_set_id,
					 stat_sample_set_t);
			if(inbound_stat_sample_set != nullptr){
				inbound_stat_sample_set->add_sample(
					get_time_microseconds(),
					data_received);
			}
		}
		if(local_buffer.size() >= byte_count){
			auto start = local_buffer.begin();
			auto end = local_buffer.begin()+byte_count;
			std::vector<uint8_t> retval =
				std::vector<uint8_t>(start, end);
			local_buffer.erase(start, end);
			return retval;
		}
	}while(!(flags & NET_SOCKET_RECV_NO_HANG));
	return {};
}

std::vector<uint8_t> net_socket_t::recv_all_buffer(){
	std::vector<uint8_t> retval =
		recv(1, NET_SOCKET_RECV_NO_HANG); // runs input code
	retval.insert(
		retval.end(),
		local_buffer.begin(),
		local_buffer.end());
	local_buffer.clear();
	return retval;
}

/*
  net_socket_t::connect: connect (without SOCKS) to another client
 */

void net_socket_t::connect(){
	IPaddress tmp_ip;
	int16_t res_host_retval = 0;
	if(get_net_ip_str() == ""){
		print("opening a listening socket", P_NOTE);
		res_host_retval = SDLNet_ResolveHost(&tmp_ip,
						     nullptr,
						     get_net_port());
	}else{
		print("opening a standard socket", P_NOTE);
		res_host_retval = SDLNet_ResolveHost(&tmp_ip,
						     get_net_ip_str().c_str(),
						     get_net_port());
	}
	if(res_host_retval == -1){
		print((std::string)"cannot resolve host:"+SDL_GetError(),
		      P_ERR);
	}
	socket = SDLNet_TCP_Open(&tmp_ip);
	if(socket == nullptr){
		print((std::string)"cannot open socket (" + std::to_string(errno) + "):"+SDL_GetError(),
		      P_ERR);
	}else{
		print("opened socket", P_NOTE);
	}
}

void net_socket_t::disconnect(){
	SDLNet_TCP_Close(socket);
	socket = nullptr;
}

/*
  only used on accepting incoming connections
 */

void net_socket_t::set_tcp_socket(TCPsocket socket_){
	socket = socket_;
	IPaddress tmp_ip;
	tmp_ip = *SDLNet_TCP_GetPeerAddress(socket);
	const char *ip_addr_tmp = SDLNet_ResolveIP(&tmp_ip);
	if(ip_addr_tmp == nullptr){
		print("cannot read IP", P_ERR);
		return;
	}
	set_net_ip(ip_addr_tmp,
		   NBO_16(tmp_ip.port),
		   NET_IP_VER_4);
}

TCPsocket net_socket_t::get_tcp_socket(){
	return socket;
}

bool net_socket_t::activity(){
	if(socket == nullptr){
		print("socket is nullptr", P_WARN);
		return false;
	}
	/*
	  For some reason, socket checking functions can only run on
	  sockets inside of sets. I hope creating a temporary socket set
	  doesn't interfere with the output
	 */
	bool retval = false;
	SDLNet_SocketSet tmp_set = SDLNet_AllocSocketSet(1);
	SDLNet_TCP_AddSocket(tmp_set, socket);
	if(SDLNet_CheckSockets(tmp_set, 0) > 0){
		retval = true;
	}
	SDLNet_TCP_DelSocket(tmp_set, socket);
	SDLNet_FreeSocketSet(tmp_set);
	tmp_set = nullptr;
	return retval;
}

id_t_ net_socket_t::get_inbound_stat_sample_set_id(){
	return inbound_stat_sample_set_id;
}

id_t_ net_socket_t::get_outbound_stat_sample_set_id(){
	return outbound_stat_sample_set_id;
}

id_t_ net_socket_t::get_proxy_id(){
	return proxy_id;
}

void net_socket_t::set_proxy_id(id_t_ proxy_id_){
	if(proxy_id_ != proxy_id){
		// nothing we can do
		disconnect();
	}
	proxy_id = proxy_id_;
}
