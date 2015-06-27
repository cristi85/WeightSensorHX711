/**
  ******************************************************************************
  * @file main.c
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "board.h" 
#include "stm8s_it.h"
#include "config.h"
#include "delay.h"
#include "timeout.h"
#include "errors.h"
#include "hd44780.h"
#include "hx711.h"
#include "ds18b20.h"

/* Defines ---------------------------------------------------------*/

/* CONSTANTS ---------------------------------------------------------------*/

/**
  ******************************************************************************
  * @brief Main function.
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  ******************************************************************************
  */
const u8 ROM_ID1[8] = {0x28, 0x16, 0xAE, 0xBF, 0x3, 0x0, 0x0, 0x89};
static u8 RMsgRcvdCnt = 0;
static volatile _Bool FLAG_ds18b20_err = FALSE;
static s16 temperature = 0;
static u8 temp_intreg;
static u8 temp_frac;

void main(void)
{
  Config();

  DS18B20_All_init();
  
  LCD_Initialize();
  LCD_Clear();
  LCD_WriteString("Weight Sensor");
  LCD_Move_Cursor(2, 1);
  LCD_WriteString("Test");
  FLAG_ds18b20_err = DS18B20_All_convert();
  enableInterrupts();
  
  while(1)
  {
    if(!HX711_STATE)  
    {
      // data ready fo retrieve
      HX711_Read();
    }
    if(FLAG_1000ms)
    {
      FLAG_1000ms = FALSE;
      disableInterrupts();
      DS18B20_Read_Temp(&temperature, ROM_ID1);
      FLAG_ds18b20_err = DS18B20_All_convert();    /* issue DS18B20 convert command, to read the results after 1s */
      enableInterrupts();
      temp_intreg = (u8)(temperature>>4);
      temp_frac = (u8)(temperature - (temp_intreg<<4));
      //temp_frac can be [0-15]
      temp_frac *= 10;  //(temp_frac * 10) / 16  Scale to [0-9]
      temp_frac >>= 4;
      if(HX711_IsNewAvgValReady())
      {
        LCD_Clear();
        // display weight
        //LCD_WriteNumber(HX711_ReadAvg());
        //LCD_WriteString("  ");
        LCD_WriteNumber(HX711_ReadAvgNoNoise());
        // display temperature
        LCD_Move_Cursor(2, 1);
        LCD_WriteNumber((s32)temp_intreg);
        LCD_Write('.');
        LCD_WriteNumber((s32)temp_frac);
        LCD_WriteString(" C");
      }
    }
    if(RFbytesReady == TRUE)
    {
      switch(RcvRFmsg.RFmsgmember.RFcmd)
      {
        case 0x0F:
        {
          RMsgRcvdCnt++;
          LCD_Move_Cursor(1, 1);
          LCD_WriteNumber((u32)RMsgRcvdCnt);
          break;
        }
        case 0xF0:
        {
          RMsgRcvdCnt = 0;
          LCD_Clear();
          LCD_Move_Cursor(1, 1);
          LCD_WriteString("0   received");
          
          break;
        }
        default:
        {
          LCD_Clear();
          LCD_WriteNumber((u32)(++RMsgRcvdCnt));
          LCD_Move_Cursor(1, 4);
          LCD_WriteByte(RcvRFmsg.RFmsgmember.RFNodeId, TRUE);
          LCD_Move_Cursor(1, 9);
          LCD_WriteByte(RcvRFmsg.RFmsgmember.RFcmd, TRUE);
          LCD_Move_Cursor(2, 1);
          LCD_WriteByte(RcvRFmsg.RFmsgmember.RFmsgCHKSUM, TRUE); 
          break;
        }
      }
      RFbytesReady = FALSE;
    }
  }
}

/**
  ******************************************************************************
  * @brief Loop for temporisation
  * @par Parameters:
  * None
  * @retval void None
  * @par Required preconditions:
  * None
  ******************************************************************************
  */

/****************** (c) 2008  STMicroelectronics ******************************/

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval
  * None
  */
void assert_failed(u8* file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */
