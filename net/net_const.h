/*
  net_const.h: all macros for net are defined here for simplicity and 
  to decrease interdependency on files
 */
#ifndef NET_CONST_H
#define NET_CONST_H
#define NET_SOCKET_USE_SOCKS (1 << 0)
#define NET_PEER_LIMIT 65536
#define NET_IP_MAX_SIZE 46
#define NET_SOCKET_RECV_NO_HANG (1 << 0)
#endif
