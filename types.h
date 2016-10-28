#ifndef TYPES_H
#define TYPES_H
/*
  Standardized interface for integers, signed and unsigned, larger than
  the C++ standard (64-bit). The current use is for easy ID interfacing,
  because the MD5 hash (RSA fingerprint) is stored inside of the ID, and
  functions as proof to the validity of an ID when referenced by another
  (check pgp_cite_t for match, decrypt with public key to prove ownership).

  For simplicity (and abstraction), these types assume the endian-ness of the
  machine they are running on (to prevent a double-conversion).

  The largest value I plan to implement is 192-bit for the ID (fingerprint + 
  UUID).

  The address will be overloaded, so this can be treated as any other integer 
  when being added to the ID array of the 'master' struct

  TODO: check for 64-bit native numbers and emulate them if they aren't present,
  most compilers ought to have them, but everything breaks when they don't
 */

/*
  since I don't want to risk losing an hour of my time, here is a template for
  all of the (needed) operators for one version of the data type. Of course, 
  other versions need to be made (probably with macros) for
  interconnectivity between the different bit lengths

  // assignment
  uint128_t& operator=(uint128_t rhs);
  // basic math
  uint128_t& operator+(uint128_t rhs);
  uint128_t& operator++(int32_t rhs);
  uint128_t& operator+=(uint128_t rhs);
  uint128_t& operator-(uint128_t rhs);
  uint128_t& operator--(int32_t rhs);
  uint128_t& operator-=(uint128_t rhs);
  uint128_t& operator*(uint128_t rhs);
  uint128_t& operator*=(uint128_t rhs);
  uint128_t& operator/(uint128_t rhs);
  uint128_t& operator/=(uint128_t rhs);
  // pointers
  uint128_t& operator&(uint128_t rhs);
  uint128_t& operator&=(uint128_t rhs);	
  // bitwise operators
  uint128_t& operator~();
  uint128_t& operator|(uint128_t rhs);
  uint128_t& operator|=(uint128_t rhs);
  uint128_t& operator<<(uint128_t rhs);
  uint128_t& operator<<=(uint128_t rhs);
  uint128_t& operator>>(uint128_t rhs);
  uint128_t& operator>>=(uint128_t rhs);
  // boolean
  uint128_t& operator&&(uint128_t rhs);
  uint128_t& operator||(uint128_t rhs);
  uint128_t& operator!();
  uint128_t& operator<(uint128_t rhs);
  uint128_t& operator<=(uint128_t rhs);
  uint128_t& operator>(uint128_t rhs);
  uint128_t& operator>=(uint128_t rhs);
  uint128_t& operator==(uint128_t rhs);
  uint128_t& operator!=(uint128_t rhs);
  // direct parsing
  // should this be uint8_t& ?
  uint128_t& operator[](uint128_t rhs);
 */

#define CUSTOM_INT_OVERLOADS(DATA_TYPE)	       \
DATA_TYPE& operator=(DATA_TYPE rhs);	       \
DATA_TYPE& operator+(DATA_TYPE rhs);	       \
DATA_TYPE& operator++(int32_t rhs);	       \
DATA_TYPE& operator+=(DATA_TYPE rhs);	       \
DATA_TYPE& operator-(DATA_TYPE rhs);	       \
DATA_TYPE& operator--(int32_t rhs);	       \
DATA_TYPE& operator-=(DATA_TYPE rhs);	       \
DATA_TYPE& operator*(DATA_TYPE rhs);	       \
DATA_TYPE& operator*=(DATA_TYPE rhs);	       \
DATA_TYPE& operator/(DATA_TYPE rhs);	       \
DATA_TYPE& operator/=(DATA_TYPE rhs);	       \
DATA_TYPE& operator&(DATA_TYPE rhs);	       \
DATA_TYPE& operator&=(DATA_TYPE rhs);	       \
DATA_TYPE& operator~();			       \
DATA_TYPE& operator|(DATA_TYPE rhs);	       \
DATA_TYPE& operator|=(DATA_TYPE rhs);	       \
DATA_TYPE& operator<<(DATA_TYPE rhs);	       \
DATA_TYPE& operator<<=(DATA_TYPE rhs);	       \
DATA_TYPE& operator>>(DATA_TYPE rhs);	       \
DATA_TYPE& operator>>=(DATA_TYPE rhs);	       \
DATA_TYPE& operator&&(DATA_TYPE rhs);	       \
DATA_TYPE& operator||(DATA_TYPE rhs);	       \
DATA_TYPE& operator!();			       \
DATA_TYPE& operator<(DATA_TYPE rhs);	       \
DATA_TYPE& operator<=(DATA_TYPE rhs);	       \
DATA_TYPE& operator>(DATA_TYPE rhs);	       \
DATA_TYPE& operator>=(DATA_TYPE rhs);	       \
DATA_TYPE& operator==(DATA_TYPE rhs);	       \
DATA_TYPE& operator!=(DATA_TYPE rhs);	       \
DATA_TYPE& operator[](DATA_TYPE rhs);	       

struct uint128_t{
private:
	//uint8_t data[128/8];
	std::array<uint8_t, 16> data;
public:
	CUSTOM_INT_OVERLOADS(uint128_t);
};

struct uint192_t{
};

/*
  I came here to define 192-bit length integers for IDs (first major 64 are a
  UUID, other 128 is an RSA fingerprint). I don't see a use case for anything
  beyond that, as they can be built in to more specialized data types
 */

struct uint256_t{
};

struct uint512_t{
};

struct uint1024_t{
};

// I don't think higher values are needed right now, but

#endif
