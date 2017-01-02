#include "../id/id.h"
#include "../id/id_api.h"
#ifndef ENCRYPT_H
#define ENCRYPT_H
#define ENCRYPT_MAX_KEY_LENGTH 8192
/*
  encrypt.h: Encryption API

  All data that is considered to be "encrypted" has a special set of bits 
  slapped to the back of it. These bits are the ID for the type of encryption
  scheme that has been used, so that future versions of BasicTV (not often) 
  will have the option of multiple encryption schemes.

  As of now, RSA is the only option. However, I want to at least implement
  decryption schemes that fall outside of BQP complexity (quantum computers
  can break it in polynomial time)
 */

/*
  Important info about encryption system:
  All fingerprints on public key encryption are the SHA-512 hash of the public
  key (512 bits long, so 64 bit)

  This system guarantees ownership of content by public keys. Proof of owning
  public keys should come from external references: posting links to webpages
  that contain the RSA public key (more fancy things might be implemented like
  encrypting with the public key, but that isn't as user friendly). These IDs'
  proof comes from a quick download and search, as well as how legit the URL
  looks when the user sees it
*/

#define ENCRYPT_UNDEFINED (0)
#define ENCRYPT_RSA (1)

struct encrypt_key_pair_t{
private:
	id_t_ priv_key = 0;
	id_t_ pub_key = 0;
public:
	data_id_t id;
	encrypt_key_pair_t();
	~encrypt_key_pair_t();
	id_t_ get_priv_key_id();
	void set_priv_key_id(id_t_ priv_key_);
	id_t_ get_pub_key_id();
	void set_pub_key_id(id_t_ pub_key_);
};

struct encrypt_key_t{
protected:
	std::vector<uint8_t> key;
	uint8_t encryption_scheme = 0;
public:
	encrypt_key_t();
	~encrypt_key_t();
	void list_virtual_data(data_id_t *id);
	void set_encrypt_key(std::vector<uint8_t> key_,
			     uint8_t encryption_scheme_);
	void get_encrypt_key(std::vector<uint8_t> *key_,
			     uint8_t *encryption_scheme_);
};

struct encrypt_pub_key_t : virtual encrypt_key_t{
private:
public:
	data_id_t id;
	encrypt_pub_key_t();
	~encrypt_pub_key_t();
};

struct encrypt_priv_key_t : virtual encrypt_key_t{
private:
public:
	data_id_t id;
	encrypt_priv_key_t();
	~encrypt_priv_key_t();
};

// key_id can be public or private
namespace encrypt_api{
	std::vector<uint8_t> encrypt(std::vector<uint8_t> data,
				     id_t_ key_id);
	std::vector<uint8_t> decrypt(std::vector<uint8_t> data,
				     id_t_ key_id);
};

#endif
