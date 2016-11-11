#ifndef UTIL_H
#define UTIL_H
#define PRINT_SPAM 0
#define PRINT_DEBUG 1
#define PRINT_NOTICE 2
#define PRINT_WARNING 3
#define PRINT_ERROR 4
#define PRINT_CRITICAL 5
#define P_SPAM PRINT_SPAM
#define P_DEBUG PRINT_DEBUG
#define P_NOTICE PRINT_NOTICE
#define P_NOTE PRINT_NOTICE
#define P_WARN PRINT_WARNING
#define P_ERR PRINT_ERROR
#define P_CRIT PRINT_CRITICAL
#include "exception"
#include "cstring"
#include "mutex"
#include "iostream"
#include "execinfo.h"
#include "sstream"
#include "vector"
#include "random"

// branch prediction
#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

extern void sleep_ms(int ms, bool force = false);
extern int search_for_argv(std::string);
extern std::string get_argv(int a);
extern void print(std::string data, int level, const char *func = nullptr);
extern long double get_mul_to_btc(std::string currency);
extern long double get_btc_rate(std::string currency);
extern std::vector<std::string> newline_to_vector(std::string data);
//128-bit GCC?
extern uint64_t true_rand(uint64_t min, uint64_t max);

#define HANG() while(true){sleep_ms(1000);}
#define LOCK_RUN(a, b)							\
	a.lock();							\
	if(search_for_argv("--spam") != -1){ \
		std::cout << "[SPAM] " << #a << " lock locked for " #b << std::endl; \
	}								\
	try{								\
		b;							\
	}catch(std::exception e){					\
		std::cerr << "Caught " << e.what() << " in LOCK_RUN" << std::endl; \
		a.unlock();						\
		throw e;						\
	}								\
	a.unlock();							\
	if(search_for_argv("--spam") != -1){ \
		std::cout << "[SPAM] " << #a << " lock unlocked" << std::endl; \
	}

#define PRINT(a, b) print(a, b, __PRETTY_FUNCTION__);

// pre-programmed responses, going to enforce 80-col more

namespace pre_pro{
	void unable(std::string from, std::string to, int level);
	void exception(std::exception e, std::string for_, int level);
};

namespace system_handler{
	std::string cmd_output(std::string cmd);
	int run(std::string str);
	void write(std::string cmd, std::string file);
};

/*
  Since networking std::vectors takes some extra code and complexity, 
  std::arrays are used with some helper functions to emulate std::vector
  (push_back being the big one)
 */

namespace array_func{
	uint64_t add(void *array,
			   uint64_t array_size,
			   void *data,
			   uint64_t data_size);
	uint64_t size(void *array,
		      uint64_t array_size,
		      uint64_t data_size);
};

uint64_t flip_bit_section(uint8_t begin, uint8_t end);

#define CONTINUE_IF_NULL(x) if(unlikely(x == nullptr)){continue;}
#define CONTINUE_IF_TRUE(x) if((unlikely(!!x)){continue;}

// print var
#define P_V(a, b) print((std::string)__PRETTY_FUNCTION__ + ":" + (std::string)#a + " == '" + std::to_string(a) + "'", b)
// print var string
#define P_V_S(a, b) print((std::string)__PRETTY_FUNCTION__ + ":" + (std::string)#a + " == '" + a + "'", b)
// print var char
#define P_V_C(a, b) print((std::string)__PRETTY_FUNCTION__ + ":" + (std::string)#a + " == '" + std::string(&a, 1) + "'", b)
// cannot use print here
// print var as binary
#define P_V_B(a, b) print(convert::number::to_binary(a), b);

#ifdef SPAM_OUTPUT
#define DEBUG_OUTPUT 1
#define P_SPAM_(a) print((std::string)a, P_SPAM);
#else
#define P_SPAM_(a) ;
#endif

#ifdef DEBUG_OUTPUT
#define P_DEBUG_(a) print((std::string)a, P_DEBUG);
#else
#define P_DEBUG_(a) ;
#endif

#define FIND_FIRST_ZERO(A, B, C) for(uint64_t A = 0; A < C;A++){if(B[A] == 0){break;}

uint64_t get_time_microseconds();

#endif
