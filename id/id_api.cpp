#include "id.h"
#include "id_api.h"

#include "../tv/tv_dev_audio.h"
#include "../tv/tv_dev_video.h"
#include "../tv/tv_frame_audio.h"
#include "../tv/tv_frame_video.h"
#include "../tv/tv_frame_caption.h"
#include "../tv/tv_channel.h"
#include "../tv/tv_window.h"
#include "../tv/tv_menu.h"
#include "../net/proto/inbound/net_proto_inbound_data.h"
#include "../net/proto/outbound/net_proto_outbound_data.h"
#include "../net/proto/net_proto_con_req.h"
#include "../net/proto/net_proto_peer.h"
#include "../net/proto/net_proto_con_req.h"
#include "../net/proto/net_proto.h"

#include "../input/input.h"
#include "../input/input_ir.h"

#include "../settings.h"


static std::vector<data_id_t*> id_list;
static std::vector<std::pair<std::vector<uint64_t>, std::array<uint8_t, TYPE_LENGTH> > > type_cache;

static data_id_t *id_find(uint64_t id){
	for(uint64_t i = 0;i < id_list.size();i++){
		if(unlikely(id_list[i]->get_id() == id)){
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
	// blank type is a wildcard, currently onlu used for RSA sorting
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

void id_api::array::add(data_id_t *ptr){
	id_list.push_back(ptr);
}

void id_api::array::del(uint64_t id){
	for(uint64_t i = 0;i < id_list.size();i++){
		if(id_list[i]->get_id() == id){
			id_list.erase(id_list.begin()+i);
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

std::vector<uint64_t> id_api::array::sort_by_rsa_pubkey(std::vector<uint64_t> tmp){
	bool changed = true;
	while(changed){
		changed = false;
		for(uint64_t i = 1;i < tmp.size()-1;i++){
			data_id_t *tmp_array[2] = {nullptr};
			tmp_array[0] = PTR_ID(tmp[i-1], "");
			tmp_array[1] = PTR_ID(tmp[i], "");
			//const bool rsa_greater_than =
			//	pgp::cmp::greater_than(tmp_array[i-1]->get_pgp_cite_id(),
			//			       tmp_array[i]->get_pgp_cite_id());
			const bool rsa_greater_than = false;
			if(rsa_greater_than){
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
	print("type cache of " + (std::string)(char*)(&tmp[0]) + " is " + std::to_string(type_cache.size()), P_SPAM);
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
std::vector<uint64_t> id_api::array::get_forward_linked_list(uint64_t id){
	std::vector<uint64_t> retval;
	while(id != 0){
		data_id_t *id_ptr = PTR_ID(id, );
		retval.push_back(id);
		id = id_ptr->get_next_linked_list();
	}
	return retval;
}

void id_api::linked_list::link_vector(std::vector<uint64_t> vector){
	switch(vector.size()){
	case 0:
		print("vector is empty", P_NOTE);
		return;
	case 1:
		print("vector has one entry, can't link to anything" , P_NOTE);
		return;
	case 2:
		PTR_ID(vector[0], )->set_next_linked_list(vector[1]);
		PTR_ID(vector[1], )->set_next_linked_list(vector[0]);
		return;
	}
	data_id_t *first = PTR_ID(vector[0], );
	if(first != nullptr){
		first->set_next_linked_list(vector[1]);
	}else{
		print("first entry is a nullptr, this can be fixed, but I didn't bother and this shouldn't happen anyways", P_ERR);
	}
	for(uint64_t i = 1;i < vector.size()-1;i++){
		data_id_t *id = PTR_ID(vector[i], );
		id->set_next_linked_list(vector[i+1]);
		id->set_prev_linked_list(vector[i-1]);
	}
	PTR_ID(vector[vector.size()-1], )->set_prev_linked_list(
		vector[vector.size()-2]);
}

std::vector<uint64_t> id_api::get_all(){
	std::vector<uint64_t> retval;
	for(uint64_t i = 0;i < id_list.size();i++){
		if(id_list[i] != nullptr){
			retval.push_back(id_list[i]->get_id());
		}
	}
	return retval;
}

// make this less stupid

#define DELETE_TYPE_2(a) if(ptr->get_type() == #a){delete (a*)ptr->get_ptr();return;}
#define DELETE_TYPE(a) if(ptr->get_type() == #a){delete (a*)ptr->get_ptr();continue;}

// refactor

/*
  Periodically update with rgrep
 */

void id_api::destroy(id_t_ id){	
	data_id_t *ptr = PTR_ID(id, );
	std::vector<uint8_t> exportable_data =
		ptr->export_data();
	// doesn't work if no struct types are exportable
	if((exportable_data.size() != 0) && (settings::get_setting("export_data") == "true")){
		// make this work on Windows
		const std::string path =
			"data_folder/"+
			std::to_string(id);
		system(("touch " + path).c_str());
		system(("rm " + path).c_str());
		std::ofstream out(path, std::ios::out | std::ios::binary);
		if(out.is_open() == false){
			print("cannot open file for exporting", P_ERR);
		}
		std::vector<uint8_t> exported_data =
			ptr->export_data();
		for(uint64_t i = 0;i < exported_data.size();i++){
			out << exported_data[i];
		}
		out.close();
	}else{
		print("not exporting ID " + std::to_string(id),  P_ERR);
	}
	// TV subsystem
	DELETE_TYPE_2(tv_frame_video_t);
	DELETE_TYPE_2(tv_frame_audio_t);
	DELETE_TYPE_2(tv_frame_caption_t);
	DELETE_TYPE_2(tv_window_t);
	DELETE_TYPE_2(tv_channel_t);
	DELETE_TYPE_2(tv_menu_entry_t);
	DELETE_TYPE_2(tv_menu_t);

	// net (proto and standard)
	DELETE_TYPE_2(net_socket_t);
	DELETE_TYPE_2(net_peer_t);	
	DELETE_TYPE_2(net_request_t);
	DELETE_TYPE_2(net_proto_con_req_t);
	DELETE_TYPE_2(net_cache_t); // ?

	// IR
	DELETE_TYPE_2(ir_remote_t);

	// input
	DELETE_TYPE_2(input_dev_standard_t);

	// cryptography
	DELETE_TYPE_2(encrypt_key_pair_t);
	DELETE_TYPE_2(encrypt_priv_key_t);
	DELETE_TYPE_2(encrypt_pub_key_t);
}

void id_api::destroy_all_data(){
	for(uint64_t i = 0;i < id_list.size();i++){
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
}

/*
  Goes through all of the IDs, and exports a select few to the disk.
  IDs with the lowest last_access_timestamp_micro_s are exported. Keep
  looping this over until enough memory has been freed. 
 */

static uint64_t get_mem_usage_kb(){
#ifdef __linux
	std::ifstream mem_buf("/proc/self/statm");
	uint32_t t_size = 0, resident = 0, share = 0;
	mem_buf >> t_size >> resident >> share;
	mem_buf.close();
	P_V(resident*sysconf(_SC_PAGE_SIZE)/1024, P_SPAM);
	return resident*sysconf(_SC_PAGE_SIZE)/1024; // RSS
#else
#error No memory function currently exists for non-Linux systems
#endif
}

void id_api::free_mem(){
	uint64_t max_mem_usage = 0;
	try{
		max_mem_usage =
			std::stoi(
				settings::get_setting(
					"max_mem_usage"));
	}catch(...){
		print("using max mem usage default of 1GB", P_NOTE);
		max_mem_usage = 1024*1024; // 1 GB, but should be lower 
	}
	while(get_mem_usage_kb() > max_mem_usage){
		id_t_ lowest_timestamp_id =
			0;
		for(uint64_t i = 0;i < id_list.size();i++){
			data_id_t *lowest =
				PTR_ID(lowest_timestamp_id, );
			data_id_t *current =
				id_list[i];
			if(lowest == nullptr){
				// probably first entry
				lowest_timestamp_id = id_list[i]->get_id();
				lowest = current;
			}else{
				if(lowest->get_last_access_timestamp_micro_s() >
				   current->get_last_access_timestamp_micro_s()){
					lowest_timestamp_id =
						current->get_id();
				}
			}
		}
		id_api::destroy(
			lowest_timestamp_id);
		/*
		  id_api::destroy takes care of exporting to disk (if that makes
		  sense and is allowed per settings.cfg)
		 */
	}
}
