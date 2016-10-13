#include "id.h"
#ifndef PGP_H
#define PGP_H
#include "array"
#include "string"

/*
  I have pretty large concerns over the speed of decoding this information
  both on the fly and securely with a Raspberry Pi (non-dual-core).
 */

#include PGP_CITE_STR_SIZE 256
#include PGP_CITE_SIZE 16
#include PGP_CITE_KEY_SIZE
#include PGP_PUBKEY_SIZE 8192

/*
  pgp_cite_t: contains citations for data_id_t ownership. Not all information
  sent is encrypted, but it is by default. The only items (at the time of
  writing) that are opted-out are the references to this data type and this
  data type. Only the data is encrypted, not the metadata, so individual 
  variables can be encrypted and decrypted. I don't know how the performance
  will
 */

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
};
#endif
