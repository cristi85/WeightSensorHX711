#ifndef _HX_711_H_
#define _HX_711_H_

#include "board.h"

void HX711_Read(void);
s32 HX711_ReadAvg(void);
u8 HX711_IsNewAvgValReady(void);
s32 HX711_ReadAvgNoNoise(void);

#endif