#ifndef STATS_H
#define STATS_H
#include "vector"
#include "tuple"
#include "id/id.h"
/*
  stats.h a simple statistics library
 */

/*
  stat_sample_set_t: set of samples

  Only currently used for bandwidth measuring in net_socket_t. Has to use 
  uint64_t as a data collection type. Only a certain number of entries are
  kept, after that start removing the older ones first.
 */

struct stat_sample_set_t{
private:
	std::vector<std::pair<uint64_t, uint64_t> > sample_vector;
public:
	data_id_t id;
	stat_sample_set_t();
	~stat_sample_set_t();
	std::vector<std::pair<uint64_t, uint64_t> > get_samples();
	void set_samples(std::vector<std::pair<uint64_t, uint64_t> > sample_vector_);
	void add_sample(uint64_t x, uint64_t y);
};

#endif
