#ifndef NET_H
#define NET_H
#include "string"
#include "vector"
#include "curl/curl.h"
namespace net{
	std::string get_cache(std::string url, int stale_time);
	void force_url(std::string url);
	std::string get_url(std::string url, int stale_time = 30);
	std::string get(std::string url, int stale_time = 30);
};
struct net_cache_t{
private:
	std::string url;
	std::string data;
	uint64_t timestamp;
	bool complete;
public:
	net_cache_t(std::string url_, std::string data_, uint64_t timestamp_);
	~net_cache_t();
	std::string get_url();
	std::string get_data();
	void set_data(std::string);
	uint64_t get_timestamp();
	void set_complete(bool);
	bool get_complete();
};
#endif
