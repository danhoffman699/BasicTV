#ifndef NET_IP_H
#define NET_IP_H
/*
  net_ip_t: virtual overlay for any type needing an IP

  Polymorphism is used as a general interface among programs, where
  it is critical to the usage, while not needing
 */

#define NET_IP_VER_4 (0)
#define NET_IP_VER_6 (1)

// TODO: make this more legit

#define SET_NET_IP_VER(fl, fo) fl = fo
#define GET_NET_IP_VER(fl) (fl)

struct net_ip_t{
protected:
	std::array<uint8_t, 16> ip = {};
	uint16_t port = 0;
	uint8_t status = 0;
public:
	net_ip_t();
	~net_ip_t();
	void list_virtual_data(data_id_t *id);
	void set_net_ip(std::string ip_,
			uint16_t port_,
			uint8_t status_);
	std::array<uint8_t, 16> get_net_ip();
	std::string get_net_ip_str();
	uint16_t get_net_port();
	uint8_t get_net_status();
};
#endif
