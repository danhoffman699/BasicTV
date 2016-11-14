#include "util.h"
#include "id/id.h"
#include "main.h"
#ifndef RSA_H
#define RSA_H
/*
  RSA is the standard encryption system for BasicTV. This hasn't been
  implemented yet, and would probably be just a wrapper for OpenSSL or
  something like that. However, the data is stored inside of custom,
  possibly networkable, data types.
 */

#define RSA_MAX_KEY_LENGTH 4096
#define RSA_DEFAULT_KEY_LENGTH 4096

class rsa_key_t{
protected:
	std::array<uint8_t, RSA_MAX_KEY_LENGTH/8> key = {{0}};
	uint16_t key_length = 0;
public:
	rsa_key_t();
	~rsa_key_t();
	void get_key(std::array<uint8_t, RSA_MAX_KEY_LENGTH/8> *key_,
		     uint16_t *key_length);
	void set_key(std::array<uint8_t, RSA_MAX_KEY_LENGTH/8> key_,
		     uint16_t key_length);
};

class rsa_priv_key_t : public rsa_key_t{
public:
	data_id_t id;
	rsa_priv_key_t();
	~rsa_priv_key_t();
};

class rsa_pub_key_t{
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
