#ifndef PGP_H
#define PGP_H
#include "id/id.h"
#include "array"
#include "string"

/*
  I have pretty large concerns over the speed of decoding this information
  both on the fly and securely with a Raspberry Pi (non-dual-core).
 */

#define PGP_CITE_STR_SIZE 256
#define PGP_CITE_SIZE 16

/*
  pgp_cite_t: contains citations for data_id_t ownership. Not all information
  sent is encrypted, but it is by default. The only items (at the time of
  writing) that are opted-out are the references to this data type and this
  data type. Only the data is encrypted, not the metadata, so individual 
  variables can be encrypted and decrypted. I don't know how the performance
  will fare
*/

struct pgp_priv_key_{
private:
	std::array<uint8_t, 512> privkey = {{0}};
	std::array<uint8_t, 512> pubkey = {{0}};
public:
};

struct pgp_cite_t{
private:
/*	
	List of URLs that contain some sort of citation for the PGP public
	key to give it legitimacy. I would use MIT's PGP keyserver, but
	the search function for verifying the source should be able to
	parse anything that displays the key in plaintext
*/
	std::array<std::array<uint8_t, PGP_CITE_STR_SIZE>, PGP_CITE_SIZE> cite;
	std::array<uint8_t, PGP_PUBKEY_SIZE> pgp_pubkey;
	// pubkey shouldn't have to be networked
public:
	data_id_t id;
	pgp_cite_t();
	~pgp_cite_t();
	void add(std::string url);
	std::array<uint8_t, PGP_PUBKEY_SIZE> get_pgp_pubkey();
};

/*
  In reality, this only does RSA, and PGP is only used to signify
  it is for ownership and because of a naming convention
 */

namespace pgp{
	namespace cmp{
		bool greater_than(std::array<uint8_t, PGP_PUBKEY_SIZE> a,
				  std::array<uint8_t, PGP_PUBKEY_SIZE> b);
		bool less_than(std::array<uint8_t, PGP_PUBKEY_SIZE> a,
			       std::array<uint8_t, PGP_PUBKEY_SIZE> b);
		bool equal_to(std::array<uint8_t, PGP_PUBKEY_SIZE> a,
			      std::array<uint8_t, PGP_PUBKEY_SIZE> b);
	}
	namespace transcode{
		
	};
};

/*
  In reality, this only does RSA, and PGP is only used to signify
  it is for ownership and because of a naming convention
 */

namespace pgp{
	namespace cmp{
		bool greater_than(std::array<uint8_t, PGP_PUBKEY_SIZE> a,
				  std::array<uint8_t, PGP_PUBKEY_SIZE> b);
		bool less_than(std::array<uint8_t, PGP_PUBKEY_SIZE> a,
			       std::array<uint8_t, PGP_PUBKEY_SIZE> b);
		bool equal_to(std::array<uint8_t, PGP_PUBKEY_SIZE> a,
			      std::array<uint8_t, PGP_PUBKEY_SIZE> b);
	}
	namespace transcode{
		
	};
};

#endif
