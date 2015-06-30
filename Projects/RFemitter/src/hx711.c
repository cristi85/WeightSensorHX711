#include "board.h"
#include "delay.h"
#include "hx711.h"

#define NUM_SAMPLES_AVERAGE     (u8)10
#define NUM_LSB_BITS_TO_DISCARD (u8)6
#define NUM_SAMPLES_TO_DISCARD  (u8)10

static s32 hx711_raw_value;
static s32 hx711_avg_value;
static s32 hx711_avg_value_no_noise;
static s32 hx711_avg_acc = 0;
static s16 hx711_grams;
static u8 num_samples_cnt = 0;
static u8 num_samples_to_discard_cnt = 0;
static u8 flag_new_avg_val_ready = FALSE;

void HX711_Read()
{
  u8 i;
  u32 mask = 0x00800000;
  hx711_raw_value = 0;
  //DELAY_US(5);
  for(i=0; i<25;i++)
  {
    HX711_SCK(1);
    DELAY_US(10);
    if(HX711_STATE)
    {
      if(i==0)
      {
        // sign bit is "1" extend sign bit from 24bit to 32bit
        hx711_raw_value |= 0xFF800000;
      }
      else hx711_raw_value |= mask;
    }
    HX711_SCK(0);
    DELAY_US(8);
    mask >>= 1;
  }
  if(num_samples_to_discard_cnt < NUM_SAMPLES_TO_DISCARD)
  {
    num_samples_to_discard_cnt++;
  }
  else
  {
    hx711_avg_acc += hx711_raw_value;
    num_samples_cnt++;
    if(num_samples_cnt == NUM_SAMPLES_AVERAGE)
    {
      hx711_avg_value = hx711_avg_acc / NUM_SAMPLES_AVERAGE;
      hx711_avg_value_no_noise = hx711_avg_value >> NUM_LSB_BITS_TO_DISCARD;
      hx711_avg_acc = 0;
      num_samples_cnt = 0;
      flag_new_avg_val_ready = TRUE;
    }
  }
}

s32 HX711_ReadAvg()
{
  flag_new_avg_val_ready = FALSE;
  return hx711_avg_value;
}

s32 HX711_ReadAvgNoNoise()
{
  flag_new_avg_val_ready = FALSE;
  return hx711_avg_value_no_noise;
}

u8 HX711_IsNewAvgValReady()
{
  return flag_new_avg_val_ready;
}

void HX711_SetZero()
{
  
}

void HX711_CalibrationPoint1()
{
  
}

void HX711_CalibrationPoint2()
{
  
}