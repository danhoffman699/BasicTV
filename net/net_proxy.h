#ifndef NET_PROXY_H
#define NET_PROXY_H

#include "net_ip.h"
#include "../id/id.h"

/*
  net_proxy_t: proxy information

  This is just the metadata for a proxy, and does not tie itself to a socket,
  but a socket ties itself to this.

  TODO: actually use proxy_stat_sample_set_id
 */

struct net_proxy_t : public net_ip_t{
private:
	uint8_t flags = 0;
	id_t_ proxy_stat_sample_set_id = ID_BLANK_ID;
public:
	data_id_t id;
	net_proxy_t();
	~net_proxy_t();
	void set_flags(uint8_t flags_);
	uint8_t get_flags();
	id_t_ get_proxy_stat_sample_set_id();
};
#endif
