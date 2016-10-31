#include "id.h"
#include "id_index.h"
#include "../main.h"
#include "../util.h"
#include "../lock.h"
#include "../net/net.h"
#include "../net/net_socket.h"
#include "../net/net_proto.h"
#include "../tv/tv.h"
#include "../tv/tv_frame.h"
#include "../tv/tv_patch.h"
#include "../tv/tv_channel.h"
#include "../tv/tv_window.h"
#include "../convert.h"

/*
  I'm not too concerned with threading if the entire idea
  of the program is for it to be runnable on a Raspberry Pi
  (single core, not the Two or Three or whatever)
 */

// when performance starts to be a problem, look into quicksorting or
// moving it over to an std::vector
static data_id_t **id_list = nullptr;

std::array<std::pair<std::array<uint8_t, TYPE_LENGTH>, std::vector<uint64_t> >, STD_ARRAY_SIZE> type_vector;

void data_id_t::init_list_all_data(){
	add_data(&id, sizeof(id));
	add_id(&id, 1);
	add_data(&(linked_list[0]), ID_LL_WIDTH*ID_LL_HEIGHT);
}

void data_id_t::init_gen_list_id(){
	while(id == 0){
		id = true_rand(0, 0xFFFFFFFFFFFFFFFF);
	} // prevents "random" returning zero
	if(id_list == nullptr){
		id_list = new data_id_t*[ID_ARRAY_SIZE];
		memset(id_list, 0, sizeof(data_id_t*)*ID_ARRAY_SIZE);
	}
	for(uint64_t i = 0;i < ID_ARRAY_SIZE;i++){
		if(id_list[i] == nullptr){
			id_list[i] = this;
		}
	}
}

void data_id_t::init_type_cache(){
	bool type_cached = false;
	for(uint64_t i = 0;i < STD_ARRAY_LENGTH;i++){
		if(type_vector[i].first == type){
			type_vector[i].second.push_back(id);
			return;
		}
	}
	// first of type created
	for(uint64_t i = 0;i < STD_ARRAY_LENGTH;i++){
		if(type_vector[i].second.size() == 0){
			type_vector[i].first = type;
			type_vector[i].second.push_back(id);
			type_cached = true;
			break;
		}
	}
	/*
	  The type should never be changed, and there is no code implemented
	  or planned to change that, so this can't be an attack vector
	 */
	print("unable to add type to cache, 64K?", P_ERR);
}

data_id_t::data_id_t(void *ptr_, std::string type_){
	// store all data in contiguous containers, avoid
	// std::string and std::vector. Use std::array for
	// raw data, strings, and lists of items. Also forces
	// a defined maximum to everything
	type = convert::array::type::to(type_);
	ptr = ptr_;
	init_gen_list_id();
	init_list_all_data();
	init_type_cache();
}

data_id_t::~data_id_t(){
	uint64_t i = 0;
	for(;i < STD_ARRAY_LENGTH;i++){
		if(type_vector[i].first == type){
			break;
		}
	}
	for(uint64_t c = 0;c < STD_ARRAY_LENGTH;c++){
		if(type_vector[i].second[c] == id){
			type_vector[i].second.erase(
				type_vector[i].second.begin()+c);
		}
	}
}
void data_id_t::dereference_id(uint64_t id_){
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		if(id_ptr[i] != nullptr){
			for(uint64_t c = 0;c < id_size[i];c++){
				uint64_t *id_tmp = id_ptr[i]+c;
				if(*id_tmp == id_){
					*id_tmp = 0;
				}
			}
		}
	}
}
	
void data_id_t::set_id(uint64_t id_){
	id = id_;
}

uint64_t data_id_t::get_id(){
	return id;
}

std::string data_id_t::get_type(){
	return convert::array::type::from(type);
}

void *data_id_t::get_ptr(){
	return ptr;
}

