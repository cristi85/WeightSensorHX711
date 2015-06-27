#ifndef _ONEWIRE_H_
#define _ONEWIRE_H_

#include "board.h"

u8 OW_reset(void);
void OW_write_8(u8);
u8 OW_read_8(void);

#endif