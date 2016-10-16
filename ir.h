#include "id.h"
#ifndef IR_H
#define IR_H
/*
  ir.h: basic input handling for IR sensor. The part is the TSOP38238
  for the Raspberry Pi (but via GPIO, so anything should work). 

  This implements the NEC protocol and does basic error checking, correcting, 	
  and repeat commands. Pretty much everything it should do
  
  I don't think LIRC is the way to go, especially with the programmable 
  aspect of it. Linux drivers exist that can read GPIO pins at 38kHz, so
  I am planning on using that in conjunction with this.
 */

#include "array"
#include "tuple"
#include "input.h"

/*
  I wish I had a remote with 64K keys...
 */

#define IR_LOOKUP_SIZE 65536
#define IR_BACKLOG_SIZE 65536

struct ir_remote_t{
private:
	/*
	  This contains:
	  The address (16-bit) inside of the NEC standard (8-bit was used with
	  error detection, but that was removed in favor of more space)
	  The command sent over. This varies heavily on the remote used, and is
	  not used directly anywhere else in the program
	  The key as the software sees it. This contains the keys on the remote
	  in a (fairly) easy to read macro setup.
	 */
	std::array<std::tuple<uint16_t, uint8_t, uint64_t>, IR_LOOKUP_SIZE> lookup;
	/*
	  The backlog contains all of the general input API  codes in the order 
	  they were sent (new ones are at the end, so old to new). The only time
	  that there is an interdependency is when the modifier keys is pressed,
	  and then it looks forward one by one until a valid string is typed. 
	  Forward this information into the general input API and have that deal
	  with the actual code being ran.

	  TODO: implement a cancel button for the modifier. I'm having trouble
	  thinking of what that key should be...
	 */
	std::array<uint64_t, IR_BACKLOG_SIZE> backlog;
public:
	data_id_t id;
	/*
	  I can't think of a good reason to get keys out of order
	  Returns the input key
	 */
	uint64_t get_key();
	ir_remote_t();
	~ir_remote_t();
};

void ir_init(); // ir_loop is statically declared

#endif
