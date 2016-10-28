#ifndef TYPES_H
#define TYPES_H
/*
  Don't bother with changing ID over to 192-bit. For now, just assume that there
  is a native 128-bit int (which there ought to be), and just use the first half
  of the MD5 hash and a 64-bit UUID. Anything more would just be wasting space
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

struct uint256_t{
};

struct uint512_t{
};

struct uint1024_t{
};

// I don't think higher values are needed right now

#endif
