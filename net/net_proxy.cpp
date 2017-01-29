#include "net_proxy.h"
#include "../stats.h"

net_proxy_t::net_proxy_t() : id(this, __FUNCTION__){
	proxy_stat_sample_set_id =
		(new stat_sample_set_t)->id.get_id();
}

net_proxy_t::~net_proxy_t(){
}

void net_proxy_t::set_flags(uint8_t flags_){
	flags = flags_;
}

uint8_t net_proxy_t::get_flags(){
	return flags;
}

id_t_ net_proxy_t::get_proxy_stat_sample_set_id(){
	return proxy_stat_sample_set_id;
}
