#ifndef INPUT_H
#define INPUT_H

/*
  INPUT_KEY_* is reserved from 32 to 127 for ASCII (extended
  ASCII isn't used). Anything above 127 is free game, so space
  isn't an issue.
 */

#define INPUT_KEY_POWER 1
#define INPUT_KEY_MENU 2
#define INPUT_KEY_UP 3
#define INPUT_KEY_DOWN 4
#define INPUT_KEY_LEFT 5
#define INPUT_KEY_RIGHT 6
#define INPUT_KEY_ENTER 7
#define INPUT_KEY_CH_UP 8
#define INPUT_KEY_CH_DOWN 9
#define INPUT_KEY_VOL_UP 10
#define INPUT_KEY_VOL_DOWN 11
#define INPUT_KEY_STAR 12
#define INPUT_KEY_POUND 13
#define INPUT_KEY_MODIFIER 14
#define INPUT_KEY_MODIFIER_CANCEL 15

/*
  ASCII is allowed as input, which is pretty dope. However,
  it doesn't natively work with IR. Create a modifier key that
  maps the numerical input to a number pad keyboard and press
  whatever the modifier cancel button is.
 */

#define INPUT_IS_ASCII(x) (x >= 32 && x <= 126)
#define INPUT_IS_LETTER(x) (x >= 'A' && x <= 'z')
#define INPUT_IS_NUMBER(x) (x >= '0' && x <= '9')

/*
  input.h: reads inputs from respective APIs and adds to a universal
  buffer. Data can not be read out of order.

  TODO: Do more serious work with this input API. I'm not too concerned
  now because the internal networking is more important, but I did what
  I did as a proof of concept and to lay down some docs for the standard
 */

void input_init();

#endif
