#include "main.h"
#include "util.h"
#include "id.h"
#include "pgp.h"

pgp_cite_t::pgp_cite_t() : id(this, __FUNCTION__){
	for(uint64_t i = 0;i < PGP_CITE_SIZE;i++){
		id.add_data(&(cite[i][0]), PGP_CITE_STR_SIZE, ID_DATA_NOPGP);
		memset(&(cite[i][0]), 0, PGP_CITE_STR_SIZE);
	}
	id.add_data(&(pgp_pubkey[0]),
		    PGP_PUBKEY_SIZE,
		    ID_DATA_CACHE | ID_DATA_NOPGP);
}

pgp_cite_t::~pgp_cite_t(){
}

void pgp_cite_t::add(std::string url){
	if(url.size() > PGP_CITE_STR_SIZE){
		throw std::runtime_error("URL too long for citation");
	}
	for(uint64_t i = 0;i < PGP_CITE_SIZE;i++){
		if(cite[i][0] == 0){
			memcpy(&(cite[i][0]), url.c_str(), url.size());
		}
	}
}

bool pgp::cmp::greater_than(std::array<uint8_t, PGP_PUBKEY_SIZE> a,
			    std::array<uint8_t, PGP_PUBKEY_SIZE> b){
	for(uint64_t i = PGP_PUBKEY_SIZE;i != ~(uint64_t)0;i--){
		if(a[i] > b[i]){
			return true;
		}
	}
	return false;
}

bool pgp::cmp::less_than(std::array<uint8_t, PGP_PUBKEY_SIZE> a,
			    std::array<uint8_t, PGP_PUBKEY_SIZE> b){
	for(uint64_t i = PGP_PUBKEY_SIZE;i >= ~(uint64_t)0;i--){
		if(a[i] < b[i]){
			return true;
		}
	}
	return false;
}

bool pgp::cmp::equal_to(std::array<uint8_t, PGP_PUBKEY_SIZE> a,
			    std::array<uint8_t, PGP_PUBKEY_SIZE> b){
	for(uint64_t i = PGP_PUBKEY_SIZE;i >= ~(uint64_t)0;i--){
		if(a[i] != b[i]){
			return false;
		}
	}
	return true;
}
