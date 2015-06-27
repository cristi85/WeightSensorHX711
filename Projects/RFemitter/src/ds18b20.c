#include "board.h"
#include "onewire.h"

/* ROM COMMANDS */
#define SEARCH_ROM         (u8)0XF0
#define READ_ROM           (u8)0x33
#define MATCH_ROM          (u8)0x55
#define SKIP_ROM           (u8)0xCC
#define ALARM_SEARCH       (u8)0XEC
/* FUNCTION COMMANDS */
#define CONVERT_T          (u8)0x44
#define WRITE_SCRATCHPAD   (u8)0x4E
#define READ_SCRATCHPAD    (u8)0xBE
#define COPY_SCRATCHPAD    (u8)0X48
#define RECALL_EE          (u8)0XB8
#define READ_POWER_SUPPLY  (u8)0XB4

/*
Function to init all temperature sensors from the 1-wire network with following settings: 12bit temperature resolution (750ms conversion time)
*/
u8 DS18B20_All_init(void)
{
  if(! OW_reset()) return 0;
  OW_write_8(SKIP_ROM);           /* SKIP ROM command - used when only one sensor on bus */
  OW_write_8(WRITE_SCRATCHPAD);   /* WRITE SCRATCHPAD command */
  OW_write_8(0x00);               /* TH register or User Byte 1 */
  OW_write_8(0x00);               /* TL register or User Byte 2 */
  OW_write_8(0x7F);               /* configuration register: 12 bit resolution - 750ms conversion time */
  return 1;
}

/*
Function to issue a CONVERT TEMPERATURE command to all temperature sensors from the 1-wire network
*/
u8 DS18B20_All_convert(void)
{
  if(!OW_reset()) return 0;
  OW_write_8(SKIP_ROM);
  OW_write_8(CONVERT_T);
  return 1;
}

/* 
Function to read temperature when only one temperature sensor device is present in the network
Function doesn't send ROM ID code, it issues SKIP ROM command
If multiple sensors are present in the network data collision will occur
This function is an optimisation when you have a single temperature sensor, no ROM ID needed tot be send therefore faster
Parameters:
  result: pointer to an s16 variable into which the result will be stored (temperature read from the sensor)
 */
u8 DS18B20_All_Read_Temp(s16* result)
{
  s16 tmp;
  if(!OW_reset()) return 0;
  OW_write_8(SKIP_ROM);
  OW_write_8(READ_SCRATCHPAD);
  tmp = OW_read_8();
  tmp |= OW_read_8() << 8;
  *result = tmp;
  return 1;
}

/*
Function to read one byte from temperature sensor scratchpad memory location 0x00
This function is designed to be used in networks with just a single temperature sensor
*/
u8 DS18B20_All_Read_Byte(u8* result)
{
  if(!OW_reset()) return 0;
  OW_write_8(SKIP_ROM);
  OW_write_8(READ_SCRATCHPAD);
  *result = OW_read_8();
  return 1;
}

/*
Function to read temperature from a specific sensor by ROM ID
Function designed to work in an 1-wire network with multiple sensors
*/
u8 DS18B20_Read_Temp(s16* result, u8* ROM_ID)
{
  s16 tmp, i;
  if(!OW_reset()) return 0;
  OW_write_8(MATCH_ROM);
  for(i = 0; i < 8; i++)
    OW_write_8(ROM_ID[i]);
  OW_write_8(READ_SCRATCHPAD);
  tmp = OW_read_8();
  tmp |= OW_read_8() << 8;
  *result = tmp;
  return 1;
}

/*
Function to read ROM ID code from temperature sensor
Parameters:
  ROM_ID: pointer to an 8 element u8 array into which the ROM code will be stored
*/
u8 DS18B20_Read_ROM_ID(u8* ROM_ID)
{
  s16 tmp, i;
  if(!OW_reset()) return 0;
  OW_write_8(READ_ROM);
  for(i = 0; i < 8; i++)
    ROM_ID[i] = OW_read_8();
  return 1;
}

