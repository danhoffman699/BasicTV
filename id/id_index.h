#include "id.h"
#ifndef ID_INDEX_H
#define ID_INDEX_H
#include "array"
#define ID_INDEX_REQUEST (1 << 0)
#define ID_INDEX_CATALOG (0 << 0)

/*
  id_index_t: Any list of IDs. This is mainly used for single and
  batch requests of IDs from one node. This is not used for the main
  list, because the amount of information needed to be allocated is
  too large to be practically implemented as a networked data type

  Flags are used to set whether this index is a request or a
  catalog of available IDs. If more information is needed to
  represent a catalog, then use the linked list in the IDs to 
  connect them, as to ensure that, as long as they are networked
  together and one is guaranteed to be up to date, then they
  all should be relatively up to date (or should at least exist, 
  doesn't have to be networked for it to be okay)
  
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
	std::vector<uint64_t> id_index;
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
