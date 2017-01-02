#include "encrypt_rsa.h"

rsa_key_t::rsa_key_t(){}

rsa_key_t::~rsa_key_t(){}

void rsa_key_t::list_virtual_data(data_id_t *id){
	id->add_data(&(key[0]), RSA_MAX_KEY_LENGTH/8);
	id->add_data(&key_length, sizeof(key_length));
}

void rsa_key_t::get_key(std::array<uint8_t, RSA_MAX_KEY_LENGTH/8> *key_,
			uint16_t *key_length_){
	*key_ = key;
	*key_length_ = key_length;
}

void rsa_key_t::set_key(std::array<uint8_t, RSA_MAX_KEY_LENGTH/8> key_,
			uint16_t key_length_){
	key = key_;
	key_length = key_length_;
}

rsa_priv_key_t::rsa_priv_key_t() : id(this, __FUNCTION__){
	list_virtual_data(&id);
	id.nonet_all_data();
}

rsa_priv_key_t::~rsa_priv_key_t(){}


rsa_pub_key_t::rsa_pub_key_t() : id(this, __FUNCTION__){
	list_virtual_data(&id);
}

rsa_pub_key_t::~rsa_pub_key_t(){}

rsa_pair_t::rsa_pair_t() : id(this, __FUNCTION__){
}

rsa_pair_t::~rsa_pair_t(){
}
