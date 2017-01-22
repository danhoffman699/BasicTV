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

std::pair<std::vector<uint8_t>, std::vector<uint8_t> > rsa::gen_key_pair(){
}
