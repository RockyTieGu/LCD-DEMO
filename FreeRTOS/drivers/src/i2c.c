/*-----------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                      */
/*                                                                                   */
/* Copyright (c) 2008 by  Nuvoton Technology Corporation                             */
/* All rights reserved                                                               */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
/* File Contents:                                                                    */
/*   i2c.c                                                                           */
/*                                                                                   */
/* This file contains:                                                               */
/*                                                                                   */
/* Project:                                                                          */
/*   1. 2007.07.23 modify for multi-master arbitration lost case.                    */
/*   2. 2008.07.14 Modify to W90P950 patform										 */
/*   3. 2009.03.10 Modify to NUC930 patform										 */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/ 
 

#include <stdio.h>
#include <string.h>
#include "wb_include.h"
#include "w55fa93_i2c.h"
#include "DrvI2CH.h"

/*-----------------------------------------*/
/* marco, type and constant definitions    */
/*-----------------------------------------*/
/*
	Define debug level
*/
//#define I2C_DEBUG
//#define I2C_DEBUG_PRint_LINE
//#define I2C_DEBUG_ENABLE_ENTER_LEAVE
//#define I2C_DEBUG_ENABLE_MSG
//#define I2C_DEBUG_ENABLE_MSG2

#ifdef I2C_DEBUG
#ifdef ECOS
#define PDEBUG			diag_printf
#else
#define PDEBUG			sysprintf
#endif  /* ECOS */
#else
#define PDEBUG(fmt, arg...)
#endif  /* I2C_DEBUG */

#ifdef I2C_DEBUG_PRint_LINE
#define PRN_LINE()				PDEBUG("[%-20s] : %d\n", __FUNCTION__, __LINE__)
#else
#define PRN_LINE()
#endif

#ifdef I2C_DEBUG_ENABLE_ENTER_LEAVE
#define ENTER()					PDEBUG("[%-20s] : Enter...\n", __FUNCTION__)
#define LEAVE()					PDEBUG("[%-20s] : Leave...\n", __FUNCTION__)
#else
#define ENTER()
#define LEAVE()
#endif

#ifdef I2C_DEBUG_ENABLE_MSG
#ifdef ECOS
#define MSG			diag_printf
#else
#define MSG			sysprintf
#endif  /* ECOS */
#else
#define MSG(msg)
#endif  /* I2C_DEBUG_ENABLE_MSG */

#ifdef I2C_DEBUG_ENABLE_MSG2
#ifdef ECOS
#define MSG2			diag_printf
#else
#define MSG2			sysprintf
#endif  /* ECOS */
#else
#define MSG2(...)
#endif  /* I2C_DEBUG_ENABLE_MSG2 */

#define i2cDisable()	outpw(REG_I2C_CSR, 0x00)  /* Disable i2c core and interrupt */
#define i2cEnable()		outpw(REG_I2C_CSR, 0x03)  /* Enable i2c core and interrupt  */
#define i2cIsBusFree() 	((inpw(REG_I2C_SWR) & 0x18) == 0x18 && (inpw(REG_I2C_CSR) & 0x0400) == 0) ? 1 : 0

/*-----------------------------------------*/
/* global file scope (static) variables    */
/*-----------------------------------------*/
typedef struct{
	//int base;		/* i2c bus number */
	int openflag;
	int volatile state;
	int addr;
	unsigned int last_error;
	
	unsigned int subaddr;
	int subaddr_len;

	UCHAR buffer[I2C_MAX_BUF_LEN];
	unsigned int volatile pos, len;

}i2c_dev;

static i2c_dev volatile i2c_device;
static int _i2cSpeed = 100;

#ifdef ECOS
cyg_interrupt  i2c_int;
cyg_handle_t   i2c_int_handle;
#endif

/*-----------------------------------------*/
/* prototypes of static functions          */
/*-----------------------------------------*/
static int _i2cSetSpeed(int sp);
static void _i2cCalcAddr(i2c_dev *dev, int mode);
static void _i2cReset(i2c_dev *dev);


//------------------------- Program -------------------------//
static int _i2cSetSpeed(int sp)  
{
	unsigned int d;
	E_SYS_SRC_CLK eSrcClk;	
	unsigned int u32PllKHz;	
	unsigned int u32SysKHz;
	unsigned int u32CpuKHz;
	unsigned int u32HclkKHz;
	unsigned int u32ApbKHz;	
	
	//if( sp != 100 && sp != 400)
		//return(I2C_ERR_NOTTY);	

	sysGetSystemClock(&eSrcClk,
				 	&u32PllKHz,	
					&u32SysKHz,
					&u32CpuKHz,
					&u32HclkKHz,
					&u32ApbKHz);	
	
	
	d = ((u32ApbKHz)/(sp * 5)) - 1;

	outpw(REG_I2C_DIVIDER, d & 0xffff);

	MSG2("Set Speed = %d\n", sp);

	return 0;
}

