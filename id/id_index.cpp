#include "id_index.h"
#include "id.h"
#include "../util.h"

id_index_t::id_index_t() : id(this, __FUNCTION__){
	id.add_data(&(id_index[0]), ID_ARRAY_SIZE*8);
	id.add_id(&(id_index[0]), ID_ARRAY_SIZE);
}

id_index_t::~id_index_t(){
}

void id_index_t::add(uint64_t id_){
	for(uint64_t i = 0;i < ID_ARRAY_SIZE;i++){
		if(id_index[i] == 0){
			id_index[i] = id_;
			return;
		}
	}
	print("unable to add to id_index_t", P_ERR);
}

uint64_t id_index_t::get(uint64_t entry){
	return id_index.at(entry);
}

uint8_t id_index_t::get_flags(){
	return flags;
}

void id_index_t::set_as_request(){
	flags |= ID_INDEX_REQUEST;
}

void id_index_t::set_as_standard(){
	flags &= ~(ID_INDEX_REQUEST);
}
