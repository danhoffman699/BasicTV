#include "id.h"
#include "../util.h"
#ifndef ID_API_H
#define ID_API_H
#include "tuple"
#include "unistd.h"
#include "sys/resource.h"
#include "cstdio"

/*
  Redefined and more unified ID API. 
 */

// Function macros for common ID API calls

/*
  Currently only used in stats collection for sockets. If the information
  doesn't exist locally, then it has fallen out of use. This DOES allow for
  re-loading of cached data (since it shouldn't be too slow)
 */
#define ID_LOOKUP_FAST (1 << 0)

#define PTR_DATA_FAST(id, type) ((type*)id_api::array::ptr_data(id, #type, ID_LOOKUP_FAST))
#define PTR_ID_FAST(id, type) (id_api::array::ptr_id(id, #type, ID_LOOKUP_FAST))

#define PTR_DATA(id, type) ((type*)id_api::array::ptr_data(id, #type))
#define PTR_ID(id, type) (id_api::array::ptr_id(id, #type))

namespace id_api{
	namespace array{
		data_id_t *ptr_id(id_t_ id,
				  std::string type,
				  uint8_t flags = 0);
		data_id_t *ptr_id(id_t_ id,
				  std::array<uint8_t, TYPE_LENGTH> type,
				  uint8_t flags = 0);
		void *ptr_data(id_t_ id,
				  std::string type,
				  uint8_t flags = 0);
		void *ptr_data(id_t_ id,
				  std::array<uint8_t, TYPE_LENGTH> type,
				  uint8_t flags = 0);
		void add(data_id_t *ptr);
		void del(id_t_ id); // no type
		id_t_ add_data(std::vector<uint8_t> data_);
	}
	namespace cache{
		// get_type_vector_ptr should never be used outside of id_api.cpp	
		void add(id_t_ id,
			 std::array<uint8_t, TYPE_LENGTH> type);
		void add(id_t_ id,
			 std::string type);
		void del(id_t_ id,
			 std::array<uint8_t, TYPE_LENGTH> type);
		void del(id_t_ id,
			 std::string type);
		std::vector<id_t_> get(std::array<uint8_t, TYPE_LENGTH> type);
		std::vector<id_t_> get(std::string type);
	}
	namespace linked_list{
		// next and previous are in the id itself, no interdependency
		void link_vector(std::vector<id_t_> vector);
		// perhaps redefine this for standard when that's needed
		uint64_t distance_fast(id_t_ linked_list_id,
				       id_t_ target_id); // only used in stats
		std::vector<id_t_> get_forward_linked_list(id_t_ id);
	};
	namespace sort{
		std::vector<id_t_> fingerprint(std::vector<id_t_> tmp);
		/*
		  Perhaps sort by last access time (when that gets implemented)?
		 */
	};
	std::vector<id_t_> get_all();
	void free_mem();
	void destroy(id_t_ id);
	void destroy_all_data();
};
#endif