static void _i2cCalcAddr(i2c_dev *dev, int mode)
{
	int i;
	unsigned int subaddr;

	subaddr = dev->subaddr;

	dev->buffer[0] = (((dev->addr << 1) & 0xfe) | I2C_WRITE) & 0xff;

	for(i = dev->subaddr_len; i > 0; i--){
		dev->buffer[i] = subaddr & 0xff;
		subaddr >>= 8;
	}

	if(mode == I2C_STATE_READ){
		i = dev->subaddr_len + 1;
		dev->buffer[i] = (((dev->addr << 1) & 0xfe)) | I2C_READ;
	}

	return;		
}

/* init i2c_dev after open */
static void _i2cReset(i2c_dev *dev)  
{
	dev->addr = -1;
	dev->last_error = 0;
	dev->subaddr = 0;
	dev->subaddr_len = 0;

	_i2cSetSpeed(100);
	
	return;	
}

/*-----------------------------------------------------------------------------------*/
/* Function:                                                                         */
/*   i2cOpen                                                                         */
/*                                                                                   */
/* Parameters:                                                                       */
/*  NONE																			 */
/*                                                                                   */                                                                           
/* Returns:                                                                          */
/*   0                 Success.                                                      */
/*   I2C_ERR_BUSY      Interface already opened.                                     */
/*   I2C_ERR_NODEV     Interface number out of range.                                */
/*                                                                                   */
/* Side effects:                                                                     */
/*   None.                                                                           */
/*                                                                                   */
/* Description:                                                                      */
/*   This function reset the i2c interface.                     */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/

int i2cOpen(void)  
{
	i2c_dev *dev;
	
	dev = (i2c_dev *)((unsigned int)&i2c_device);
	
	if( dev->openflag != 0 )		/* a card slot can open only once */
		return(I2C_ERR_BUSY);
			
	/* Import enable two I2C clock together */
	outpw(REG_APBCLK, inpw(REG_APBCLK) |I2C_CKE);
	
	outpw(REG_I2C_CSR, inpw(REG_I2C_CSR) | I2C_EN);
				
	memset(dev, 0, sizeof(i2c_dev));
	
	_i2cReset(dev);
	
	dev->openflag = 1;
	
	return 0;
}

/*-----------------------------------------------------------------------------------*/
/* Function:                                                                         */
/*   i2cClose                                                                        */
/*                                                                                   */
/* Parameters:                                                                       */
/*	NONE																			 */
/*                                                                                   */                                                                           
/* Returns:                                                                          */
/*   0				Success.                                                         */
/*	 SC_ENODEV      Interface number out of range.                                   */
/*                                                                                   */
/* Side effects:                                                                     */
/*   None.                                                                           */
/*                                                                                   */
/* Description:                                                                      */
/*   Disable I2C interrupt. And initialize some parameters.                          */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/

int i2cClose(void)  
{
	i2c_dev *dev;

	outpw(REG_I2C_CSR, inpw(REG_I2C_CSR) & ~I2C_EN);
	outpw(REG_APBCLK, inpw(REG_APBCLK) & ~I2C_CKE);
	dev = (i2c_dev *)( (unsigned int)&i2c_device );	
		
	dev->openflag = 0;		

	return 0;
}

