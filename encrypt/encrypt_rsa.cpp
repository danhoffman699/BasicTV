#include "encrypt.h"
#include "encrypt_rsa.h"

/*
  DER is the standard for transporting RSA public keys with IDs (fingerprinting
  doesn't offer enough security).

  Private keys are exported in plain text for ease of use

  TODO: allow for importing of encrypted RSA keys
*/

std::vector<uint8_t> rsa::encrypt(std::vector<uint8_t> data,
				  std::vector<uint8_t> key,
				  uint8_t type){
	if(key.size() == 0){
		print("key is blank, can't decode anything", P_ERR);
	}
	RSA *rsa = nullptr;
	const uint8_t *key_start = &(key[0]);
	if(type == ENCRYPT_KEY_TYPE_PUB){
		d2i_RSAPublicKey(&rsa, &key_start, key.size());
	}else if(type == ENCRYPT_KEY_TYPE_PRIV){
		d2i_RSAPrivateKey(&rsa, &key_start, key.size());
	}else{
		print("invalid key type supplied", P_ERR);
	}
	if(rsa == nullptr){
		print("can't allocate RSA key:"+std::to_string(ERR_get_error()), P_ERR);
	}
	std::vector<uint8_t> retval(RSA_size(rsa), 0);
	uint32_t encrypt_retval = 0;
	if(type == ENCRYPT_KEY_TYPE_PRIV){
		encrypt_retval =
			RSA_private_encrypt(
				data.size(),
				data.data(),
				retval.data(),
				rsa,
				RSA_PKCS1_PADDING);
	}else if(type == ENCRYPT_KEY_TYPE_PUB){
		encrypt_retval =
			RSA_public_encrypt(
				data.size(),
				data.data(),
				retval.data(),
				rsa,
				RSA_PKCS1_PADDING);
	}else{
		print("invalid key type supplied", P_ERR);
	}
	if(encrypt_retval != -1){
		print("unable to encrypt RSA string:"+std::to_string(ERR_get_error()), P_ERR);
	}
	RSA_free(rsa);
	rsa = nullptr;
	return retval;
}

std::vector<uint8_t> rsa::decrypt(std::vector<uint8_t> data,
				  std::vector<uint8_t> key,
				  uint8_t type){
	if(key.size() == 0){
		print("key is blank, can't decode anything", P_ERR);
	}
	RSA *rsa = nullptr;
	const uint8_t *key_start = &(key[0]);
	if(type == ENCRYPT_KEY_TYPE_PUB){
		d2i_RSAPublicKey(&rsa, &key_start, key.size());
	}else if(type == ENCRYPT_KEY_TYPE_PRIV){
		d2i_RSAPrivateKey(&rsa, &key_start, key.size());
	}else{
		print("invalid key type supplied", P_ERR);
	}
	if(rsa == nullptr){
		print("can't allocate RSA key:"+std::to_string(ERR_get_error()), P_ERR);
	}
	std::vector<uint8_t> retval(RSA_size(rsa), 0);
	uint32_t encrypt_retval = 0;
	if(type == ENCRYPT_KEY_TYPE_PRIV){
		encrypt_retval =
			RSA_private_decrypt(
				data.size(),
				data.data(),
				retval.data(),
				rsa,
				RSA_PKCS1_PADDING);
	}else if(type == ENCRYPT_KEY_TYPE_PUB){
		encrypt_retval =
			RSA_public_decrypt(
				data.size(),
				data.data(),
				retval.data(),
				rsa,
				RSA_PKCS1_PADDING);
	}else{
		print("invalid key type supplied", P_ERR);
	}
	if(encrypt_retval != -1){
		print("unable to encrypt RSA string:"+std::to_string(ERR_get_error()), P_ERR);
	}
	RSA_free(rsa);
	rsa = nullptr;
	return retval;
}

// TODO: convert this to templates for readability

std::pair<id_t_, id_t_> rsa::gen_key_pair(uint64_t bits){
	encrypt_priv_key_t *priv_key =
		new encrypt_priv_key_t;
	encrypt_pub_key_t *pub_key =
		new encrypt_pub_key_t;
	for(uint32_t i = 0;i < 2*(bits/64);i++){
		uint64_t rand_seed_tmp =
			true_rand(0, ~0);
		RAND_seed(&rand_seed_tmp, 8);
	}
	RSA *rsa_key =
		RSA_generate_key(
			bits,
			65537,
			nullptr,
			nullptr);
	if(rsa_key == nullptr){
		print("can't generate new RSA key:"+std::to_string(ERR_get_error()), P_ERR);
	}
	uint8_t *priv_buf = 0;
	int32_t priv_len =
		i2d_RSAPrivateKey(rsa_key, &priv_buf);
	priv_key->set_encrypt_key(
		std::vector<uint8_t>(
			priv_buf,
			priv_buf+priv_len),
		ENCRYPT_SCHEME_RSA);
	uint8_t *pub_buf = 0;
	int32_t pub_len =
		i2d_RSAPublicKey(rsa_key, &pub_buf);
	pub_key->set_encrypt_key(
		std::vector<uint8_t>(
			pub_buf,
			pub_buf+pub_len),
		ENCRYPT_SCHEME_RSA);
	delete priv_buf;
	priv_buf = nullptr;
	delete pub_buf;
	pub_buf = nullptr;
	RSA_free(rsa_key);
	rsa_key = nullptr;
	return std::make_pair(
		priv_key->id.get_id(),
		pub_key->id.get_id());
}
