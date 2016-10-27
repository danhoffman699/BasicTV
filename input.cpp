/*
  input.cpp: the general input API. This gets fed information from either the
  IR or networking API on key macros. Modifier keys work on both the IR and
  networking systems. However, a lot of the features on the networking API
  (phone app, if it is made) would have more intuitive displays that downscale
  the menus into modifiers, setting numbers, among other keys. 

  TODO: define behavior for multiple conflicting streams of input
 */
#include "input.h"
#include "ir.h"

void input_loop(){
}

void input_init(){
	function_vector.push_back(input_loop);
}

