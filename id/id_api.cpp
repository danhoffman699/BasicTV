#include "id.h"
#include "id_api.h"

#include "../tv/tv_dev_audio.h"
#include "../tv/tv_dev_video.h"
#include "../tv/tv_frame_audio.h"
#include "../tv/tv_frame_video.h"
#include "../net/net_proto.h"

static data_id_t **id_list = nullptr;
static std::vector<std::pair<std::vector<uint64_t>, std::array<uint8_t, TYPE_LENGTH> > > type_cache;

static void check_and_allocate_list(){
	if(unlikely(id_list == nullptr)){
		id_list = new data_id_t*[ID_ARRAY_SIZE];
		memset(id_list, 0, ID_ARRAY_SIZE*sizeof(data_id_t*));
	}
}

static data_id_t *id_find(uint64_t id){
	check_and_allocate_list();
	for(uint64_t i = 0;i < ID_ARRAY_SIZE;i++){
		if(id_list[i] == nullptr){
			continue;
		}
		if(id_list[i]->get_id() == id){
			return id_list[i];
		}
	}
	print("Couldn't find ID", P_WARN);
	return nullptr;
}

data_id_t *id_api::array::ptr_id(uint64_t id,
				 std::string type){
	if(id == 0){
		return nullptr;
	}
	data_id_t *retval = id_find(id);
	if(retval == nullptr){
		return nullptr;
	}
	// blank type is a wildcard, currently onlu used for PGP sorting
	// "" is used for direct calls
	if(retval->get_type() != type && type != ""){
		P_V_S(retval->get_type(), P_ERR);
		P_V_S(type, P_ERR);
		return nullptr;
	}
	return retval;
}

data_id_t *id_api::array::ptr_id(uint64_t id,
				 std::array<uint8_t, TYPE_LENGTH> type){
	return ptr_id(id, convert::array::type::from(type));
}

void *id_api::array::ptr_data(uint64_t id,
			      std::string type){
	data_id_t *id_ptr = ptr_id(id, type);
	if(id_ptr == nullptr){
		return nullptr;
	}
	return id_ptr->get_ptr();
}

void *id_api::array::ptr_data(uint64_t id,
			      std::array<uint8_t, TYPE_LENGTH> type){
	return ptr_data(id, convert::array::type::from(type));
}

void id_api::array::add(uint64_t id, data_id_t *ptr){
	check_and_allocate_list();
	for(uint64_t i = 0;i < ID_ARRAY_SIZE;i++){
		if(id_list[i] == nullptr){
			id_list[i] = ptr;
			return;
		}
	}
}

void id_api::array::del(uint64_t id){
	check_and_allocate_list();
	print("THIS ISNT GOOD AT ALL", P_CRIT);
	for(uint64_t i = 0;i < ID_ARRAY_SIZE;i++){
		if(id_list[i] == nullptr){
			continue;
		}
		if(id_list[i]->get_id() == id){
			id_list[i] = nullptr;
			return;
		}
	}
	print("cannot find ID in list", P_ERR);
}

#define CHECK_TYPE(a) if(convert::array::type::from(type) == #a){(new a)->id.import_data(data_);return;}

// interprets network data and populates it

void id_api::array::add_data(std::vector<uint8_t> data_){
	uint64_t id = 0;
	std::array<uint8_t, TYPE_LENGTH> type;
	type.fill(0);
	/*
	  TODO: complete this
	 */
	std::vector<uint64_t> tmp_type_cache =
		id_api::cache::get(type);
	for(uint64_t i = 0;i < tmp_type_cache.size();i++){
		if(tmp_type_cache[i] == id){
			ptr_id(tmp_type_cache[i], "")->import_data(data_);
			return;
		}
	}
	CHECK_TYPE(tv_channel_t);
	CHECK_TYPE(tv_frame_audio_t);
	CHECK_TYPE(tv_frame_video_t);
	CHECK_TYPE(net_peer_t);
	print("type isn't valid", P_ERR);
}

#undef CHECK_TYPE

