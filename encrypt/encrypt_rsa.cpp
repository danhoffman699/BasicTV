#include "encrypt.h"
#include "encrypt_rsa.h"

/*
  Since we are dealing with raw numbers, we don't have to abide by base 64
  standards at the lowest level (might want to be converted for importing,
  but that isn't a problem)
 */

/*
  I'm only writing this function because I am not restricted to base64

  Currently, std::vector<uint8_t> is used to store insanely large numbers. I'm
  going to implement functions that do math operations on these vectors, so I
  don't have to define multiple types of different sizes (128, 256, 512, etc).
*/

static void rsa_read_raw_key_data(std::vector<uint8_t> data,
				  std::vector<uint8_t> *e,
				  std::vector<uint8_t> *d,
				  std::vector<uint8_t> *n){
	uint64_t e_len = 0, d_len = 0, n_len = 0;
	memcpy(&e_len, data.data(), 8);
	memcpy(&d_len, data.data()+8, 8);
	memcpy(&n_len, data.data()+16, 8);
	*e = convert::nbo::from(
		std::vector<uint8_t>(
			data.begin()+24,
			data.begin()+24+e_len));
	*d = convert::nbo::from(
		std::vector<uint8_t>(
			data.begin()+24+e_len,
			data.begin()+24+d_len));
	*n = convert::nbo::from(
		std::vector<uint8_t>(
			data.begin()+24+e_len+d_len,
			data.begin()+24+n_len));
}

static std::vector<uint8_t> rsa_write_raw_key_data(std::vector<uint8_t> e,
						   std::vector<uint8_t> d,
						   std::vector<uint8_t> n){
	const uint64_t e_len = NBO_64(e.size());
	const uint64_t d_len = NBO_64(d.size());
	const uint64_t n_len = NBO_64(n.size());
	std::vector<uint8_t> retval;
	retval.insert(
		retval.begin(),
		&e_len,
		&e_len+8);
	retval.insert(
		retval.begin(),
		&d_len,
		&d_len+8);
	retval.insert(
		retval.begin(),
		&n_len,
		&n_len+8);
	return retval;
}

std::vector<uint8_t> rsa::encrypt(std::vector<uint8_t> data,
				  std::vector<uint8_t> key){
	// import data into the OpenSSL datatype, encrypt, and return
	std::vector<uint8_t> e, d, n;
	rsa_read_raw_key_data(data, &e, &d, &n);
	RSA *rsa = RSA_new();
	if(rsa == nullptr){
		print("can't allocate RSA key:"+std::to_string(ERR_get_error()), P_ERR);
	}
	e = convert::nbo::to(e);
	d = convert::nbo::to(d);
	n = convert::nbo::to(n);
	rsa->e = BN_bin2bn(e.data(), e.size(), nullptr);
	rsa->d = BN_bin2bn(e.data(), e.size(), nullptr);
	rsa->n = BN_bin2bn(e.data(), e.size(), nullptr);
	std::vector<uint8_t> retval(RSA_size(rsa), 0);
	uint32_t encrypt_retval = 0;
	if(e.size() == 0 && n.size() == 0){
		encrypt_retval =
			RSA_private_encrypt(
				data.size(),
				data.data(),
				retval.data(),
				rsa,
				RSA_PKCS1_PADDING);
	}else{
		encrypt_retval =
			RSA_public_encrypt(
				data.size(),
				data.data(),
				retval.data(),
				rsa,
				RSA_PKCS1_PADDING);
	}
	if(encrypt_retval != RSA_size(rsa)){
		print("unable to encrypt RSA string:"+std::to_string(ERR_get_error()), P_ERR);
	}
	RSA_free(rsa);
	rsa = nullptr;
	return retval;
}

std::vector<uint8_t> rsa::decrypt(std::vector<uint8_t> data,
				  std::vector<uint8_t> key){
}