void data_id_t::add_data(void *ptr_, uint32_t size_, uint64_t flags){
	/*
	  All data in this array is networked, and until I see a need for
	  non-network-able variables to be extracted, then ID_DATA_CACHE
	  will be not registered
	 */
	if(flags & ID_DATA_CACHE){
		return;
	}
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		if(data_ptr[i] == NULL){
			data_ptr[i] = ptr_;
			data_size[i] = size_;
			data_flags[i] = flags;
			return;
		}
	}
}

void data_id_t::add_id(uint64_t *ptr_, uint32_t size_){
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		if(id_ptr[i] == NULL){
			id_ptr[i] = ptr_;
			id_size[i] = size_;
			// no flags needed, since no networking happens
		}
	}
}

uint64_t data_id_t::get_pgp_cite_id(){
	return pgp_cite_id;
}

uint64_t data_id_t::get_data_index_size(){
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		if(data_ptr[i] == NULL){
			return i+1;
		}
	}
	return 0;
}

/*static std::vector<uint8_t> pull_data(std::vector<uint8_t> array,
				      uint8_t *array_to,
				      uint64_t len){
	std::vector<uint8_t> retval;
	for(uint64_t i = 0;i < len;i++){
		retval.push_back(array[i]);
	}
	return retval;
}*/

/*
  Pulled from id.h

  ID is the first 8 bytes
  Next TYPE_LENGTH (32) bytes are the name of the data type, padded
  with zeroes (artificially low to force short types).

  NOTE: TYPE_LENGTH shouldn't change

  The rest is formatted in the following blocks
  2 byte for the entry in the data_ptr, should be enough
  1 byte for the flags given in the initializer (encrypted)
  4 bytes for the size of the string to be parsed
  The raw string is here

  TODO: implement transparent encryption somehow
 */

std::vector<uint8_t> data_id_t::export_data(){
	std::vector<uint8_t> retval;
	const uint64_t nbo_id = NBO_64(id);
	const std::vector<uint8_t> nbo_type =
		convert::nbo::to(
			std::vector<uint8_t>(type.begin(), type.end()));
	const uint64_t nbo_pgp_cite_id = NBO_64(pgp_cite_id);
	retval.insert(retval.end(), ((uint8_t*)&nbo_id), ((uint8_t*)&nbo_id)+8);
	retval.insert(retval.end(), nbo_type.begin(), nbo_type.end());
	retval.insert(retval.end(), ((uint8_t*)nbo_pgp_cite_id), ((uint8_t*)nbo_pgp_cite_id)+8);
	for(uint16_t i = 0;i != 0xFFFF;i++){
		if(data_ptr[i] == nullptr){
			continue;
			// should I just exit here?
		}
		const uint16_t nbo_i = NBO_16(i);
		retval.insert(retval.end(), ((uint8_t*)&nbo_i), ((uint8_t*)&nbo_i)+2);
		/*
		  the entire array is guaranteed to be blanked if any part
		  of the entry is used, so read from the back to the front
		  and set the data size to distance from the beginning
		  to the first non-zero, so long arrays don't have to be 
		  networked with redundant zeroes without sacrificing any
		  data loss
		 */
		uint8_t *byte_array = (uint8_t*)data_ptr[i];
		uint32_t data_entry_size = 0;
		for(;data_entry_size < data_size[i];data_entry_size++){
			if(byte_array[data_entry_size] != 0){
				data_entry_size++;
				break;
			}
		}
		const uint32_t nbo_data_entry_size = NBO_32(data_entry_size);
		retval.insert(
			retval.end(),
			((uint8_t*)&nbo_data_entry_size),
			((uint8_t*)&nbo_data_entry_size)+4);
		std::vector<uint8_t> nbo_data_ptr =
			convert::nbo::to(
				std::vector<uint8_t>(
					(uint8_t*)&data_ptr[i],
					((uint8_t*)&data_ptr[i])+data_entry_size));
		retval.insert(
			retval.end(),
			nbo_data_ptr.begin(),
			nbo_data_ptr.end());
			}
	return retval;
}

