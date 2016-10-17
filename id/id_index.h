#ifndef ID_INDEX_H
#define ID_INDEX_H
#include "array"
struct id_index_t{
private:
	/*
	  A networkable index of all of the IDs that a network peer has.
	  A client will request one of these from each network peer and
	  request the packet from the one with the best connection.

	  TODO: syncronize garbage collection with the generation of a
	  new id_index_t, so there are no instances where informatio is
	  sent out that is invalid at the time of sending (but there can
	  still be cases where a version becomes stale and the information
	  is no longer valid). Perhaps add a grace period between de-listing
	  and deleting, so it isn't advertised that the ID exists anymore, but
	  can still be delivered upon request (1 minute)?

	  Grace period sounds like a good idea.
	 */
	std::array<uint64_t, ID_ARRAY_SIZE> id_index = {0};
public:
	data_id_t id;
	id_index_t();
	~id_index_t();
};
#endif
