#include "../main.h"
#include "../util.h"
#include "net_socket.h"
#include "net.h"

net_socket_t::net_socket_t() : id(this, __FUNCTION__){
	id.add_data(&status, 8);
	// no real need to add anything else, should never
	// be networked
}

net_socket_t::~net_socket_t(){
	print("THIS SHOULDN'T BE HAPPENING", P_CRIT);
}

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

/*
  net_socket_t::get_status: returns the status variable. There
  are no individual getters, and direct setters don't exist. All
  flags that can be passed are preceded with NET_SOCKET
 */

uint64_t net_socket_t::get_status(){
	socket_check(); // should this be here?
	return status;
}

/*
  net_socket_t::is_alive: returns the status of the socket
 */

bool net_socket_t::is_alive(){
	if(socket == nullptr){
		return false;
	}
	return true;
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

static void net_socket_append_to_buffer(std::array<uint8_t, NET_SOCKET_OLD_BUFFER_SIZE> *buffer,
					std::vector<uint8_t> data){
	if(data.size() >= NET_SOCKET_OLD_BUFFER_SIZE){
	 	memcpy(buffer,
	 	       data.data(),
	 	       NET_SOCKET_OLD_BUFFER_SIZE);
	}else{
		for(uint64_t i = data.size();i < NET_SOCKET_OLD_BUFFER_SIZE;i++){
			(*buffer)[i-data.size()] = (*buffer)[i];
		}
		for(uint64_t i = NET_SOCKET_OLD_BUFFER_SIZE-data.size();i < NET_SOCKET_OLD_BUFFER_SIZE;i++){
			(*buffer)[i] = data[NET_SOCKET_OLD_BUFFER_SIZE-data.size()-i];
		}
		// TODO: implement this in memcpy, networking code is probably
		// the most important code for efficiency
	}
}

std::vector<uint8_t> net_socket_t::recv(int64_t byte_count, uint64_t flags){
	socket_check();
	const bool hang = !(flags & NET_SOCKET_RECV_NO_HANG);
	uint8_t tmp_data = 0;
	int32_t recv_retval = 0;
	do{
		while(activity()){
			recv_retval =
				SDLNet_TCP_Recv(socket, &tmp_data, 1);
			if(recv_retval == -1){
				net_socket_recv_posix_error_checking(recv_retval);
				continue;
			}
			local_buffer.push_back(tmp_data);
		}
	}while((local_buffer.size() < byte_count) && hang);
	std::vector<uint8_t> retval;
	if(byte_count > 0){
		if(local_buffer.size() >= byte_count){
			auto start = local_buffer.begin();
			auto end = local_buffer.begin()+byte_count;
			retval = std::vector<uint8_t>(start, end);
			net_socket_append_to_buffer(&old_buffer, retval);
			local_buffer.erase(start, end);
		}
	}else if(byte_count < 0){
		if(std::abs(byte_count) <= buffer_written_memory){
			retval =
				std::vector<uint8_t>(
					&old_buffer[NET_SOCKET_OLD_BUFFER_SIZE+byte_count-1],
					&old_buffer[NET_SOCKET_OLD_BUFFER_SIZE-1]);
		}
	}
	return retval;
}

/*
  net_socket_t::enable_socks: enables SOCKS proxy (Tor). Isn't responsible
  for enabling or disabling anything Tor related, or to do anything specific
  to the Tor network. However, external functions (should) exist that faciliate
  new circuits, port assignment, and other interfacing requirements

  TODO: possibly investigate SOCKS4a for domain name resolution and SOCKS5 for 
  whatever new thing that does (but it looks pretty complex)
 */

void net_socket_t::enable_socks(std::pair<std::string, uint16_t> socks_info,
				std::pair<std::string, uint16_t> client_info){
	client_conn = client_info;
	socks_conn = socks_info;
	std::array<uint8_t, 13> socks_request = {{0}};
	uint32_t socks_user_id = true_rand(0, std::numeric_limits<uint32_t>::max());	
	IPaddress client_addr, socks_addr;
	SDLNet_ResolveHost(&client_addr, client_info.first.c_str(),
			   client_info.second);
	SDLNet_ResolveHost(&socks_addr, socks_info.first.c_str(),
			   socks_info.second);
	socket = SDLNet_TCP_Open(&socks_addr);
	memcpy(&(socks_user_id_str), &socks_user_id, 4);
	socks_user_id_str[4] = 0; // has to be null terminated
	socks_request[0] = 0x04; // SOCKSv4
	if(client_conn.first != ""){
		socks_request[1] = 0x01; // establish a TCP/IP stream connection
		memcpy(&(socks_request[4]), &client_addr.host, 4);
	}else{
		socks_request[1] = 0x02; // establish a TCP/IP port binding 
	}
	memcpy(&(socks_request[2]), &client_addr.port, 2);
	memcpy(&(socks_request[8]), &(socks_user_id_str[0]), 4);
	socks_request[12] = 0;
	std::vector<uint8_t> request;
	for(uint64_t i = 0;i < 13;i++){
		request.push_back(socks_request[i]);
	}
	send(request);
	switch(recv(8).at(1)){
	case 0x5A:
		print("request granted for SOCKS", P_NOTE);
		status |= NET_SOCKET_USE_SOCKS;
		break;
	case 0x5B:
		print("request rejected or failed for SOCKS", P_ERR);
		break;
	case 0x5C:
		print("not running identd for SOCKS", P_ERR);
		break;
	case 0x5D:
		print("identd can't confirm the user ID string", P_ERR);
		break;
	default:
		print("unknown staus byte for SOCKS", P_ERR);
		break;
	}
}

void net_socket_t::disable_socks(){
	/*
	  Can probably get away with closing the connection
	 */
}

/*
  net_socket_t::connect: connect (without SOCKS) to another client
 */

void net_socket_t::connect(std::pair<std::string, uint16_t> conn_info){
	P_V_S(conn_info.first, P_DEBUG);
	P_V(conn_info.second, P_DEBUG);
	client_conn = conn_info;
	IPaddress tmp_ip;
	int16_t res_host_retval = 0;
	// 0.0.0.0 might be a better option instead of a blank string
	if(client_conn.first == ""){
		print("opening a listening socket", P_NOTE);
		res_host_retval = SDLNet_ResolveHost(&tmp_ip,
						     nullptr,
						     client_conn.second);
	}else{
		print("opening a standard socket", P_NOTE);
		res_host_retval = SDLNet_ResolveHost(&tmp_ip,
						     client_conn.first.c_str(),
						     client_conn.second);
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
	client_conn.first = ip_addr_tmp;
	client_conn.second = tmp_ip.port;
}

TCPsocket net_socket_t::get_tcp_socket(){
	return socket;
}

std::pair<std::string, uint16_t> net_socket_t::get_client_conn(){
	return client_conn;
}

std::pair<std::string, uint16_t> net_socket_t::get_socks_conn(){
	return socks_conn;
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
