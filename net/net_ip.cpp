#include <SDL2/SDL_net.h>
#include "net_ip.h"
#include "../convert.h"
#include "../util.h"

net_ip_t::net_ip_t(){
}

net_ip_t::~net_ip_t(){
}

void net_ip_t::list_virtual_data(data_id_t *id){
	id->add_data(&(ip[0]), 16);
	id->add_data(&port, 2);
	id->add_data(&status, 1);
}

// TODO: make this IPv6 able

void net_ip_t::set_net_ip(std::string ip_, uint16_t port_, uint8_t status_){
	if(ip_ != ""){
		IPaddress ip_addr_tmp;
		SDLNet_ResolveHost(&ip_addr_tmp, ip_.c_str(), port_);
		ip_addr_tmp.host = NBO_32(ip_addr_tmp.host);
		memcpy(&(ip[0]), &ip_addr_tmp.host, 4);
	}else{
		memset(&(ip[0]), 0, 4);
	}
	port = port_;
	status = status_;
}

// all data retrieved from getters is in local byte order

std::array<uint8_t, 16> net_ip_t::get_net_ip(){
	return ip;
}

std::string net_ip_t::get_net_ip_str(){
	IPaddress ip_addr_tmp;
	ip_addr_tmp.host = NBO_32(*((uint32_t*)&ip[0]));
	ip_addr_tmp.port = NBO_16(get_net_port());
	std::string retval = SDLNet_ResolveIP(&ip_addr_tmp);
	if(retval == "0.0.0.0"){
		retval = ""; // legacy
	}else{
		print("registering " + retval + " as a legit IP", P_SPAM);
	}
	return retval;
}

uint16_t net_ip_t::get_net_port(){
	return port;
}

uint8_t net_ip_t::get_net_status(){
	return status;
}
