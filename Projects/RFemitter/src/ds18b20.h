#ifndef _DS18B20_H_
#define _DS18B20_H_

#include "board.h"

u8 OW_reset(void);
void OW_write(u8);
u8 OW_read(void);
u8 DS18B20_All_init(void);
u8 DS18B20_All_convert(void);
u8 DS18B20_All_Read_Temp(s16*);
u8 DS18B20_Read_Temp(s16*, u8*);
u8 DS18B20_Read_ROM_ID(u8*);
u8 DS18B20_Read_Byte(u8*);

#endif
