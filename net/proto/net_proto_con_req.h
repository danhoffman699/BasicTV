#ifndef NET_PROTO_CON_REQ_H
#define NET_PROTO_CON_REQ_H
#include "../net_peer.h"
/*
  net_con_req_t: connection request

  Since TCP hole punching requires rather precise syncronization, this data type
  is the home of that information. The entire network sends out all of the 
  SYN packets for TCP holepunching at the same time (coordinated with the UNIX
  timestamp). Since most OSes get their time from an NTP server, the variation
  between clocks is on par with the variation with ping times, so it should work
  fine.

  Since establishing connections is supposed to be rather low latency, this data
  type in particular has a high priority.

  net_con_req_t is also for instructing a node to connect to a node with an open
  TCP port, since the node wanting the data can't inititate the connection.

  Since this is such a common data type, and is used extensively for basic
  connections, nodes delete them when one of the two occurs:
  1. The heartbeat timestamp for the connection initiating is over the limit
  set in the settings (probably around 5 minutes)
  2. The heartbeat timestamp has already happened

  net_con_req_t also works with UDP, but UDP support in general isn't a high on
  the priorities list.
*/

// first two bits are for the protocol

#define NET_CON_REQ_TCP NET_PEER_TCP
#define NET_CON_REQ_UDP NET_PEER_UDP

/*
  First peer (sender) wants to connect to the second peer (receiver). Second
  peer doesn't have a TCP port open, but the first does. 

  Since this doesn't require hole punching at all, heartbeat_timestamp is set
  to the current time (falls in line with deletion rules). 

  When the second peer receives this, it connects to the first peer like it
  would with any other node (sends information over normally). There is no
  special timing needed.

  The name comes from a reverse initiation of a forwarded port connection

  This doesn't translate over to UDP, since both ports for UDP have to be
  opened for anything meaningful to happen (to my knowledge, without using
  holepunching), and that case would be covered by just directly talking.
*/
#define NET_CON_REQ_REVERSE_FORWARD 0

/*
  First peer (sender) wants to connect to the second peer (receiver). Second
  peer doesn't have a port open, nor does the first peer.

  Assuming the connection request protocol is TCP:
  heartbeat_timestamp is set to the UNIX time at which the SYN packets will be
  sent by the first towards the second and vice-versa. Both sides ought to know
  pretty quickly if it had worked (from what they received). If it didn't work
  (within the timeout time, probably three seconds), try it again. Keep this up
  until the default timeout is reached (probably five tries). 
  
  If the connection is UDP the third peer is the server with proper UDP ports
  forwarded that can handle the nitty gritty technicals with UDP hole punching.
  However, I don't care about UDP that much, so that's on the bacnburner.
 */
#define NET_CON_REQ_HOLEPUNCH 1

/*
  This can be referred to net_con_req_t in personal flowcharts and docs, but it
  is formally defined as net_proto_con_req_t
*/

struct net_proto_con_req_t{
private:
	// ideally, flags are internal only
	uint8_t flags = 0;
	id_t_ first_peer_id = 0;
	id_t_ second_peer_id = 0;
	id_t_ third_peer_id = 0;
	uint64_t heartbeat_timestamp = 0;
public:
	data_id_t id;
	void set_ids(id_t_ first_peer_id_
		     id_t_ second_peer_id_
		     id_t_ third_peer_id_);
	void get_ids(id_t_ *first_peer_id_,
		     id_t_ *second_peer_id_,
		     id_t_ *third_peer_id_);
	void set_proto(uint8_t proto);
	uint8_t get_proto();
	void set_con_type(uint8_t con_type);
	uint8_t get_con_type();
	void set_heartbeat_timestamp(uint64_t heartbeat_timestamp_);
	uint64_t get_heartbeat_timestamp();
};

#endif