/*-----------------------------------------------------------------------------------*/
/* Function:                                                                         */
/*   i2cRead                                                                         */
/*                                                                                   */
/* Parameters:                                                                       */
/*   buf         Receive buffer pointer.                                             */
/*   len         Receive buffer length.                                              */
/*                                                                                   */
/* Returns:                                                                          */
/*   > 0                      Retrun read length on success.                         */
/*   I2C_ERR_BUSY             Interface busy.                                        */
/*   I2C_ERR_IO               Interface not opened.                                  */
/*   I2C_ERR_NODEV            No such device.                                        */
/*   I2C_ERR_NACK             Slave returns an errorneous ACK.                       */
/*   I2C_ERR_LOSTARBITRATION                                                         */
/*                                                                                   */
/* Side effects:                                                                     */
/*   None.                                                                           */
/*                                                                                   */
/* Description:                                                                      */
/*   Read data from I2C slave.                                                       */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
int i2cRead(unsigned char * buf, unsigned int len)
{
	i2c_dev *dev;	
	int i;	
			
	dev = (i2c_dev *)( (unsigned int)&i2c_device);	
	
	if(dev->openflag == 0)
		return(I2C_ERR_IO);			

	if(len == 0)
		return 0;

	if(!i2cIsBusFree())
		return(I2C_ERR_BUSY);
		
	if(len > I2C_MAX_BUF_LEN - 10)
		len = I2C_MAX_BUF_LEN - 10;

	dev->state = I2C_STATE_READ;
	dev->pos = 1;
	                                            
	dev->len = dev->subaddr_len + 1 + len + 1;  
	dev->last_error = 0;

	_i2cCalcAddr(dev, I2C_STATE_READ);	

	/* Send START & Chip Address & check ACK */
	dev->last_error = DrvI2CH_WriteByte(TRUE, dev->buffer[0], TRUE, FALSE);	
	if(dev->last_error) 
	{
		MSG2("Write uAddr fail\n");	
		goto exit_read;
	}

	/* Send address & check ACK */
	for (i = 1; i < (dev->subaddr_len + 1); i++)
	{
		dev->last_error = DrvI2CH_WriteByte(FALSE, dev->buffer[i], TRUE, FALSE);
		if(dev->last_error) 
		{
			MSG2("Write uRegAddr fail\n");
			goto exit_read;  
		}
	}
	
	/* Send START & chip address with read & check ACK */
	dev->last_error = DrvI2CH_WriteByte(TRUE, dev->buffer[dev->subaddr_len + 1], TRUE, FALSE);	
	if(dev->last_error) 
	{
		MSG2("Write uAddr fail\n");
		goto exit_read;
	}

	/* Read byte & NAK & STOP  */	
	for (i = 0; i < len; i++)
	{		
		if (i == (len -1))
		{
			dev->last_error = DrvI2CH_ReadByte(FALSE, &buf[i], FALSE, TRUE);
			if(!i2cIsBusFree())
			{
				outpw(REG_APBIPRST, inpw(REG_APBIPRST) | I2CRST);	//reset i2c
				outpw(REG_APBIPRST, inpw(REG_APBIPRST) & ~I2CRST);

				outpw(REG_I2C_CSR, inpw(REG_I2C_CSR) | I2C_EN);
				
				MSG2("!!!I2CRST\n");		
				_i2cSetSpeed(_i2cSpeed);
			}
		}
		else
			dev->last_error = DrvI2CH_ReadByte(FALSE, &buf[i], TRUE, FALSE);
		if(dev->last_error) 
		{
			MSG2("Read fail\n");
			goto exit_read;
		}
	}	
	
	return len;

exit_read:

	DrvI2CH_SendStop();
	if(!i2cIsBusFree())
	{
		outpw(REG_APBIPRST, inpw(REG_APBIPRST) | I2CRST);	//reset i2c
		outpw(REG_APBIPRST, inpw(REG_APBIPRST) & ~I2CRST);

		outpw(REG_I2C_CSR, inpw(REG_I2C_CSR) | I2C_EN);
		
		MSG2("!!!I2CRST\n");		
		_i2cSetSpeed(_i2cSpeed);
	}
	return(dev->last_error);
	
}

