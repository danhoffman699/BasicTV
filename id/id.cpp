#include "id.h"
#include "id_api.h"
#include "../main.h"
#include "../util.h"
#include "../lock.h"
#include "../convert.h"

// just for all of the data types
#include "../net/net.h"
#include "../net/net_socket.h"
#include "../net/proto/net_proto.h"
#include "../tv/tv.h"
#include "../tv/tv_frame_standard.h"
#include "../tv/tv_frame_video.h"
#include "../tv/tv_frame_audio.h"
#include "../tv/tv_channel.h"
#include "../tv/tv_window.h"

/*
  Because of encryption and compression overheads, making this multithreadable
  isn't a bad idea. However, implementing locks shouldn't be hard later on.
 */

// when performance starts to be a problem, look into quicksorting or
// moving it over to an std::vector

void data_id_t::init_list_all_data(){
	add_data(&id,
		 sizeof(id_t_),
		 ID_DATA_ID);
	add_data(&linked_list.first,
		 sizeof(id_t_),
		 ID_DATA_ID);
	add_data(&linked_list.second,
		 sizeof(id_t_),
		 ID_DATA_ID);
	
}

void data_id_t::init_gen_id(){
	while(id == 0){
		id = true_rand(1, 0xFFFFFFFFFFFFFFFF);
	} // doesn't hurt, true_rand might still be retarded
}


data_id_t::data_id_t(void *ptr_, std::string type_){
	type = convert::array::type::to(type_);
	ptr = ptr_;
	init_list_all_data();
	init_gen_id();
	id_api::array::add(this);
	id_api::cache::add(id, type);
}

data_id_t::~data_id_t(){
	id_api::array::del(id);
	id_api::cache::del(id, type);
}

uint64_t data_id_t::get_id(){
	return id;
}

std::string data_id_t::get_type(){
	return convert::array::type::from(type);
}

/*
  Even though function can pass information to each other through IDs,
  get_id is used in too many searches to be a healthy benchmark
 */

void *data_id_t::get_ptr(){
	last_access_timestamp_micro_s =
		get_time_microseconds();
	return ptr;
}

void data_id_t::add_data(void *ptr_, uint32_t size_, uint64_t flags){
	data_vector.push_back(
		data_id_ptr_t(
			ptr_,
			size_,
			flags));
}

// std::vector<uint8_t> is the only vector that is networkable

void data_id_t::add_data(std::vector<uint8_t> *ptr_, uint32_t size_, uint64_t flags){
	data_vector.push_back(
		data_id_ptr_t(
			ptr_,
			size_,
			flags | ID_DATA_BYTE_VECTOR));
}

void data_id_t::add_data(std::vector<id_t_> *ptr_, uint32_t size_, uint64_t flags){
	data_vector.push_back(
		data_id_ptr_t(
			ptr_,
			size_,
			flags | ID_DATA_ID_VECTOR));
}

uint64_t data_id_t::get_rsa_cite_id(){
	return rsa_cite_id;
}

uint64_t data_id_t::get_data_index_size(){
	return data_vector.size();
}

/*
  Only currently used for exporting rules, so there is no need for any fine
  grain getters
 */

std::vector<uint8_t> data_id_t::get_ptr_flags(){
	std::vector<uint8_t> retval;
	for(uint64_t i = 0;i < data_vector.size();i++){
		retval.push_back(
			data_vector[i].get_flags());
	}
	return retval;
}

/*
  Anything being directly passed to ID_EXPORT_RAW is assumed to be inserted in
  network byte order. I don't see any exception to this, and whatever niche
  function it serves can be created with another function macro
 */

static void id_export_raw(uint8_t *var, uint64_t size, std::vector<uint8_t> *vector){
	std::vector<uint8_t> tmp(var, var+size);
	tmp = convert::nbo::to(tmp);
	vector->insert(vector->end(), tmp.begin(), tmp.end());
}

#define ID_EXPORT(var) id_export_raw((uint8_t*)&var, sizeof(var), &retval)

typedef uint16_t transport_i_t;
typedef uint32_t transport_size_t;

std::vector<uint8_t> data_id_t::export_data(uint8_t flags_){
	// TODO: enforce flags
	std::vector<uint8_t> retval;
	bool valid = false;
	for(uint64_t i = 3;i < data_vector.size();i++){
		if(!(data_vector[i].get_flags() & ID_DATA_NOEXPORT)){
			valid = true;
			break;
		}
	}
	if(valid == false){
		return {};
	}
	if(is_owner()){
		ID_EXPORT(id);
		ID_EXPORT(type);
		transport_i_t trans_i = 0;
		transport_size_t trans_size = 0;
		for(uint64_t i = 0;i < data_vector.size();i++){
			const uint8_t data_vector_flags =
				(data_vector[i].get_flags() & ID_DATA_NOEXP) |
				(data_vector[i].get_flags() & ID_DATA_NONET);
			if(data_vector_flags != flags_){
				print("skipping unexportable data_vector entry", P_NOTE);
			}
			trans_i = i;
			trans_size = data_vector[i].get_length();
			ID_EXPORT(trans_i);
			ID_EXPORT(trans_size);
			void *ptr_to_export = data_vector[i].get_ptr();
			if(data_vector[i].get_flags() & ID_DATA_BYTE_VECTOR){
				ptr_to_export =
					((std::vector<uint8_t>*)ptr_to_export)->data();
			}
			id_export_raw((uint8_t*)data_vector[i].get_ptr(), trans_size, &retval);
		}
		P_V(retval.size(), P_NOTE);
		/*
		  DATA COMPRESSION SHOULD GO RIGHT HERE
		*/
	}else{
		/*
		  can't compress already encrypted data
		*/
		retval = imported_data;
	}
	return retval;
}

