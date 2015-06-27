#include "board.h"
#include "delay.h"

u8 OW_reset(void)
{
  u8 result;
  OW_LOW();                /* master - drive bus low */
  DELAY_US(DELAY_480US);   /* master - wait for 480us (H-480,480,640) */
  OW_HIGH();               /* master - release bus */
  DELAY_US(DELAY_70US);    /* master - wait for 70us (I-63,70,78) */
  result = OW_READ();
  DELAY_US(DELAY_410US);   /* master - wait for 410us (J-410,410,N/A) */
  OW_HIGH();               /* master - release bus */
  return (u8)(result == (u8)0);
}

void OW_write_8(u8 data)
{
  u8 i;
  for(i=0; i<8; i++)
  {
    if(data & 0x01)
    { /* write '1' */
      OW_LOW();               /* master - drive bus low */
      DELAY_US(DELAY_6US);    /* master - wait 6us (A-5,6,15) */
      OW_HIGH();              /* master - release bus */
      DELAY_US(DELAY_64US);   /* master - wait 64us (B-59,64,N/A) */
    }
    else
    { /* write '0' */
      OW_LOW();                /* master - drive bus low */
      DELAY_US(DELAY_60US);    /* master - wait 60us (C-60,60,120) */
      OW_HIGH();               /* master - release bus */
      DELAY_US(DELAY_10US);    /* master - wait 10us (D-8,10,N/A) */
    }
    data >>= 1;
  }
}
u8 OW_read_8(void)
{
  u8 i;
  u8 result = 0;

  for(i=0; i<8; i++)
  {
    result >>= 1;
    OW_LOW();                 /* master - drive bus low */
    DELAY_US(DELAY_6US);      /* master - wait 6us (A-5,6,15) */
    OW_HIGH();                /* master - release bus */
    DELAY_US(DELAY_9US);      /* master - wait 9us (E-5,9,12) */
    if(OW_READ())  result |= 0x80;
    DELAY_US(DELAY_55US);     /* master - wait 55us (F-50,55,N/A) */
  }
  return (result);
}