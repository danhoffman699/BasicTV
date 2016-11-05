#include "id.h"
#include "id_index.h"
#include "id_api.h"
#include "../main.h"
#include "../util.h"
#include "../lock.h"
#include "../convert.h"

// just for all of the data types
#include "../net/net.h"
#include "../net/net_socket.h"
#include "../net/net_proto.h"
#include "../tv/tv.h"
#include "../tv/tv_frame.h"
#include "../tv/tv_patch.h"
#include "../tv/tv_channel.h"
#include "../tv/tv_window.h"

/*
  I'm not too concerned with threading if the entire idea
  of the program is for it to be runnable on a Raspberry Pi
  (single core, not the Two or Three or whatever)
 */

// when performance starts to be a problem, look into quicksorting or
// moving it over to an std::vector

void data_id_t::init_list_all_data(){
	add_data(&id, sizeof(id));
	add_id(&id, 1);
	add_data(&(linked_list[0]), ID_LL_WIDTH*ID_LL_HEIGHT);
}

void data_id_t::init_gen_id(){
	while(id == 0){
		id = true_rand(0, 0xFFFFFFFFFFFFFFFF);
	} // prevents "random" returning zero
}


data_id_t::data_id_t(void *ptr_, std::string type_){
	// store all data in contiguous containers, avoid
	// std::string and std::vector. Use std::array for
	// raw data, strings, and lists of items. Also forces
	// a defined maximum to everything
	type = convert::array::type::to(type_);
	ptr = ptr_;
	init_list_all_data();
	init_gen_id();
	id_api::array::add(id, this);
	id_api::cache::add(id, type);
}

data_id_t::~data_id_t(){
	id_api::array::del(id);
	id_api::cache::del(id, type);
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
	// shouldn't run at all, at least not now
	exit(0);
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

uint64_t data_id_t::get_prev_linked_list(uint64_t height){
	P_V(linked_list[(height*2)+0], P_SPAM);
	return linked_list[(height*2)+0];
}

uint64_t data_id_t::get_next_linked_list(uint64_t height){
	P_V(linked_list[(height*2)+1], P_SPAM);
	return linked_list[(height*2)+1];
}

void data_id_t::set_prev_linked_list(uint64_t height, uint64_t data){
	linked_list[(height*2)+0] = data;
}

void data_id_t::set_next_linked_list(uint64_t height, uint64_t data){
	linked_list[(height*2)+1] = data;
}