static void id_import_raw(uint8_t* var, uint8_t flags, uint64_t size, std::vector<uint8_t> *vector){
	if(flags & ID_DATA_BYTE_VECTOR){
		std::vector<uint8_t> *vector_tmp =
			(std::vector<uint8_t>*)var;
		// not the fastest
		vector_tmp->erase(vector_tmp->begin(),
				 vector_tmp->end());
		vector_tmp->insert(vector_tmp->end(),
				  size-vector_tmp->size(),
				  0);
		var = vector_tmp->data();
	}
	memcpy(var, vector->data(), size);
	vector->erase(vector->begin(), vector->begin()+size);
	convert::nbo::from((uint8_t*)var, size);
}

/*
  a call to just ID_IMPORT means the data is guaranteed to not be std::vector,
  since it is a part of the protocol (and hopefully std::vector never makes it
  that far).
*/
#define ID_IMPORT(var) id_import_raw((uint8_t*)&var, 0, sizeof(var), &data)

void data_id_t::import_data(std::vector<uint8_t> data){
	id_t_ trans_id = 0;
	std::array<uint8_t, TYPE_LENGTH> trans_type = {{0}};
	ID_IMPORT(trans_id);
	ID_IMPORT(trans_type);
	transport_i_t trans_i = 0;
	transport_size_t trans_size = 0;
	while(data.size() > sizeof(transport_i_t) + sizeof(transport_size_t)){
		// pretty much guaranteed to work
		ID_IMPORT(trans_i);
		ID_IMPORT(trans_size);
		const bool valid_entry =
			trans_i < data_vector.size();
		if(unlikely(!valid_entry)){
			print("invalid i entry, probably came from a new version", P_WARN);
			return;
		}else if(unlikely(data_vector[trans_i].get_ptr() == nullptr)){
			print("cannot write to nullptr entry", P_WARN);
			return;
		}
		if(unlikely(trans_size > data.size())){
			print("fetched size is greater than working data", P_WARN);
			return;
		}else if(unlikely(trans_size > data_vector[trans_i].get_length())){
			print("fetched size is greater than the local version", P_WARN);
			return;
		}
		id_import_raw((uint8_t*)data_vector[trans_i].get_ptr(),
			      data_vector[trans_i].get_flags(),
			      trans_size,
			      &data);
	}
	imported_data = data;
}

void data_id_t::rsa_decrypt_backlog(){
	// The whole strucute isn't guaranteed to come over, and TCP
	// gives us lossless connections, so apply them from oldest to
	// newest
	for(uint64_t i = 0;i < rsa_backlog.size();i++){
		import_data(rsa_backlog[i]);
	}
	rsa_backlog.clear();
}

id_t_ data_id_t::get_prev_linked_list(){
	return linked_list.first;
}

id_t_ data_id_t::get_next_linked_list(){
	return linked_list.second;
}

void data_id_t::set_prev_linked_list(id_t_ data){
	linked_list.first = data;
}

void data_id_t::set_next_linked_list(id_t_ data){
	linked_list.second = data;
}

/*
  THIS IS OBVIOUSLY NOT CORRECT, however it should work for tests. I need
  to get a legit public-private key system down before I can declare
  ownership (I can just list private key structus and compare them, but
  that itself would need more definition).
 */

bool data_id_t::is_owner(){
	return true;
}

void data_id_t::noexport_all_data(){
	for(uint64_t i = 0;i < data_vector.size();i++){
		data_vector[i].set_flags(
			data_vector[i].get_flags() | ID_DATA_NOEXPORT);
	}
}

void data_id_t::nonet_all_data(){
	for(uint64_t i = 0;i < data_vector.size();i++){
		data_vector[i].set_flags(
			data_vector[i].get_flags() | ID_DATA_NONET);
	}
}

data_id_ptr_t::data_id_ptr_t(void *ptr_,
			     uint32_t length_,
			     uint8_t flags_){
	ptr = ptr_;
	length = length_;
	flags = flags_;
}

data_id_ptr_t::~data_id_ptr_t(){
}

void *data_id_ptr_t::get_ptr(){
	return ptr;
}

uint32_t data_id_ptr_t::get_length(){
	return length;
}

uint8_t data_id_ptr_t::get_flags(){
	return flags;
}
