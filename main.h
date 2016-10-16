#ifndef MAIN_H
#define MAIN_H
#include "iostream"
#include "vector"
#include "SDL2/SDL.h"
#define STD_ARRAY_LENGTH 65536
#define STD_ARRAY_SIZE STD_ARRAY_LENGTH
#define PGP_PUBKEY_SIZE 8192
extern std::vector<void(*)()> function_vector;
extern int argc;
extern char **argv;
extern bool running;
#endif
