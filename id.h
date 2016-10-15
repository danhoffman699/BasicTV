#include "main.h" // STD_ARRAY_LENGTH
#ifndef ID_H
#define ID_H
#include "cstdint"
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
  Next 8 bytes are the pgp_cite_t ID

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

/*
  ID_PTR_LENGTH is any pointer array size, not just id_ptr
 */
#define ID_PTR_LENGTH STD_ARRAY_LENGTH
/*
  don't actually network this information. this is
  for any variable that is derived from other information
  and shouldn't be networked for some reason (PGP pubkey is
  the big one, because verification after every send is
  going to be bad)
 */
#define ID_DATA_CACHE (1 << 0)
/*
  force no PGP encryption on one specific variable, only
  used for core PGP functions (ID to pgp_cite_t and pgp_cite_t
  internal variables).
 */
#define ID_DATA_NOPGP (1 << 1)

/*
  The position in the array is encoded into the ID.

  TODO: convert std::vector to std::array
 */

#define ID_RAND_MASK 0xFFFFFFFF00000000
#define ID_SET_RAND(x, y) ((x) | ((y) << 32)) 
#define ID_PULL_RAND(x) ((x) >> 32) // doesn't need logical and

#define ID_POS_MASK 0x00000000FFFFFFFF
#define ID_SET_POS(x, y) ((x) | ((y) & ID_POS_MASK))
#define ID_PULL_POS(x) ((x) & (~ID_POS_MASK))

/*
  Array is dunamically allocated to prevent the stack from
  having any major problems. I think dedicating 64MB of RAM to
  just IDs should be plenty (assuming 64-bit pointers). I might 
  want to install a spam-filter or just kick people from having their 
  info relayed if they spaa ton of data (identifiable via PGP)

  TODO: clearly define what "spam" means in the previous paragraph
 */

#define ID_ARRAY_SIZE ((64*1024*1024)/8)

struct data_id_t{
private:
	uint64_t id = 0;
	std::string type;
	void *ptr = nullptr;
	uint64_t pgp_cite_id = 0;
	/*
	  If a pgp_cite_t item has not come yet, then
	  put that data into the pgp_unverified vector and
	  run that when pgp_cite_id is found

	  pgp_cite_t cannot be changed. If a change is detected, then 
	  reject the incoming data. This is not typical software behavior
	 */
	std::vector<std::vector<uint8_t> > pgp_backlog;
	
	std::array<void*, ID_PTR_LENGTH> data_ptr = {{nullptr}};
	std::array<uint32_t, ID_PTR_LENGTH> data_size = {{0}};
	std::array<uint8_t, ID_PTR_LENGTH> data_flags = {{0}};
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
	void *get_ptr();
	void add_data(void *ptr_, uint32_t size_, uint64_t flags = 0);
	void add_id(uint64_t *ptr_, uint32_t size_);
	uint64_t get_data_index_size();
	std::vector<uint8_t> export_data();
	void import_data(std::vector<uint8_t> data);
	void import_data(std::string data);
	void pgp_decrypt_backlog();
};

namespace id_array{
	bool exists(uint64_t id);
	bool exists_in_list(uint64_t id,
			    std::array<uint64_t, ID_PTR_LENGTH> catalog);
	void optimize(std::array<uint64_t, ID_PTR_LENGTH> *array);
	std::vector<uint64_t> all_of_type(std::string type);
	data_id_t *ptr_id(uint64_t id);
	void *ptr_data(uint64_t id);
};
#endif
