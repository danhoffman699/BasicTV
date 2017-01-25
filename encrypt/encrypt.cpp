#include "encrypt.h"

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

std::pair<uint8_t, std::vector<uint8_t> >  encrypt_key_t::get_encrypt_key(){
	return std::make_pair(encryption_scheme, key);
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

id_t_ encrypt_priv_key_t::get_pub_key_id(){
	return pub_key_id;
}

void encrypt_priv_key_t::set_pub_key_id(id_t_ pub_key_id_){
	pub_key_id = pub_key_id_;
}


/*
  This works on private and public keys in the same way. The only reason why
  there are two key types is so every instance of a private key is guaranteed
  to be non-networkable, but exportable (through the initializer)
 */
static void encrypt_pull_key_info(id_t_ id,
				  std::vector<uint8_t> *key,
				  uint8_t *encryption_scheme,
				  uint8_t *key_type){
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
		std::pair<uint8_t, std::vector<uint8_t> > key_data =
			pub_key->get_encrypt_key();
		*encryption_scheme = key_data.first;
		*key = key_data.second;
		*key_type = ENCRYPT_KEY_TYPE_PUB;
 	}else if(ptr->get_type() == "encrypt_priv_key_t"){
		encrypt_priv_key_t *priv_key =
			(encrypt_priv_key_t*)ptr->get_ptr();
		if(priv_key == nullptr){
			print("can't load private key", P_ERR);
		}
		std::pair<uint8_t, std::vector<uint8_t> > key_data =
			priv_key->get_encrypt_key();
		*encryption_scheme = key_data.first;
		*key = key_data.second;
		*key_type = ENCRYPT_KEY_TYPE_PRIV;		
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
	uint8_t key_type = 0;
	encrypt_pull_key_info(key_id,
			      &key,
			      &encryption_scheme,
			      &key_type);
	switch(encryption_scheme){
	case ENCRYPT_RSA:
		retval = rsa::encrypt(data, key, key_type);
		break;
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
	std::vector<uint8_t> retval;
	uint8_t encryption_scheme = ENCRYPT_UNDEFINED;
	std::vector<uint8_t> key;
	uint8_t key_type = 0;
	encrypt_pull_key_info(key_id,
			      &key,
			      &encryption_scheme,
			      &key_type);
	switch(encryption_scheme){
	case ENCRYPT_RSA:
		retval = rsa::decrypt(data, key, key_type);
		break;
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

std::array<uint8_t, 32> encrypt_api::hash::sha256::gen_raw(std::vector<uint8_t> data){
	std::array<uint8_t, 32> retval;
	SHA256_CTX sha256;
	if(SHA256_Init(&sha256) == 0){
		print("can't initialize SHA256_CTX", P_ERR);
	}
	if(SHA256_Update(&sha256, &(data[0]), data.size()) == 0){
		print("can't update SHA256_CTX", P_ERR);
	}
	if(SHA256_Final(&(retval[0]), &sha256) == 0){
		print("can't compute SHA256_CTX", P_ERR);
	}
	return retval;
}

std::string encrypt_api::hash::sha256::gen_str(std::vector<uint8_t> data){
	return gen_str_from_raw(gen_raw(data));
}

std::string encrypt_api::hash::sha256::gen_str_from_raw(std::array<uint8_t, 32> data){
	std::string retval;
	for(uint16_t i = 0;i < 32;i++){
		retval += to_hex(data[i]);
	}
	P_V_S(retval, P_SPAM);
	return retval;
}
