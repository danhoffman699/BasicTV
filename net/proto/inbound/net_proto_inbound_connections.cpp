#include "net_proto_inbound.h"
#include "net_proto_inbound_connections.h"

/*
  Actually model this after the flowchart I made detailing the protocol
*/

void net_proto_loop_accept_all_connections(net_socket_t *incoming_socket){
	TCPsocket tmp_tcp_socket = SDLNet_TCP_Accept(incoming_socket->get_tcp_socket());
	if(tmp_tcp_socket != nullptr){
		// actually do something
	}
}
