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
#include "../tv/tv_frame_standard.h"
#include "../tv/tv_frame_video.h"
#include "../tv/tv_frame_audio.h"
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
	add_data(&(linked_list[0]), ID_LL_WIDTH*ID_LL_HEIGHT*sizeof(linked_list[0]));
}

void data_id_t::init_gen_id(){
	while(id == 0){
		id = true_rand(0, 0xFFFFFFFFFFFFFFFF);
	} // prevents "random" returning zero
	// Testing locally only
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
			// should make this into a tuple
			data_ptr[i] = ptr_;
			data_size[i] = size_;
			data_flags[i] = flags;
			return;
		}
	}
}

void data_id_t::add_id(uint64_t *ptr_, uint32_t size_){
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		if(id_ptr[i] == nullptr){
			id_ptr[i] = ptr_;
			id_size[i] = size_;
			return;
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

/*
  Anything being directly passed to ID_EXPORT_RAW is assumed to be inserted in
  network byte order. I don't see any exception to this, and whatever niche
  function it serves can be created with another function macro
 */
#define ID_EXPORT_RAW(var, size)					\
	for(uint64_t qqq = 0;qqq < size;qqq++){				\
		retval.push_back(NBO_8(((uint8_t*)&var)[size-qqq-1]));	\
	}								

#define ID_EXPORT(var) ID_EXPORT_RAW(var, sizeof(var))

/*
  TODO: more well-define this prefix information. std::array of pointers to the
  data and the corresponding lengths sounds pretty good.
 */

typedef uint64_t transport_i_t;
typedef uint64_t transport_size_t;

std::vector<uint8_t> data_id_t::export_data(){
	std::vector<uint8_t> retval;
	ID_EXPORT(id);
	ID_EXPORT(type);
	ID_EXPORT(pgp_cite_id);
	print("retval initial size is " + std::to_string(retval.size()), P_NOTE);
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		if(data_ptr[i] == nullptr){
			continue; // should it just break?
		}
		transport_i_t trans_i = (transport_i_t)i;
		transport_size_t trans_size = (transport_size_t)data_size[i];
		ID_EXPORT(trans_i);
		ID_EXPORT(trans_size);
		ID_EXPORT_RAW(data_ptr[i], trans_size);
		print("pushing back data entry " + std::to_string(trans_i) + " of size " + std::to_string(trans_size), P_NOTE);
	}
	P_V(retval.size(), P_NOTE);
	return retval;
}

/*
  Once the data has been imported, it either works or it doesn't, so it 
  shouldn't matter if the data has been truncated.
 */

static void id_import_data(std::vector<uint8_t> *data,
			   uint8_t *var,
			   uint64_t length){
	if(data->size() < length){
		print("not enough space in data to import into var", P_ERR);
	}
	memcpy(var,
	       data->data(),
	       length);
	// P_V(length, P_DEBUG);
	// P_V(data->size(), P_DEBUG);
	data->erase(data->begin(), data->begin()+length);
	for(uint64_t i = 0;i < length/2;i++){
		var[i] = NBO_8(var[length-i-1]);
	}
	if(length&1){
		var[(length/2)] = NBO_8(var[(length/2)]);
	}
}

// read into a local variable
#define ID_IMPORT(var) id_import_data(&data, (uint8_t*)&var, sizeof(var))
// read directly into the memory (after sanity checks)
#define ID_IMPORT_RAW(var, size) id_import_data(&data, var, size)

void data_id_t::import_data(std::vector<uint8_t> data){
	uint64_t trans_id = 0;
	std::array<uint8_t, TYPE_LENGTH> trans_type = {{0}};
	uint64_t trans_pgp_cite_id = 0;
	ID_IMPORT(trans_id);
	ID_IMPORT(trans_type);
	ID_IMPORT(trans_pgp_cite_id);
	P_V(trans_id, P_NOTE);
	P_V_S((char*)trans_type.data(), P_NOTE);
	P_V(trans_pgp_cite_id, P_NOTE);
	while(data.size() > 0){
		if(data.size() <= sizeof(transport_i_t) + sizeof(transport_size_t)){
			P_V(data.size(), P_NOTE);
			print("cannot import corrupt data, assuming dirty read", P_WARN);
		}
		transport_i_t trans_i = 0;
		transport_size_t trans_size = 0;
		ID_IMPORT(trans_i);
		ID_IMPORT(trans_size);
		P_V(trans_i, P_NOTE);
		P_V(trans_size, P_NOTE);
		const bool valid_entry =
			trans_i < ID_PTR_LENGTH;
		if(!valid_entry){
			print("invalid i entry, probably came from a new version", P_WARN);
			continue;
		}
		const bool valid_with_data =
			trans_size <= data.size();
		const bool valid_with_var =
			trans_size <= data_size[trans_i];
		if(!(valid_with_data && valid_with_var)){
			P_V(trans_i, P_NOTE);
			P_V(data_size[trans_i], P_NOTE);
			print("cannot import data with a size mis-match", P_ERR);
		}
		ID_IMPORT_RAW((uint8_t*)data_ptr[trans_i], trans_size);
	}
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
	return linked_list[(height*2)+0];
}

uint64_t data_id_t::get_next_linked_list(uint64_t height){
	return linked_list[(height*2)+1];
}

void data_id_t::set_prev_linked_list(uint64_t height, uint64_t data){
	linked_list[(height*2)+0] = data;
}

void data_id_t::set_next_linked_list(uint64_t height, uint64_t data){
	linked_list[(height*2)+1] = data;
}
