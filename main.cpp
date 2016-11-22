#include "main.h"
#include "settings.h"
#include "file.h"
#include "util.h"
#include "rsa.h"
#include "ir.h"
#include "tv/tv.h"
#include "tv/tv_frame_standard.h"
#include "tv/tv_frame_audio.h"
#include "tv/tv_frame_video.h"
#include "tv/tv_window.h"
#include "tv/tv_patch.h"
#include "tv/tv_dev_video.h"
#include "tv/tv_dev_audio.h"
#include "tv/tv_dev.h"
#include "tv/tv_channel.h"
#include "input.h"
#include "net/net_proto.h"
#include "net/net.h" // two seperate units (right now)
#include "id/id_api.h"

/*
  TODO:
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

int argc = 0;
char **argv = nullptr;
bool running = true;

std::array<uint8_t, 4> version = {
	VERSION_MAJOR,
	VERSION_MINOR,
	VERSION_REVISION,
	NETWORK_MAJOR};

/*
  Since this is so early in development, I'm not worried about this
 */

static void init(){
	/*
	  settings_init() only reads from the file, it doesn't do anything
	  critical to setting default values
	*/
	// default port for ID networking
	settings::set_setting("network_port", "58486");
	// enable socks
	settings::set_setting("socks_enable", "false");
	// if SOCKS cannot be set up properly, then terminate
	settings::set_setting("socks_strict", "true");
	// SOCKS proxy ip address in ASCII
	settings::set_setting("socks_proxy_ip", "127.0.0.1");
	// SOCKS proxy port in ASCII
	settings::set_setting("socks_proxy_port", "9050");
	settings_init();
	
	tv_init();
	input_init();
	net_proto_init();
}

static void close(){
	tv_close();
	input_close();
	net_proto_close();
	id_api::destroy_all_data();
}

static void test_socket(){
	/*
	  I cannot locally connect to this computer without using another IP
	  address (breaking 4-tuple), so just test this with laptop
	 */
	net_socket_t *test_socket_ = new net_socket_t;
	std::string ip;
	const uint16_t port = std::stoi(settings::get_setting("network_port"));
	if(search_for_argv("--laptop") != -1){
		// don't do anything, just accept the connection
		// as normal and print the data
	}else{
		print("IP address of laptop to test", P_NOTE);
		std::cin >> ip;
		std::pair<std::string, uint16_t> laptop_conn = std::make_pair(ip, port);
		test_socket_->connect(laptop_conn);
		test_socket_->send({'A', 'A', 'A', 'A'});
	}
}

static void test(){
}

// TODO: define some ownership, don't actually use this
// in production, but just as a leak checker

int main(int argc_, char **argv_){
	argc = argc_;
	argv = argv_;
	init();
	while(running){
		tv_loop();
		input_loop();
		net_proto_loop();
		if(search_for_argv("--slow-iterate") != -1){
			sleep_ms(1000);
		}
		if(search_for_argv("--prove-iterate") != -1){
			std::cout << "iterated" << std::endl;
		}
	}
	close();
	return 0;
}
