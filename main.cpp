#include "main.h"
#include "settings.h"
#include "file.h"
#include "util.h"
#include "tv.h"

/*
  TODO:
  Get an IR API set up (and a general input API)
  Define the settings and key bindings better
  Write some simple OpenSSL wrappers for PGP encryption/decryption for
  the channel data
  Implement SDL_image in tv.cpp
  Implement SDL_mixer in tv.cpp
  16x2 support? What would I use it for?

  I might be able to piggyback on the RPI_BTC_ATM code for Bitcoin txs,
  but I don't want to force people to run a Bitcoin node on a restrained
  connection (look into libbitcoin, cbitcoin, etc.). I only need a dedicated
  system of transactions for a sponsorship program, so having a QR code show
  up should be enough integration for most people (at the time of writing), 
  not to mention the fact that I don't want people locking away tons of 
  money on inseucre software
 */

std::vector<void(*)()> function_vector;

int argc = 0;
char **argv = nullptr;
bool running = true;

static void init(){
	tv_init();
}

int main(int argc_, char **argv_){
	argc = argc_;
	argv = argv_;
	init();
	while(running){
		for(uint64_t i = 0;i < function_vector.size();i++){
			function_vector[i]();
		}
	}
}
