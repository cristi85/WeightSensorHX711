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
static u8 RMsgRcvdCnt = 0;
void main(void)
{
  Config();
  enableInterrupts();

  LCD_Initialize();
  LCD_Clear();
  LCD_WriteString("Weight Sensor");
  LCD_Move_Cursor(2, 1);
  LCD_WriteString("Test");

  while(1)
  {
    if(!HX711_STATE)  
    {
      // data ready fo retrieve
      HX711_Read();
      if(HX711_IsNewAvgValReady())
      {
        LCD_Clear();
        LCD_WriteNumber(HX711_ReadAvg());
        LCD_WriteString("  ");
        LCD_WriteNumber(HX711_ReadAvgNoNoise());
        LCD_Move_Cursor(2, 1);
        LCD_WriteBinary((u16)HX711_ReadAvg());
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
