#include "id.h"
#include "main.h"
#include "util.h"
#include "lock.h"

static std::vector<data_id_t*> id_list_vector;
static lock_t id_list_vector_lock;

/*
  Split the ID into two parts for easy searching, since that is 
  probably the singe most-used function outside of PRINT()
 */

std::default_random_engine generator;

data_id_t::data_id_t(void *ptr_, std::string type_){
	uint64_t part_one = 0;
	while(part_one == 0){
		std::uniform_int_distribution<uint32_t>
			distribution(0, std::numeric_limits<uint32_t>::max());
		part_one = (uint64_t)distribution(generator);
	}
	uint64_t part_two = 0;
	auto part_two_function = [&](){
					 if(id_list_vector.size() == 0){
						 return;
					 }
					 part_two =
						 id_list_vector
						 [id_list_vector.size()-1]->
						 get_id() & 0xFFFFFFFF00000000;
					 part_two >>= 32;
					 part_two++;
					 part_two <<= 32;
				 };
	LOCK_RUN(id_list_vector_lock, part_two_function());
	id = part_one | part_two;
	type = type_;
	ptr = ptr_;
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		data_ptr[i] = nullptr;
		data_size[i] = 0;
		id_ptr[i] = nullptr;
		id_size[i] = 0;
	}
	auto function = [&](){
				id_list_vector.push_back(this);
			};
	LOCK_RUN(id_list_vector_lock, function());
}

data_id_t::~data_id_t(){
}

void data_id_t::set_id(uint64_t id_){
	id = id_;
}

uint64_t data_id_t::get_id(){
	return id;
}

std::string data_id_t::get_type(){
	return type;
}

void data_id_t::add_data(void *ptr_, uint32_t size_){
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		if(data_ptr[i] == NULL){
			data_ptr[i] = ptr_;
			data_size[i] = size_;
			return;
		}
	}
}

void data_id_t::add_id(uint64_t *ptr_, uint32_t size_){
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		if(id_ptr[i] == NULL){
			id_ptr[i] = ptr_;
			id_size[i] = size_;
		}
	}
}

uint64_t data_id_t::get_data_index_size(){
	for(uint64_t i = 0;i < ID_PTR_LENGTH;i++){
		if(data_ptr[i] == NULL){
			return i+1;
		}
	}
	return 0;
}

static void append_to_data(std::string str,
			   uint64_t padding,
			   std::vector<uint8_t> *data){
	if(str.size() > padding){
		PRINT("str.size() > padding", P_CRIT);
	}
	for(uint64_t i = 0;i < padding;i++){
		if(i < str.size()){
			data->push_back(str[i]);
		}else{
			data->push_back(0);
		}
	}
}

static void append_to_data(void* raw,
			   uint64_t len,
			   std::vector<uint8_t> *data){
	for(uint64_t i = 0;i < len;i++){
		data->push_back(((uint8_t*)raw)[i]);
	}
}

/*
  Pulled from id.h

  ID is the first 8 bytes
  Next 24 bytes are the name of the data type, padded
  with zeroes (null).

  The rest is formatted in the following blocks
  2 byte for the entry in the data_ptr, should be enough
  4 bytes for the size of the string to be parsed
  The raw string is here

 */

std::vector<uint8_t> data_id_t::get_data_block(){
	std::vector<uint8_t> retval;
	append_to_data(&id, 8, &retval);
	append_to_data(type, 24, &retval);
	for(uint16_t i = 0;i <= ID_PTR_LENGTH-1;i++){
		if(data_ptr[i] == nullptr){
			continue;
			// should I just exit here?
		}
		append_to_data(&i, 2, &retval);
		append_to_data(&data_size[i], 4, &retval);
		append_to_data(data_ptr[i], data_size[i], &retval);
	}
	return retval;
}

static data_id_t *id_fast_find(uint64_t id){
	const uint64_t tmp_entry = id >> 32;
	const bool proper_id =
		id_list_vector.at(tmp_entry)->get_id() == id;
	if(proper_id){
		return id_list_vector[tmp_entry];
	}
	return nullptr;

}

static data_id_t *id_std_find(uint64_t id){
	for(uint64_t i = 0;i < id_list_vector.size();i++){
		data_id_t *tmp_id = id_list_vector[i];
		if(id == tmp_id->get_id()){
			return id_list_vector[i];
		}
	}
	return nullptr;
}

data_id_t *id_array::ptr(uint64_t id){
	data_id_t *retval = nullptr;
	LOCK_RUN(id_list_vector_lock, retval = id_fast_find(id));
	if(retval == nullptr){
		LOCK_RUN(id_list_vector_lock, retval = id_std_find(id));
	}
	return retval;
}

bool id_array::exists(uint64_t id){
	return id_array::ptr(id);
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
