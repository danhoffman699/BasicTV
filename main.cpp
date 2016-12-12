#include "main.h"
#include "settings.h"
#include "file.h"
#include "util.h"
#include "rsa.h"
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
#include "input/input.h"
#include "input/input_ir.h"
#include "net/net_proto.h"
#include "net/net.h" // two seperate units (right now)
#include "id/id_api.h"
#include "compress.h"

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

static void test_compressor(){
	std::vector<uint8_t> input_data;
	for(uint64_t i = 0;i < 1024*1024;i++){
		input_data.push_back(i&0xFF);
	}
	std::vector<uint8_t> output_data =
		compressor::from_xz(
			compressor::to_xz(
				input_data,
				0));
	if(std::equal(input_data.begin(), input_data.end(), output_data.begin())){
		print("Compressor works", P_NOTE);
	}else{
		print("input != output", P_ERR);
	}
}

static void test_socket(){
	/*
	  I cannot locally connect to this computer without using another IP
	  address (breaking 4-tuple), so just test this with laptop
	 */
	net_socket_t *test_socket_ = new net_socket_t;
	std::string ip;
	uint16_t port = 0;
	bool recv = false;
	try{
		recv = settings::get_setting(
			"test_recv") == "1";
	}catch(...){}
	if(recv){
		while(true){
			net_proto_loop();
		}
	}else{
		print("IP address to test", P_NOTE);
		std::cin >> ip;
		print("Port to test", P_NOTE);
		std::cin >> port;
		std::pair<std::string, uint16_t> laptop_conn =
			std::make_pair(ip, port);
		test_socket_->connect(laptop_conn);
		test_socket_->send({'A', 'A', 'A', 'A'});
		while(true){
			sleep_ms(1);
		}
	}
}

/*
  BasicTV works best when there are many open TCP connections at one
  time, because there isn't an overhead for connecting to a new client.
  However, consumer grade routers are terrible at this, so this is an
  automatic test to see how much the router can actually handle.

  TODO: check to see if this information makes it to the router, or if
  it is just stuck locally (which makes sense, but defeats the purpose
  of the test).
 */

static void test_socket_array(std::vector<std::pair<uint64_t, uint64_t> > socket_array){
	for(uint64_t i = 0;i < socket_array.size();i++){
		net_socket_t *first =
			PTR_DATA(socket_array[i].first,
				 net_socket_t);
		net_socket_t *second =
			PTR_DATA(socket_array[i].second,
				 net_socket_t);
		if(first == nullptr || second == nullptr){
			P_V(socket_array[i].first, P_SPAM);
			P_V(socket_array[i].second, P_SPAM);
			print("SOCKETS STRUCTS ARE NULL", P_ERR);
		}
		first->send({'a','a','a','a'});
		sleep_ms(1);
		if(second->recv(4, NET_SOCKET_RECV_NO_HANG).size() == 0){
			print("SOCKET HAS CLOSED", P_ERR);
		}else{
			print("received data for socket number " + std::to_string(i), P_NOTE);
		}
	}
}

/*
  This causes SDL2_net to crash when ulimit is changed. You still get a very
  sane number of ~500 before that happens, so I'm fine with this staying
  like it is.
 */

static void test_max_tcp_sockets(){
	print("Local IP address:", P_NOTE);
	std::string ip;
	std::cin >> ip;
	std::vector<std::pair<uint64_t, uint64_t> > socket_pair;
	bool dropped = false;
	net_socket_t *inbound =
		new net_socket_t;
	inbound->connect(
		std::make_pair("",
			       50000)); // accepts connections
	while(!dropped){
		for(uint64_t i = 0;i < 128;i++){
			net_socket_t *first =
				new net_socket_t;
			first->connect(std::make_pair(ip, 50000));
			net_socket_t *second =
				new net_socket_t;
			sleep_ms(1); // probably isn't needed
			TCPsocket tmp_socket =
				SDLNet_TCP_Accept(inbound->get_tcp_socket());
			if(tmp_socket != nullptr){
				second->set_tcp_socket(tmp_socket);
			}else{
				print("unable to receive connection request", P_ERR);
			}
			socket_pair.push_back(
				std::make_pair(
					first->id.get_id(),
					second->id.get_id()));
		}
		test_socket_array(socket_pair);
	}
}

static void test(){}

// TODO: define some ownership, don't actually use this
// in production, but just as a leak checker

int main(int argc_, char **argv_){
	argc = argc_;
	argv = argv_;
	init();
	test_max_tcp_sockets();
	//test_compressor();
	//test_socket();
	while(running){
		tv_loop();
		input_loop();
		net_proto_loop();
		try{
			if(settings::get_setting("slow_iterate") == "1"){
				sleep_ms(1000);
			}
		}catch(...){}
		try{
			if(settings::get_setting("prove_iterate") == "1"){
				std::cout << "iterated" << std::endl;
			}
		}catch(...){}
	}
	close();
	return 0;
}
