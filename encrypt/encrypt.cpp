#include "encrypt.h"

encrypt_key_pair_t::encrypt_key_pair_t() : id(this, __FUNCTION__){
}

encrypt_key_pair_t::~encrypt_key_pair_t(){
}

id_t_ encrypt_key_pair_t::get_priv_key_id(){
	return priv_key;
}

void encrypt_key_pair_t::set_priv_key_id(id_t_ priv_key_){
	priv_key = priv_key_;
}

id_t_ encrypt_key_pair_t::get_pub_key_id(){
	return pub_key;
}

void encrypt_key_pair_t::set_pub_key_id(id_t_ pub_key_){
	pub_key = pub_key_;
}

encrypt_key_t::encrypt_key_t(){}

encrypt_key_t::~encrypt_key_t(){}

void encrypt_key_t::list_virtual_data(data_id_t *id){
	id->add_data(&key,
		     65536, // absolute max size
		     ID_DATA_BYTE_VECTOR);
	id->add_data(&encryption_scheme,
		     1);
}

void encrypt_key_t::set_encrypt_key(std::vector<uint8_t> key_,
				    uint8_t encryption_scheme_){
	key = key_;
	encryption_scheme = encryption_scheme_;
}

void encrypt_key_t::get_encrypt_key(std::vector<uint8_t> *key_,
				    uint8_t *encryption_scheme_){
	*key_ = key;
	*encryption_scheme_ = encryption_scheme;
}

encrypt_pub_key_t::encrypt_pub_key_t() : id(this, __FUNCTION__){
	list_virtual_data(&id);
}

encrypt_pub_key_t::~encrypt_pub_key_t(){}

encrypt_priv_key_t::encrypt_priv_key_t() : id(this, __FUNCTION__){
	list_virtual_data(&id);
	id.nonet_all_data();
}

encrypt_priv_key_t::~encrypt_priv_key_t(){}


/*
  This works on private and public keys in the same way. The only reason why
  there are two key types is so every instance of a private key is guaranteed
  to be non-networkable, but exportable (through the initializer)
 */
static void encrypt_pull_key_info(id_t_ id,
				  std::vector<uint8_t> *key,
				  uint8_t *encryption_scheme){
	data_id_t *ptr = PTR_ID(id, );
	if(ptr == nullptr){
		print("id is nullptr", P_ERR);
	}
	if(ptr->get_type() == "encrypt_pub_key_t"){
		encrypt_pub_key_t *pub_key =
			(encrypt_pub_key_t*)ptr->get_ptr();
		if(pub_key == nullptr){
			print("can't load public key", P_ERR);
		}
		pub_key->get_encrypt_key(key, encryption_scheme);
	}else if(ptr->get_type() == "encrypt_pub_key_t"){
		encrypt_priv_key_t *priv_key =
			(encrypt_priv_key_t*)ptr->get_ptr();
		if(priv_key == nullptr){
			print("can't load private key", P_ERR);
		}
		priv_key->get_encrypt_key(key, encryption_scheme);
	}else{
		print("key ID is not a valid key", P_ERR);
		// redundant
		*key = {};
		*encryption_scheme = ENCRYPT_UNDEFINED;
	}
}

std::vector<uint8_t> encrypt_api::encrypt(std::vector<uint8_t> data,
					  id_t_ key_id){	
	std::vector<uint8_t> retval;
	uint8_t encryption_scheme = ENCRYPT_UNDEFINED;
	std::vector<uint8_t> key;
	encrypt_pull_key_info(key_id,
			      &key,
			      &encryption_scheme);
	switch(encryption_scheme){
	case ENCRYPT_RSA:
		retval = rsa::encrypt(data, key);
	case ENCRYPT_UNDEFINED:
		print("no encryption scheme is set", P_ERR);
		break;
	default:
		print("unknown encryption scheme is set", P_ERR);
		break;
	}
	// encryption scheme is ALWAYS the first byte
	retval.insert(
		retval.begin(),
		&encryption_scheme,
		&encryption_scheme+1);
	return retval;
}

std::vector<uint8_t> encrypt_api::decrypt(std::vector<uint8_t> data,
					  id_t_ key_id){
	
}
