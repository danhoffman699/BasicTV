#ifndef MAIN_H
#define MAIN_H
#include "csignal"
#include "iostream"
#include "vector"
#include "SDL2/SDL.h"
#include "array"
#define STD_ARRAY_LENGTH 65536
#define STD_ARRAY_SIZE STD_ARRAY_LENGTH
#define BITCOIN_WALLET_LENGTH 36
extern int argc;
extern char **argv;
extern bool running;
extern std::array<uint8_t, 4> version;


/*
  Each version number is dedicated 1 byte. It should be read by people
  as VERSION_MAJOR.VERSION_MINOR.VERSION_REVISION@vNETWORK_MAJOR

  ex: 0.0.0@v0
 */

// these version numbers can change freely when it comes to networking
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_REVISION 0

/*
  Networking is where things can get a bit complicated. In order
  to preserve all data previously transmitted on the network, any
  future network versions MUST be able to read those bits of data
  and be able to operate "normally" (use all of the data sent over
  properly with no bugs or errors). Because of the encryption, there
  is no need to write information to this older standard (assuming
  that most of the network has updated to a new version of the networking
  standard, which shouldn't happen often).

  Types that don't exist yet can be ditched, and the software should
  continue to function properly in all cases.
 */

// v0 is used only for testing purposes. v0 should always be the most
// cutting edge version, and shouldn't really be used seriously by people
// because of the instability of the system.
#ifndef TESTNET
#define NETWORK_MAJOR 0
#else
#error "there is no mainnet version yet. compile with -DTESTNET"
#endif
#endif
