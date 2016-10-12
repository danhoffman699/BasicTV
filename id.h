#include "main.h" // STD_ARRAY_LENGTH
#ifndef ID_H
#define ID_H
#include "fstream"
#include "array"
#include "vector"
#include "random"
/*
  Custom data types reference each other with IDs since
  pointers don't work over a network. This allows for easy
  transmission of multiple different structures reliant
  upon statically allocated data types.
 */

/*
  Networking standard:
  ID is the first 8 bytes
  Next 24 bytes are the name of the data type, padded
  with zeroes (null).

  The rest is formatted in the following blocks
  2 byte for the entry in the data_ptr, should be enough
  4 bytes for the size of the string to be parsed
  The raw string is here

  If an ID already exists and the type checks out, then
  update the current version with the new version.

  If an ID already exists and the type does not check out, then
  discard the ID (possible attack vector)

  It is assumed that, somehow, the connection between the
  client and server is secured, so the actual content of 
  the packets shouldn't be tampered with.

  TODO: Establish cryptographic ownership of data
  What I want to do is have an unencrypted string that
  "cites" websites that have a copy of the PGP public key
  that this is signed with, and run that by the end-user
  through some sort of easy GUI system. All data_id_t
  metadata is NOT encrypted, just data_ptr (id_ptr isn't sent)

  All networked data between the client and server on
  the "main" port should be done in this format, and 
  other services that might pop up should use another
  format
 */

/*
  id_t: ID and pointer system for the networking system
 */

#define ID_PTR_LENGTH STD_ARRAY_LENGTH

struct data_id_t{
private:
	uint64_t id;
	std::string type;
	void *ptr;
	std::array<void*, ID_PTR_LENGTH> data_ptr = {{nullptr}};
	std::array<uint32_t, ID_PTR_LENGTH> data_size = {{0}};
	std::array<uint64_t*, ID_PTR_LENGTH> id_ptr = {{nullptr}};
	std::array<uint32_t, ID_PTR_LENGTH> id_size = {{0}};
	// everything in id_ptr is put into data_ptr in another call. id_ptr
	// should never call data_ptr. id_ptr exists for garbage collection
	// and other internal functions
	/*
	  std::array doesn't have to be used here, since they
	  are defined lengths at initialization and aren't
	  networked (or shouldn't be)
	 */
public:
	data_id_t(void *ptr_, std::string type_);
	~data_id_t();
	void set_id(uint64_t id_);
	uint64_t get_id();
	std::string get_type();
	void add_data(void *ptr_, uint32_t size_);
	void add_id(uint64_t *ptr_, uint32_t size_);
	uint64_t get_data_index_size();
	std::vector<uint8_t> get_data_block();
};

namespace id_array{
	bool exists(uint64_t id);
	bool exists_in_list(uint64_t id,
			    std::array<uint64_t, ID_PTR_LENGTH> catalog);
	void optimize(std::array<uint64_t, ID_PTR_LENGTH> *array);
	data_id_t *ptr(uint64_t id);
};
#endif
