#ifndef NET_PROTO_INBOUND_H
#define NET_PROTO_INBOUND_H
extern void net_proto_loop_accept_conn(net_socket_t *incoming_socket);
extern void net_proto_loop_dummy_read();
extern void net_proto_loop_handle_inbound_requests();
#endif