void data_id_t::import_data(std::vector<uint8_t> data){
	/*
	  TODO: Finish and test export for security flaws and the like before I
	  sink time into maintaining and building two converse functions
	 */
	throw std::runtime_error("import_data hasn't been made yet");
}

void data_id_t::pgp_decrypt_backlog(){
	// The whole strucute isn't guaranteed to come over, and TCP
	// gives us lossless connections, so apply them from oldest to
	// newest
	for(uint64_t i = 0;i < pgp_backlog.size();i++){
		import_data(pgp_backlog[i]);
	}
	pgp_backlog.clear();
}

static data_id_t *id_std_find(uint64_t id){
	for(uint64_t i = 0;i < ID_ARRAY_SIZE;i++){
		if(id_list[i] == nullptr){
			continue;
		}
		if(id == id_list[i]->get_id()){
			return id_list[i];
		}
	}
	return nullptr;
}

data_id_t *id_array::ptr_id(uint64_t id){
	data_id_t *retval = id_std_find(id);
	if(retval == nullptr){
		throw std::runtime_error("ptr_id: retval == nullptr");
	}
	return retval;
}

void *id_array::ptr_data(uint64_t id){
	data_id_t *id_ptr = ptr_id(id);
	// currently impossible
	if(unlikely(id_ptr->get_ptr() == nullptr)){
		throw std::runtime_error("ptr_data: id_ptr->ptr == nullptr");
	}
	return id_ptr->get_ptr();
}

bool id_array::exists(uint64_t id){
	return id_array::ptr_id(id) != nullptr;
}

bool id_array::exists_in_list(uint64_t id,
			      std::array<uint64_t, ID_PTR_LENGTH> catalog){
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		if(catalog[i] == id){
			return true;
		}
	}
	return false;
}

/*
  TODO: crunch some numbers and check if comparing 16-bit hashes
  would make any major difference
 */

std::vector<uint64_t> id_array::all_of_type(std::string type){
	std::array<uint8_t, TYPE_LENGTH> type_tmp = convert::array::type::to(type);
	for(uint64_t i = 0;i < STD_ARRAY_SIZE;i++){
		if(type_vector[i].first == type_tmp){
			return type_vector[i].second;
		}
	}
	return {};
}

#define CHECK_TYPE(a) if(convert::array::type::from(type) == #a){(new a)->id.import_data(data_);return;}

void id_array::add_data(std::vector<uint8_t> data_){
	uint64_t id = 0;
	std::array<uint8_t, TYPE_LENGTH> type;
	type.fill(0);
	/*
	  TODO: complete this
	 */
	std::vector<uint64_t> tmp_type_vector = all_of_type(convert::array::type::from(type));
	for(uint64_t i = 0;i < tmp_type_vector.size();i++){
		if(tmp_type_vector[i] == id){
			ptr_id(tmp_type_vector[i])->import_data(data_);
			return;
		}
	}
	CHECK_TYPE(tv_channel_t);
	CHECK_TYPE(tv_frame_t);
	CHECK_TYPE(pgp_cite_t);
	CHECK_TYPE(net_peer_t);
	print("type isn't valid", P_ERR);
}

#undef CHECK_TYPE

std::vector<uint64_t> id_array::sort_by_pgp_pubkey(std::vector<uint64_t> tmp){
	bool changed = true;
	while(changed){
		changed = false;
		for(uint64_t i = 1;i < tmp.size()-1;i++){
			data_id_t *tmp_array[2] = {nullptr};
			tmp_array[0] = id_array::ptr_id(tmp[i-1]);
			tmp_array[1] = id_array::ptr_id(tmp[i]);
			const bool pgp_greater_than =
				pgp::cmp::greater_than(tmp_array[i-1]->get_pgp_cite_id(),
						       tmp_array[i]->get_pgp_cite_id());
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
