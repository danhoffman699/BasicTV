#include "id.h"
#ifndef ID_INDEX_H
#define ID_INDEX_H
#include "array"
#define ID_INDEX_REQUEST (1 << 0)
/*
  id_index_t: A networkable index of all of the IDs of a network peer.
  A client will request one of these from each network peer and
  request the packet from the one with the best connection.

  flags is used to set if this is a request or just a standard
  index that is relayed
  
  TODO: syncronize garbage collection with the generation of a
  new id_index_t, so there are no instances where information is
  sent out that is invalid at the time of sending (but there can
  still be cases where a version becomes stale and the information
  is no longer valid). Perhaps add a grace period between de-listing
  and deleting, so it isn't advertised that the ID exists anymore, but
  can still be delivered upon request (1 minute)?
  
  Grace period sounds like a good idea.
*/
 struct id_index_t{
private:
	std::array<uint64_t, ID_ARRAY_SIZE> id_index = {{0}};
	uint8_t flags = 0;
public:
	data_id_t id;
	id_index_t();
	~id_index_t();
	void add(uint64_t id_);
	uint64_t get(uint64_t entry);
	uint8_t get_flags();
	void set_as_request();
	void set_as_standard();
};
#endif
