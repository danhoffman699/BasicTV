#ifndef IR_H
#define IR_H
/*
  ir.h: basic input handling for IR sensor. The part is the TSOP38238
  for the Raspberry Pi (but via GPIO, so anything should work). 

  Modifier key combinations are used extensively for input. The only
  purpose of this file is to interface with LIRC to extract keycodes. 
  This file is referenced by input.cpp & input.h (not created yet) and
  will extract the key combinations from there.

  This implements the NEC protocol (8-bit command length) and does basic
  error checking, correcting, and repeat commands

  This is the point where I gave up on Windows support, because LIRC seems
  like the best way forward for this.
 */

#define IR_COMMAND_BUFFER_SIZE 32

struct ir_remote_t{
private:
	std::array<std::array<uint8_t, 8>, IR_COMMAND_BUFFER_SIZE> buffer;
public:
};

#endif
