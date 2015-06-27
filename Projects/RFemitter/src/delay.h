#ifndef _DELAY_H_
#define _DELAY_H_

#include "board.h"

#define DELAY_480US (u16)1229
#define DELAY_70US  (u16)179
#define DELAY_410US (u16)1050
#define DELAY_6US   (u8)15
#define DELAY_64US  (u16)164
#define DELAY_60US  (u16)154
#define DELAY_10US  (u8)26
//#define DELAY_15US  (u8)80
#define DELAY_9US   (u8)23
#define DELAY_55US  (u16)141
void delay_ms(u16);
void delay_10us(u16);

/* 
  Delay constants for 8MHz SYSCLK
  1us   - 2,56
  10us  - 25,6
  100us - 256
  1ms   - 2560

  Measurements:
  DELAY_US(1)   - 1.12us
  DELAY_US(2)   - 2.23us
  DELAY_US(5)   - 4.86us
  DELAY_US(10)  - 9.702us
  DELAY_US(20)  - 19.41us
  DELAY_US(50)  - 48.14us
  DELAY_US(100) - 95.9us
*/


#define DELAY_US( loops ) _asm("$N: \n decw X \n jrne $L \n nop", (u16)loops);

#endif