std::vector<uint64_t> id_api::array::sort_by_pgp_pubkey(std::vector<uint64_t> tmp){
	bool changed = true;
	while(changed){
		changed = false;
		for(uint64_t i = 1;i < tmp.size()-1;i++){
			data_id_t *tmp_array[2] = {nullptr};
			tmp_array[0] = PTR_ID(tmp[i-1], "");
			tmp_array[1] = PTR_ID(tmp[i], "");
			//const bool pgp_greater_than =
			//	pgp::cmp::greater_than(tmp_array[i-1]->get_pgp_cite_id(),
			//			       tmp_array[i]->get_pgp_cite_id());
			const bool pgp_greater_than = false;
			if(pgp_greater_than){
				uint64_t tmp_ = tmp[i-1];
				tmp[i-1] = tmp[i];
				tmp[i] = tmp_;
				changed = true;
				break; // ?
			}
		}
	}
	return tmp;
}


// Type cache code

static std::vector<uint64_t> *get_type_cache_ptr(std::array<uint8_t, TYPE_LENGTH> tmp){
	for(uint64_t i = 0;i < type_cache.size();i++){
		if(type_cache[i].second == tmp){
			return &type_cache[i].first;
		}
	}
	type_cache.push_back(std::make_pair(std::vector<uint64_t>({}), tmp));
	return &type_cache[type_cache.size()-1].first;
}

void id_api::cache::add(uint64_t id, std::array<uint8_t, TYPE_LENGTH> type){
	std::vector<uint64_t> *vector =
		get_type_cache_ptr(type);
	vector->push_back(id);
	vector = nullptr;
}

void id_api::cache::add(uint64_t id, std::string type){
	add(id, convert::array::type::to(type));
}

void id_api::cache::del(uint64_t id, std::array<uint8_t, TYPE_LENGTH> type){
	std::vector<uint64_t> *vector =
		get_type_cache_ptr(type);
	for(uint64_t i = 0;i < vector->size();i++){
		if((*vector)[i] == id){
			vector->erase(vector->begin()+i);
			vector = nullptr;
			return;
		}
	}
	vector = nullptr;
	print("couldn't find ID in type cache", P_ERR);
}

void id_api::cache::del(uint64_t id, std::string type){
	del(id, convert::array::type::to(type));
}

std::vector<uint64_t> id_api::cache::get(std::array<uint8_t, TYPE_LENGTH> type){
	return *get_type_cache_ptr(type);
}

std::vector<uint64_t> id_api::cache::get(std::string type){
	return get(convert::array::type::to(type));
}

// TODO: make a forwards and backwards function too
// redefine this in linked_list
std::vector<uint64_t> id_api::array::get_forward_linked_list(uint64_t id,
							     uint64_t height){
	std::vector<uint64_t> retval;
	while(id != 0){
		data_id_t *id_ptr = PTR_ID(id, );
		retval.push_back(id);
		id = id_ptr->get_next_linked_list(height);
	}
	return retval;
}

void id_api::linked_list::link_vector(std::vector<uint64_t> vector,
				      uint16_t position){
	PTR_ID(vector[0], )->set_next_linked_list(position, vector[1]);
	for(uint64_t i = 1;i < vector.size()-1;i++){
		data_id_t *id = PTR_ID(vector[i], );
		id->set_next_linked_list(position, vector[i+1]);
		id->set_prev_linked_list(position, vector[i-1]);
	}
	PTR_ID(vector[vector.size()-1], )->set_prev_linked_list(position,
								vector[vector.size()-2]);
}

std::vector<uint64_t> id_api::get_all(){
	std::vector<uint64_t> retval;
	for(uint64_t i = 0;i < ID_ARRAY_SIZE;i++){
		if(id_list[i] != nullptr){
			retval.push_back(id_list[i]->get_id());
		}
	}
	return retval;
}

#define DELETE_TYPE(a) if(ptr->get_type() == #a){delete (a*)ptr->get_ptr();continue;}

// refactor

void id_api::destroy_all_data(){
	check_and_allocate_list();
	for(uint64_t i = 0;i < ID_ARRAY_SIZE;i++){
		if(id_list[i] == nullptr){
			continue;
		}
		try{
			data_id_t *ptr = id_list[i];
			DELETE_TYPE(tv_frame_video_t);
			DELETE_TYPE(tv_frame_audio_t);
			DELETE_TYPE(tv_channel_t);
			DELETE_TYPE(tv_window_t);
			DELETE_TYPE(tv_dev_video_t);
			DELETE_TYPE(tv_dev_audio_t);
			DELETE_TYPE(net_socket_t);
			print("unknown type:" + ptr->get_type(), P_WARN);
		}catch(...){}
	}
	delete[] id_list;
}
