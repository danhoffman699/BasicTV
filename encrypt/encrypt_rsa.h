#include "../util.h"
#include "../id/id.h"
#include "../main.h"
#ifndef ENCRYPT_RSA_H
#define ENCRYPT_RSA_H
#include <vector>
/*
  RSA is the standard encryption system for BasicTV. This hasn't been
  implemented yet, and would probably be just a wrapper for OpenSSL or
  something like that. However, the data is stored inside of custom,
  possibly networkable, data types.

  This does raw encryption, all metadata and fancy stuff is done one
  layer above in the encrypt namespace
*/

namespace rsa{
	std::vector<uint8_t> encrypt(std::vector<uint8_t> raw_data,
				     std::vector<uint8_t> key,
				     uint8_t type);
	std::vector<uint8_t> decrypt(std::vector<uint8_t> raw_data,
				     std::vector<uint8_t> key,
				     uint8_t type);
	std::pair<id_t_, id_t_> gen_key_pair(uint64_t bits);
};

#endif
