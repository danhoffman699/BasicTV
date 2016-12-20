#include "../main.h"
#ifndef ID_H
#define ID_H
#include "cstdint"
#include "fstream"
#include "array"
#include "vector"
#include "random"

#define ADD_DATA(x) (id.add_data(&x, sizeof(x)))
#define ADD_DATA_ARRAY(x, y, z) (id.add_data(&(x[0]), y*z))
#define ADD_DATA_NONET(x) (id.add_data(&x, sizeof(x), ID_DATA_NONET))
#define ADD_DATA_PTR(x) (id->add_data(&x, sizeof(x)))

/*
  id_t: ID and pointer system for the networking system
 */

/*
  ID_PTR_LENGTH is any pointer array size, not just id_ptr
 */
#define ID_PTR_LENGTH 32
/*
  don't actually network this information. this is
  for any variable that is derived from other information
  and shouldn't be networked for some reason (PGP pubkey is
  the big one, because verification after every send is
  going to be bad)
 */

#define ID_DATA_CACHE (1 << 0)
#define ID_DATA_NONET ID_DATA_CACHE

/*
  Array is dunamically allocated to prevent the stack from
  having any major problems. I think dedicating 64MB of RAM to
  just IDs should be plenty (assuming 64-bit pointers). I might 
  want to install a spam-filter or just kick people from having their 
  info relayed if they spaa ton of data (identifiable via PGP)

  TODO: clearly define what "spam" means in the previous paragraph
 */

// I know this doesn't have any actual bearing on 64MB, but it will work for now

#define ID_ARRAY_SIZE ((64*1024*1024)/8)

#define ID_SIZE 16

/*
  TODO: create a larger ID so an RSA fingerprint can be embedded
  into the ID? Regardless, create a typedef for the IDs
 */
//typedef id_t std::array<uint8_t, ID_SIZE>;

#define ID_LL_WIDTH 3
#define ID_LL_HEIGHT 16

#define TYPE_LENGTH 32 // standard length for C++ types

typedef uint64_t id_t_; // needs a snazzier name

struct data_id_t{
private:
	// half UUID, half RSA fingerprint (for verification)
	uint64_t id = 0;
       	std::array<uint8_t, TYPE_LENGTH> type = {{0}};
	void *ptr = nullptr;
	/*
	  If a pgp_cite_t item has not come yet, then
	  put that data into the pgp_backlog vector and
	  run that when pgp_cite_id is found

	  pgp_cite_t cannot be changed. If a change is detected, then 
	  reject the incoming data. This is not typical software behavior
	 */
	uint64_t pgp_cite_id = 0;
	/*
	  Contains all information that can't be decrypted because pgp_cite_id
	  is not valid. If pgp_cite_id doesn't match up with pgp_backlog data,
	  then destroy this variable, as it is probably phishing
	 */
	std::vector<std::vector<uint8_t> > pgp_backlog;

	std::array<void*, ID_PTR_LENGTH> data_ptr = {{nullptr}};
	std::array<uint64_t, ID_PTR_LENGTH> data_ptr_upper_range = {{0}};
	std::array<uint64_t, ID_PTR_LENGTH> data_ptr_lower_range = {{0}};
	std::array<uint32_t, ID_PTR_LENGTH> data_size = {{0}};
	/*
	  Flags for the data tell us if it is compressed, encrypted, or
	  whatever.

	  TODO: throw an error if data comes over that does NOT have anything
	  encrypted, as showing proof of ownership there would be impossible
	 */
	std::array<uint64_t, ID_PTR_LENGTH> data_flags = {{0}};
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
	/*
	  Standardized linked list system (IDs). This can exist in up to two 
	  dimensions. The only current case where two dimensions is used
	  is with tv_frame_t (one for time, the other for quality). Other
	  cases for the 1D would be a TV Guide style system, along with
	  any other streaming system (tv_channel_t doesn't handle the streaming,
	  it just handles the metadata and one ID per sequenceable item (TV
	  Guide, video & audio stream, other metadata, etc.). This can also
	  be used for requesting large sums of data. The only information that
	  would need to be sent across would be an ID to a local version, and
	  the number of entires forwards/backwards that should be sent back.

	  TODO: create a standardized version of querying for data using
	  the linked list system (as specified above).
	 */
	std::array<uint64_t, ID_LL_WIDTH*ID_LL_HEIGHT> linked_list = {{0}};
	/*
	  Keep all imported data. This preserves the public-private encryption
	 */
	std::vector<uint8_t> imported_data;
	void init_list_all_data();
	void init_gen_id();
	void init_type_cache();
public:
	data_id_t(void *ptr_, std::string type_);
	~data_id_t();
	// getters and setters
	void set_id(uint64_t id_);
	uint64_t get_id();
	std::string get_type();
	void *get_ptr();
	//std::array<uint8_t, PGP_PUBKEY_SIZE> get_owner_pubkey();
	void set_pgp_cite_id(uint64_t id){pgp_cite_id = id;} // probably should fix this soon
	uint64_t get_pgp_cite_id();
	uint64_t get_data_index_size();
	uint64_t get_next_linked_list(uint64_t height);
	uint64_t get_prev_linked_list(uint64_t height);
	void set_next_linked_list(uint64_t height, uint64_t data);
	void set_prev_linked_list(uint64_t height, uint64_t data);
	// pointer list modififers
	/*
	  size of data is referring to the type size and the array size, whereas
	  the size of the ID is referring to just the array size, since the size
	  is assumed with the pointer type (8 bytes, but maybe more later?)
	 */
	void add_data(void *ptr_, uint32_t size_,  uint64_t flags = 0);
	void add_id(uint64_t *ptr_, uint32_t size_);
	// export and import data
	std::vector<uint8_t> export_data();
	void import_data(std::vector<uint8_t> data);
	void import_data(std::string data);
	// misc.
	void pgp_decrypt_backlog();
	void dereference_id(uint64_t id_);
	bool is_owner();
};
#endif
