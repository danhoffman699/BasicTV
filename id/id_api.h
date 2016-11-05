#include "id.h"
#include "../util.h"
#ifndef ID_API_H
#define ID_API_H
#include "tuple"

/*
  Redefined and more unified ID API. 
 */

// Function macros for common ID API calls

#define PTR_DATA(id, type) ((type*)id_api::array::ptr_data(id, #type))
#define PTR_ID(id, type) (id_api::array::ptr_id(id, #type))

namespace id_api{
	namespace array{
		data_id_t *ptr_id(uint64_t id,
				  std::string type);
		data_id_t *ptr_id(uint64_t id,
				  std::array<uint8_t, TYPE_LENGTH> type);
		void *ptr_data(uint64_t id,
				  std::string type);
		void *ptr_data(uint64_t id,
				  std::array<uint8_t, TYPE_LENGTH> type);
		void add(uint64_t id,
			 data_id_t *ptr);
		void del(uint64_t id); // no type
		void add_data(std::vector<uint8_t> data_);
		std::vector<uint64_t> sort_by_pgp_pubkey(std::vector<uint64_t> tmp);
		std::vector<uint64_t> get_forward_linked_list(uint64_t id, uint64_t height);
	}
	namespace cache{
		// get_type_vector_ptr should never be used outside of id_api.cpp	
		void add(uint64_t id,
			 std::array<uint8_t, TYPE_LENGTH> type);
		void add(uint64_t id,
			 std::string type);
		void del(uint64_t id,
			 std::array<uint8_t, TYPE_LENGTH> type);
		void del(uint64_t id,
			 std::string type);
		std::vector<uint64_t> get(std::array<uint8_t, TYPE_LENGTH> type);
		std::vector<uint64_t> get(std::string type);

	}
};

#endif
