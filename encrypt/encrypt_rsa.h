#include "../util.h"
#include "../id/id.h"
#include "../main.h"
#ifndef ENCRYPT_RSA_H
#define ENCRYPT_RSA_H

/*
  RSA is the standard encryption system for BasicTV. This hasn't been
  implemented yet, and would probably be just a wrapper for OpenSSL or
  something like that. However, the data is stored inside of custom,
  possibly networkable, data types.
*/

#define RSA_MAX_KEY_LENGTH 4096
#define RSA_DEFAULT_KEY_LENGTH 4096

#define RSA_PUB_KEY_REFERENCE_LENGTH 256
#define RSA_PUB_KEY_REFERENCE_LIST_SIZE 16

class rsa_key_t{
protected:
	std::array<uint8_t, RSA_MAX_KEY_LENGTH/8> key = {{0}};
	uint16_t key_length = 0;
public:
	rsa_key_t();
	~rsa_key_t();
	void list_virtual_data(data_id_t *id);
	void get_key(std::array<uint8_t, RSA_MAX_KEY_LENGTH/8> *key_,
		     uint16_t *key_length_);
	void set_key(std::array<uint8_t, RSA_MAX_KEY_LENGTH/8> key_,
		     uint16_t key_length_);
};

class rsa_priv_key_t : public rsa_key_t{
public:
	data_id_t id;
	rsa_priv_key_t();
	~rsa_priv_key_t();
};

/*
  Due to the easiness of freebooting content, each RSA public key has the
  option of having a list of references. A reference is a URL to a webpage
  (or anything typically resolvable) that contains the RSA public key for
  BasicTV (easily exportable).

  When a channel is pulled up, the user (should) have the option of listing
  all of the references for the ID. These references, ideally, should link
  to the website of the organization in a simple and short way (for example:
  www.example.org/stream/basictv_pubkey, not tinyurl.com/jFy8Fk or anything
  intentionally misguiding or not obviously legit from the URL). The URL should
  prove the source is legit, while the information therein should match the
  received RSA public key. However, the public key should still be networked
  simply because it is easy to do so and the viewability of the content on
  an always on network shouldn't be limited by uptime of an unrelated service.

  Freebooting is a serious problem. I considered having a list of RSA public
  keys that upload freebooted or otherwise illegal content (opt-in). However,
  such an approach is pretty bad, given that there is little to no overhead to 
  create a new RSA private-public keypair (unless a 65536-bit or higher key
  length is mandated, whose decryption overhead is FAR beyond reasonable), and
  that a majority of users don't care if the information is freebooted, so it
  makes little sense to opt-in for that reason (but opting in for not fetching
  and serving illegal content would be reasonable).
  
  Most programmatic approaches can be exploited, while also misflagging legit
  stream information as freebooted. The only sane approach I can think of is
  scoring the URL by shortness and how much of it is actually English versus
  random jibberish. I feel like this is the best approach to an unsolvable
  problem.
*/

class rsa_pub_key_t : public rsa_key_t{
private:
	std::array<
		std::array<uint8_t, RSA_PUB_KEY_REFERENCE_LENGTH>,
		RSA_PUB_KEY_REFERENCE_LIST_SIZE> references = {{{{0}}}};
public:
	data_id_t id;
	rsa_pub_key_t();
	~rsa_pub_key_t();
};

struct rsa_pair_t{
private:
	uint64_t priv_key_id = 0;
	uint64_t pub_key_id = 0;
public:
	data_id_t id;
	rsa_pair_t();
	~rsa_pair_t();
	uint64_t get_priv_key_id();
	void set_priv_key_id(uint64_t priv_key_id_);
	uint64_t get_pub_key_id();
	void set_pub_key_id(uint64_t pub_key_id_);
	void gen_pair();
};

namespace rsa{
	
};

#endif
