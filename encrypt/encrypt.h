#ifndef ENCRYPT_H
#define ENCRYPT_H
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

namespace encrypt_api{
	
};

#endif
