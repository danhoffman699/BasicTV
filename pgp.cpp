#include "main.h"
#include "util.h"
#include "id/id.h"
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

std::array<uint8_t, PGP_PUBKEY_SIZE> pgp_cite_t::get_pgp_pubkey(){
	return pgp_pubkey;
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

bool pgp::cmp::greater_than(uint64_t a, uint64_t b){
	data_id_t *tmp[2] = {{nullptr}};
	tmp[0] = PTR_ID(a, "");
	tmp[1] = PTR_ID(b, "");
	if(tmp[0] == nullptr || tmp[1] == nullptr){
		print("can't compare invalid IDs", P_ERR);
	}
	pgp_cite_t *tmp_[2] = {{nullptr}};
	tmp_[0] = PTR_DATA(tmp[0]->get_pgp_cite_id(), pgp_cite_t);
	tmp_[1] = PTR_DATA(tmp[1]->get_pgp_cite_id(), pgp_cite_t);
	if(tmp_[0] == nullptr || tmp_[1] == nullptr){
		print("can't compare invalid pgp_cite_ids", P_ERR);
	}
	return greater_than(tmp_[0]->get_pgp_pubkey(), tmp_[1]->get_pgp_pubkey());
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

pgp_priv_key_t::pgp_priv_key_t() : id(this, __FUNCTION__){
	// don't bother with adding anything to ID, can't risk
	// private key leakage (especially with current state
	// of the software).
}

pgp_priv_key_t::~pgp_priv_key_t(){
}

