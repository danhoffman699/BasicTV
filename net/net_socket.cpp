#include "main.h"
#include "util.h"
#include "net_socket.h"
#include "net.h"

net_socket_t::net_socket_t() : id(this, __FUNCTION__){
	id.add_data(&tor_instance_id, 8);
	id.add_id(&tor_instance_id, 1);
}

net_socket_t::~net_socket_t(){
}
