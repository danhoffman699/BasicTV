#ifndef STATS_H
#define STATS_H
#include "vector"
#include "tuple"
#include "id/id.h"
/*
  stats.h a simple statistics library
 */

/*
  TODO: expand this to get some general stats information. I need to pass a 
  vector of the data, and doing statistics on the IDs themselves is useless
  (perhaps specify an data_vector entry?)
 */

// virtual class, standard doesn't mean a standard curve
struct stat_sample_set_standard_t{
protected:
	uint64_t max_samples = 0;
public:
	void list_virtual_data(data_id_t *id);
	void set_max_samples(uint64_t max_samples_);
	uint64_t get_max_samples();
	
};

/*
  stat_sample_set_t: a data set that plots two 64-bit integers.
  Currently only used for bandwidth monitoring, but should be the only needed
  data set (non-ID)
 */

struct stat_sample_set_t : virtual stat_sample_set_standard_t{
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

/*
  stat_sample_set_id_t: a data set that plots a 64-bit and IDs
  Currently only used by the net_proto_socket_t struct to get statistical odds
  that a peer has wanted information.
*/

struct stat_sample_set_id_t{
private:
	std::vector<std::pair<uint64_t, id_t_> > sample_vector;
	
public:
};

#endif