int i2cRead_OV(unsigned char * buf, unsigned int len)
{
	i2c_dev *dev;	
	int i;	
			
	dev = (i2c_dev *)( (unsigned int)&i2c_device);	
	
	if(dev->openflag == 0)
		return(I2C_ERR_IO);			

	if(len == 0)
		return 0;

	if(!i2cIsBusFree())
		return(I2C_ERR_BUSY);
		
	if(len > I2C_MAX_BUF_LEN - 10)
		len = I2C_MAX_BUF_LEN - 10;

	dev->state = I2C_STATE_READ;
	dev->pos = 1;
	                                            
	dev->len = dev->subaddr_len + 1 + len + 1;  
	dev->last_error = 0;

	_i2cCalcAddr(dev, I2C_STATE_READ);	

	/* Send START & Chip Address & check ACK */
	dev->last_error = DrvI2CH_WriteByte(TRUE, dev->buffer[0], TRUE, FALSE);	
	if(dev->last_error) 
	{
		MSG2("Write uAddr fail\n");	
		goto exit_read_ov;
	}

	/* Send address & check ACK */
	for (i = 1; i < (dev->subaddr_len + 1); i++)
	{
		dev->last_error = DrvI2CH_WriteByte(FALSE, dev->buffer[i], TRUE, TRUE);
		if(dev->last_error) 
		{
			MSG2("Write uRegAddr fail\n");
			goto exit_read_ov;  
		}
	}
	
	/* Send START & chip address with read & check ACK */
	dev->last_error = DrvI2CH_WriteByte(TRUE, dev->buffer[dev->subaddr_len + 1], TRUE, FALSE);	
	if(dev->last_error) 
	{
		MSG2("Write uAddr fail\n");
		goto exit_read_ov;
	}

	/* Read byte & NAK & STOP  */	
	for (i = 0; i < len; i++)
	{		
		if (i == (len -1))
		{
			dev->last_error = DrvI2CH_ReadByte(FALSE, &buf[i], FALSE, TRUE);
			if(!i2cIsBusFree())
			{
				outpw(REG_APBIPRST, inpw(REG_APBIPRST) | I2CRST);	//reset i2c
				outpw(REG_APBIPRST, inpw(REG_APBIPRST) & ~I2CRST);

				outpw(REG_I2C_CSR, inpw(REG_I2C_CSR) | I2C_EN);
				
				MSG2("!!!I2CRST\n");		
				_i2cSetSpeed(_i2cSpeed);
			}
		}
		else
			dev->last_error = DrvI2CH_ReadByte(FALSE, &buf[i], TRUE, FALSE);
		if(dev->last_error) 
		{
			sysprintf("Read fail\n");
			goto exit_read_ov;
		}
	}	
	
	return len;

exit_read_ov:

	DrvI2CH_SendStop();
	if(!i2cIsBusFree())
	{
		outpw(REG_APBIPRST, inpw(REG_APBIPRST) | I2CRST);	//reset i2c
		outpw(REG_APBIPRST, inpw(REG_APBIPRST) & ~I2CRST);

		outpw(REG_I2C_CSR, inpw(REG_I2C_CSR) | I2C_EN);
		
		MSG2("!!!I2CRST\n");		
		_i2cSetSpeed(_i2cSpeed);
	}
	return(dev->last_error);
	
}

/*-----------------------------------------------------------------------------------*/
/* Function:                                                                         */
/*   i2cWrite                                                                        */
/*                                                                                   */
/* Parameters:                                                                       */
/*   buf         Transmit buffer pointer.                                            */
/*   len         Transmit buffer length.                                             */
/*                                                                                   */                                                                           
/* Returns:                                                                          */
/*      > 0                      Return write length on success.                     */
/*      I2C_ERR_BUSY             Interface busy.                                     */
/*      I2C_ERR_IO               Interface not opened.                               */
/*      I2C_ERR_NODEV            No such device.                                     */
/*      I2C_ERR_NACK             Slave returns an errorneous ACK.                    */
/*      I2C_ERR_LOSTARBITRATION                                                      */
/*                                                                                   */
/* Side effects:                                                                     */
/*   None.                                                                           */
/*                                                                                   */
/* Description:                                                                      */
/*   Write data to i2c slave.                                                        */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
int i2cWrite(unsigned char * buf, unsigned int len)
{
	i2c_dev *dev;
	int i;
	
	dev = (i2c_dev *)( (unsigned int)&i2c_device);	
			
	if(dev->openflag == 0)
		return(I2C_ERR_IO);			

	if(len == 0)
		return 0;

	if(!i2cIsBusFree())
		return(I2C_ERR_BUSY);
		
	if(len > I2C_MAX_BUF_LEN - 10)
		len = I2C_MAX_BUF_LEN - 10;

	dev->state = I2C_STATE_WRITE;
	dev->pos = 1;
	dev->len = dev->subaddr_len + 1 + len;
	dev->last_error = 0;

	_i2cCalcAddr(dev, I2C_STATE_WRITE);	

	/* Send START & Chip Address & check ACK */
	dev->last_error = DrvI2CH_WriteByte(TRUE, dev->buffer[0], TRUE, FALSE);
	if(dev->last_error) 
	{
		MSG2("Write uAddr fail\n");
		goto exit_write; 
	}
	
	/* Send address & check ACK */	
	for (i = 1; i < (dev->subaddr_len + 1); i++)
	{
		dev->last_error = DrvI2CH_WriteByte(FALSE, dev->buffer[i], TRUE, FALSE);
		if(dev->last_error) 
		{
			MSG2("Write uRegAddr fail\n");
			goto exit_write;  
		}
	}

	/* Send data & check ACK & STOP */	
	for (i = 0; i < len; i++)
	{		
		if(i == (len -1))
		{
			dev->last_error = DrvI2CH_WriteByte(FALSE, buf[i], TRUE, TRUE);
			if(!i2cIsBusFree())
			{
				outpw(REG_APBIPRST, inpw(REG_APBIPRST) | I2CRST);	//reset i2c
				outpw(REG_APBIPRST, inpw(REG_APBIPRST) & ~I2CRST);

				outpw(REG_I2C_CSR, inpw(REG_I2C_CSR) | I2C_EN);
				
				MSG2("!!!I2CRST\n");		
				_i2cSetSpeed(_i2cSpeed);
			}
		}
		else
			dev->last_error = DrvI2CH_WriteByte(FALSE, buf[i], TRUE, FALSE);
		if(dev->last_error) 
		{
			MSG2("Write uData fail\n");
			goto exit_write;   
		}
	}	
	
	return len;
	
exit_write:

	DrvI2CH_SendStop();
	if(!i2cIsBusFree())
	{
		outpw(REG_APBIPRST, inpw(REG_APBIPRST) | I2CRST);	//reset i2c
		outpw(REG_APBIPRST, inpw(REG_APBIPRST) & ~I2CRST);

		outpw(REG_I2C_CSR, inpw(REG_I2C_CSR) | I2C_EN);
		
		MSG2("!!!I2CRST\n");		
		_i2cSetSpeed(_i2cSpeed);
	}
	return(dev->last_error);
	
}



