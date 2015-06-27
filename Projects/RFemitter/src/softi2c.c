#include "board.h"
#include "delay.h"

#define SDA_HIGH  (SOFTI2C_PORT->ODR |= SOFTI2C_SDA_PIN)
#define SDA_LOW   (SOFTI2C_PORT->ODR &= (u8)(~SOFTI2C_SDA_PIN))
#define SCL_HIGH  (SOFTI2C_PORT->ODR |= SOFTI2C_SCL_PIN)
#define SCL_LOW   (SOFTI2C_PORT->ODR &= (u8)(~SOFTI2C_SCL_PIN))
#define SDA_READ  (SOFTI2C_PORT->IDR & SOFTI2C_SDA_PIN)
#define SCL_READ  (SOFTI2C_PORT->IDR & SOFTI2C_SCL_PIN)

#define READ  (u8)0x01
#define WRITE (u8)0x00

/*
The data on the SDA line must be stable during the HIGH
period of the clock. The HIGH or LOW state of the data line
can only change when the clock signal on the SCL line is
LOW
*/


void I2C_Flush(u8 cycles)
{
  u8 i;
  for(i = 0; i < cycles; i++)
  {
    SCL_LOW;
    DELAY_US(DELAY_6US);  /*2uS*/
    SCL_HIGH;
    DELAY_US(DELAY_6US);  /*2uS*/
  }
}

/*! \brief Writes SCL.
    \param x tristates SCL when x = 1, other wise 0
*/
void write_scl(u8 x)
{
      if(x)
      {
        SCL_HIGH;
	    //check clock stretching
	    while(!SCL_READ);
      }
      else
      {
        SCL_LOW; 
      }
}

/*! \brief Sends start condition
 */
u8 I2C_StartCond(void)
{
  SDA_LOW;
  DELAY_US(I2C_DELAY);
	
  write_scl(0);	
  DELAY_US(I2C_DELAY);
  return 1;  
}

/*! \brief Toggles SCL.
*/
void toggle_scl(void) 
{
  if(SCL_READ)
  {
    SCL_LOW;
  }
  else
  {
    SCL_HIGH;
    while(!SCL_READ);
  }		
}

/*! \brief Writes a byte on I2C.
    \param data Data 
    \return 1 if successful, otherwise 0
 */
u8 I2C_WriteByte(u8 data)
{
  u8 bit;
  for (bit = 0; bit < 8; bit++) 
  {
    if((data & 0x80) != 0)
	{
	  SDA_HIGH;
	}
	else
	{
	  SDA_LOW;
	}
    toggle_scl();//goes high
    DELAY_US(I2C_DELAY);
    toggle_scl();//goes low
    data <<= 1;
    DELAY_US(I2C_DELAY);
  }
  //release SDA
  SDA_HIGH;
  toggle_scl(); //goes high for the 9th clock
  //Check for acknowledgment
  if(SDA_READ)
  {
    return 0;			
  }
  DELAY_US(I2C_DELAY);
  //Pull SCL low
  toggle_scl(); //end of byte with acknowledgment. 
  DELAY_US(I2C_DELAY); 
  return 1;
}

/*! \brief Reads one byte into buffer.
    \param rcvdata Pointer to data buffer
    \param bytes  Number of bytes to read
    \param index Position of the incoming byte in hte receive buffer 
    \return 1 if successful, otherwise 0
 */
u8 I2C_ReadByte(u8* rcvdata, u8 bytes, u8 index)
{
  u8 byte = 0;
  u8 bit = 0;
  //release SDA
  SDA_HIGH;
  for (bit = 0; bit < 8; bit++) 
  {
    toggle_scl();//goes high
    if(SDA_READ)
	{
	  byte |= (u8)((1 << (7- bit)));
	}
    DELAY_US(I2C_DELAY);
    toggle_scl();//goes low
    DELAY_US(I2C_DELAY);
  }
  rcvdata[index] = byte;
  if(index < (bytes-1))
  {
    SDA_LOW;
    toggle_scl(); //goes high for the 9th clock
    DELAY_US(I2C_DELAY);
    //Pull SCL low
    toggle_scl(); //end of byte with acknowledgment. 
    //release SDA
    SDA_HIGH;
    DELAY_US(I2C_DELAY);
  }
  else //send NACK on the last byte
  {
    SDA_HIGH;
    toggle_scl(); //goes high for the 9th clock
    DELAY_US(I2C_DELAY);
    //Pull SCL low
    toggle_scl(); //end of byte with acknowledgment. 
    //release SDA
    DELAY_US(I2C_DELAY);
  }		
  return 1;
}

/*! \brief Writes data from buffer.
    \param indata Pointer to data buffer
    \param bytes  Number of bytes to transfer
	\param slave_adr  Slave address on I2C bus
	\param slave_reg  Slave memory address from which the reading is started
    \return 1 if successful, otherwise 0
 */
u8 I2C_WriteBytes(u8* indata, u8 bytes, u8 slave_adr, u8 slave_reg)
{
	u8 index, ack = 0;
	
	if(!I2C_StartCond())
	{
		return 0;
	}
	if(!I2C_WriteByte((u8)(slave_adr | WRITE)))
	{
		return 0;	
	}
	if(!I2C_WriteByte(slave_reg))
    {
      return 0;	
    }
	for(index = 0; index < bytes; index++)
	{
		 ack = I2C_WriteByte(indata[index]);
		 if(!ack)
		 {
			break;	
         }			
	}
	//put stop here
	write_scl(1);
	DELAY_US(SCL_SDA_DELAY);
	SDA_HIGH;
	return ack;
}

/*! \brief Reads data into buffer.
    \param data Pointer to data buffer
    \param bytes  Number of bytes to read
	\param slave_adr  Slave address on I2C bus
	\param slave_reg  Slave memory address from which the reading is started
    \return 1 if successful, otherwise 0
 */
u8 I2C_ReadBytes(u8* data, u8 bytes, u8 slave_adr, u8 slave_reg)
{
  u8 index, success = 0;
  if(!I2C_StartCond())
  {
    return 0;
  }
  if(!I2C_WriteByte((u8)(slave_adr | WRITE)))
  {
    return 0;	
  }
  if(!I2C_WriteByte(slave_reg))
  {
    return 0;	
  }
  write_scl(1);
	DELAY_US(SCL_SDA_DELAY);
	SDA_HIGH;
  if(!I2C_StartCond())
  {
    return 0;
  }
  if(!I2C_WriteByte((u8)(slave_adr | READ)))
  {
    return 0;	
  }
  for(index = 0; index < bytes; index++)
  {
    success = I2C_ReadByte(data, bytes, index);
    if(!success)
	  {
      break; 
    }
  }
  //put stop here
  write_scl(1);
  DELAY_US(SCL_SDA_DELAY);
  SDA_HIGH;
  return success;
}