#include "stats.h"

stat_sample_set_t::stat_sample_set_t() : id(this, __FUNCTION__){
}

stat_sample_set_t::~stat_sample_set_t(){
}

std::vector<std::pair<uint64_t, uint64_t> > stat_sample_set_t::get_samples(){
	return sample_vector;
}

void stat_sample_set_t::set_samples(std::vector<std::pair<uint64_t, uint64_t> > sample_vector_){
	sample_vector = sample_vector_;
}

void stat_sample_set_t::add_sample(uint64_t x, uint64_t y){
	sample_vector.push_back(std::make_pair(x, y));
}