/*-----------------------------------------------------------------------------------*/
/* Function:                                                                         */
/*   i2cIoctl                                                                        */
/*                                                                                   */
/* Parameters:                                                                       */
/*   cmd               Command.                                                      */
/*   arg0, arg1        Arguments for the command.                                    */   
/*                                                                                   */                                                                           
/* Returns:                                                                          */
/*   0                 Success                                                       */
/*   I2C_ERR_NODEV     Interface number out of range.                                */
/*   I2C_ERR_IO        Card not activated or card removed.                           */
/*   I2C_ERR_NOTTY     Command not support, or parameter error.                      */
/*                                                                                   */
/* Side effects:                                                                     */
/*   None.                                                                           */
/*                                                                                   */
/* Description:                                                                      */
/*   Support some I2C driver commands for application.                               */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/

int i2cIoctl(unsigned int cmd, unsigned int arg0, unsigned int arg1)
{
	i2c_dev *dev;
	
	dev = (i2c_dev *)((unsigned int)&i2c_device);
	
	if(dev->openflag == 0)
		return(I2C_ERR_IO);	

	switch(cmd){
		case I2C_IOC_SET_DEV_ADDRESS:
			dev->addr = arg0;
			MSG2("Address : %02x\n", arg0&0xff);
			break;
			
		case I2C_IOC_SET_SPEED:
			_i2cSpeed = (int)arg0;
			return(_i2cSetSpeed((int)arg0));

		case I2C_IOC_SET_SUB_ADDRESS:

			if(arg1 > 4){
				return(I2C_ERR_NOTTY);
			}

			dev->subaddr = arg0;
			dev->subaddr_len = arg1;
			MSG2("Sub Address = %02x, length = %d\n",arg0, arg1);
						
			break;

		default:
			return(I2C_ERR_NOTTY);
	}

	return (0);	
}

/*-----------------------------------------------------------------------------------*/
/* Function:                                                                         */
/*   i2cExit                                                                         */
/*                                                                                   */
/* Parameters:                                                                       */
/*   None.                                                                           */
/*                                                                                   */                                                                           
/* Returns:                                                                          */
/*   0                 Success.                                                      */
/*                                                                                   */
/* Side effects:                                                                     */
/*   None.                                                                           */
/*                                                                                   */
/* Description:                                                                      */
/*   Do nothing.                                                                     */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/

int i2cExit(void)
{
	return(0);
}

/*-----------------------------------------------------------------------------------*/
/* Function:                                                                         */
/*   i2cInit                                                                         */
/*                                                                                   */
/* Parameters:                                                                       */
/*   None.                                                                           */
/*                                                                                   */                                                                           
/* Returns:                                                                          */
/*   0        Success.                                                               */
/*                                                                                   */
/* Side effects:                                                                     */
/*   None.                                                                           */
/*                                                                                   */
/* Description:                                                                      */
/*   Configure GPIO to I2C mode                                        */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/

int  i2cInit(void)  
{
	/* Configure GPIO pins to I2C mode */	
	outpw(REG_GPBFUN, inpw(REG_GPBFUN) | (MF_GPB13 | MF_GPB14));	//gpiob(13,14)	
	
	outpw(REG_APBIPRST, inpw(REG_APBIPRST) | I2CRST);	//reset i2c
	outpw(REG_APBIPRST, inpw(REG_APBIPRST) & ~I2CRST);	
	
	memset((void *)&i2c_device, 0, sizeof(i2c_device));	
	
	return(0);
}
