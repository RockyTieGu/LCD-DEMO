/*-----------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                       */
/*                                                                                   */
/* Copyright (c) 2008 by Nuvoton Technology Corporation                              */
/* All rights reserved                                                               */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
/****************************************************************************
 * 
 * FILENAME
 *     spiflash.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This file contains SPI flash library APIs.
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     10/12/07      Create Ver 1.0
 *     06/12/09      Add SPI flash
 *
 * REMARK
 *     None
 **************************************************************************/
/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wb_include.h"
#include "w55fa93_spi.h"
#include "w55fa93_edma.h"

int usiCheckBusy()
{
	// check status
	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

	// status command
	outpw(REG_SPI0_TX0, 0x05);
	spiTxLen(0, 0, 8);
	spiActive(0);

	// get status
	while(1)
	{
		outpw(REG_SPI0_TX0, 0xff);
		spiTxLen(0, 0, 8);
		spiActive(0);
		if (((inpw(REG_SPI0_RX0) & 0xff) & 0x01) != 0x01)
			break;
	}

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

	return Successful;
}

short usiReadID()
{
	unsigned short volatile id;

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

	// command 8 bit
	outpw(REG_SPI0_TX0, 0x90);
	spiTxLen(0, 0, 8);
	spiActive(0);

	// address 24 bit
	outpw(REG_SPI0_TX0, 0x000000);
	spiTxLen(0, 0, 24);
	spiActive(0);

	// data 16 bit
	outpw(REG_SPI0_TX0, 0xffff);
	spiTxLen(0, 0, 16);
	spiActive(0);
	id = inpw(REG_SPI0_RX0) & 0xffff;

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

	return id;
}


int usiWriteEnable()
{
	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

	outpw(REG_SPI0_TX0, 0x06);
	spiTxLen(0, 0, 8);
	spiActive(0);

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

	return Successful;
}

int usiWriteDisable()
{
	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

	outpw(REG_SPI0_TX0, 0x04);
	spiTxLen(0, 0, 8);
	spiActive(0);

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

	return Successful;
}

int usiStatusWrite(unsigned char data)
{
	usiWriteEnable();

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

	// status command
	outpw(REG_SPI0_TX0, 0x01);
	spiTxLen(0, 0, 8);
	spiActive(0);

	// write status
	outpw(REG_SPI0_TX0, data);
	spiTxLen(0, 0, 8);
	spiActive(0);

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

	return Successful;
}


/**************************************************/
int spiFlashInit(void)
{
	static char bIsSpiFlashOK = 0;
	int volatile loop;

	if (!bIsSpiFlashOK)
	{
		outpw(REG_APBCLK, inpw(REG_APBCLK) | SPIMS0_CKE);	// turn on SPI clk
		//Reset SPI controller first
		outpw(REG_APBIPRST, inpw(REG_APBIPRST) | SPI0RST);
		outpw(REG_APBIPRST, inpw(REG_APBIPRST) & ~SPI0RST);
		// delay for time
		// Delay
	    for (loop=0; loop<500; loop++);  
		//configure SPI0 interface, Base Address 0xB800C000

		/* apb clock is 48MHz, output clock is 10MHz */
		spiIoctl(0, SPI_SET_CLOCK, 96, 48000);

		//Startup SPI0 multi-function features, chip select using SS0
		outpw(REG_GPDFUN, inpw(REG_GPDFUN) | 0xFF000000);

		outpw(REG_SPI0_SSR, 0x00);		// CS active low
		outpw(REG_SPI0_CNTRL, 0x04);		// Tx: falling edge, Rx: rising edge

		if ((loop=usiReadID()) == -1)
		{
			sysprintf("read id error !! [0x%x]\n", loop&0xffff);
			return -1;
		}

	//	sysprintf("SPI flash id [0x%x]\n", loop&0xffff);
		usiStatusWrite(0x00);	// clear block protect
		// delay for time
		// Delay
	    for (loop=0; loop<0x20000; loop++);  

		bIsSpiFlashOK = 1;
	}	
	return 0;
 }


int spiFlashEraseSector(unsigned int addr, unsigned int secCount)
{
	int volatile i;

	if ((addr % (64*1024)) != 0)
		return -1;

	for (i=0; i<secCount; i++)
	{
		usiWriteEnable();

		outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

		// erase command
		outpw(REG_SPI0_TX0, 0xd8);
		spiTxLen(0, 0, 8);
		spiActive(0);

		// address
		outpw(REG_SPI0_TX0, addr+i*64*1024);
		spiTxLen(0, 0, 24);
		spiActive(0);

		outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

		// check status
		usiCheckBusy();
	}

	return Successful;
}


int spiFlashEraseAll(void)
{
	usiWriteEnable();

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

	outpw(REG_SPI0_TX0, 0xc7);
	spiTxLen(0, 0, 8);
	spiActive(0);

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

	// check status
	usiCheckBusy();

	return Successful;
}


int spiFlashWrite(unsigned int addr, unsigned int len, unsigned int *buf)
{
	int volatile count=0, page, i;
	unsigned int u32Tmp;
	
	count = len / 256;
	if ((len % 256) != 0)
		count++;

	for (i=0; i<count; i++)
	{
		// check data len
		if (len >= 256)
		{
			page = 256;
			len = len - 256;
		}
		else
			page = len;

		usiWriteEnable();

		outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

		// write command
		outpw(REG_SPI0_TX0, 0x02);
		spiTxLen(0, 0, 8);
		spiActive(0);

		// address
		outpw(REG_SPI0_TX0, addr+i*256);
		spiTxLen(0, 0, 24);
		spiActive(0);		
		
		// write data
		while (page > 0)
		{	
			u32Tmp = ((*buf & 0xFF) << 24) | ((*buf & 0xFF00) << 8) | ((*buf & 0xFF0000) >> 8)| ((*buf & 0xFF000000) >> 24);
			outpw(REG_SPI0_TX0, u32Tmp);
			spiTxLen(0, 0, 32);
			spiActive(0);
			buf++;
			page -=4;
		}

		outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

		// check status
		usiCheckBusy();
	}

	return Successful;
}


int spiFlashRead(unsigned int addr, unsigned int len, unsigned int *buf)
{
	int volatile i;
	unsigned int u32Tmp;
	unsigned char *ptr;

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

	// read command
	outpw(REG_SPI0_TX0, 03);
	spiTxLen(0, 0, 8);
	spiActive(0);

	// address
	outpw(REG_SPI0_TX0, addr);
	spiTxLen(0, 0, 24);
	spiActive(0);

	// data	 
	for (i=0; i<len/4; i++)
	{
		outpw(REG_SPI0_TX0, 0xffffffff);
		spiTxLen(0, 0, 32);
		spiActive(0);
		u32Tmp = inp32(REG_SPI0_RX0);
		*buf++ = ((u32Tmp & 0xFF) << 24) | ((u32Tmp & 0xFF00) << 8) | ((u32Tmp & 0xFF0000) >> 8)| ((u32Tmp & 0xFF000000) >> 24);
	}
	
	if(len % 4)
	{
		ptr = (unsigned char *)buf;
		for (i=0; i<(len %4); i++)
		{
			outpb(REG_SPI0_TX0, 0xff);
			spiTxLen(0, 0, 8);
			spiActive(0);
			*ptr++ = inpb(REG_SPI0_RX0);
		}		
	}	

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

	return Successful;
}

int spiFlashWriteByte(unsigned int addr, unsigned int len, unsigned char *buf)
{
	int volatile count=0, page, i;
	unsigned char u8Tmp;
	
	count = len / 256;
	if ((len % 256) != 0)
		count++;

	for (i=0; i<count; i++)
	{
		// check data len
		if (len >= 256)
		{
			page = 256;
			len = len - 256;
		}
		else
			page = len;

		usiWriteEnable();

		outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

		// write command
		outpw(REG_SPI0_TX0, 0x02);
		spiTxLen(0, 0, 8);
		spiActive(0);

		// address
		outpw(REG_SPI0_TX0, addr+i*256);
		spiTxLen(0, 0, 24);
		spiActive(0);		
		
		// write data
		while (page > 0)
		{	
			u8Tmp = *buf;
			outpw(REG_SPI0_TX0, u8Tmp);
			spiTxLen(0, 0, 8);
			spiActive(0);
			buf++;
			page --;
		}

		outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

		// check status
		usiCheckBusy();
	}

	return Successful;
}


int spiFlashReadByte(unsigned int addr, unsigned int len, unsigned char *buf)
{
	int volatile i;
//	unsigned char u8Tmp;
//	unsigned char *ptr;

	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

	// read command
	outpw(REG_SPI0_TX0, 03);
	spiTxLen(0, 0, 8);
	spiActive(0);

	// address
	outpw(REG_SPI0_TX0, addr);
	spiTxLen(0, 0, 24);
	spiActive(0);

	// data	 
	for (i=0; i<len; i++)
	{
		outpw(REG_SPI0_TX0, 0xffffffff);
		spiTxLen(0, 0, 8);
		spiActive(0);
		*buf++ = inp8(REG_SPI0_RX0);
	}
	
	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

	return Successful;
}

//=========================================================
static int g_PdmaCh = 0;
volatile static char g_bPdmaInt = FALSE;

void PdmaCallback_SPI(unsigned int arg)
{ 	
	EDMA_Free(g_PdmaCh);  	
	g_bPdmaInt = TRUE;
}
int initSPIPDMA_Read(unsigned int dest_addr, unsigned int dma_length)
{
	
	g_PdmaCh = PDMA_FindandRequest();
	EDMA_SetAPB(g_PdmaCh,			//int channel, 
						eDRVEDMA_SPIMS0,			//E_DRVEDMA_APB_DEVICE eDevice, 
						eDRVEDMA_READ_APB,		//E_DRVEDMA_APB_RW eRWAPB, 
						eDRVEDMA_WIDTH_32BITS);	//E_DRVEDMA_TRANSFER_WIDTH eTransferWidth	

	EDMA_SetupHandlers(g_PdmaCh, 		//int channel
						eDRVEDMA_BLKD, 			//int interrupt,	
						PdmaCallback_SPI, 				//void (*irq_handler) (void *),
						NULL);					//void *data

	EDMA_SetWrapintType(g_PdmaCh , NULL);								

	EDMA_SetDirection(g_PdmaCh , eDRVEDMA_DIRECTION_FIXED, eDRVEDMA_DIRECTION_INCREMENTED);


	EDMA_SetupSingle(g_PdmaCh,		// int channel, 
						REG_SPI0_RX0,		// unsigned int src_addr,  (ADC data port physical address) 
						dest_addr, //phaddrrecord,		// unsigned int dest_addr,
						dma_length);	// unsigned int dma_length /* Lenth equal 2 half buffer */

	return Successful;
}

int spiFlashPDMARead(unsigned int addr, unsigned int len, unsigned int *buf)
{
	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) | 0x01);	// CS0

	// read command
	outpw(REG_SPI0_TX0, 0x0b);
	spiTxLen(0, 0, 8);
	spiActive(0);

	// address
	outpw(REG_SPI0_TX0, addr);
	spiTxLen(0, 0, 24);
	spiActive(0);

	// dummy byte
	outp32(REG_SPI0_TX0, 0xff);
	spiTxLen(0, 0, 8);
	spiActive(0);

	outpw(REG_SPI0_CNTRL, inpw(REG_SPI0_CNTRL) | BYTE_ENDIN);
	spiTxLen(0, 0, 32);

	initSPIPDMA_Read((unsigned int)buf, len);
	
	EDMA_Trigger(g_PdmaCh);
	outp32(REG_SPI0_EDMA, (inp32(REG_SPI0_EDMA) & ~0x03) | (EDMA_RW | EDMA_GO));	
	outp32(REG_SPI0_CNTRL, inp32(REG_SPI0_CNTRL) |GO_BUSY);	    
	
	while(g_bPdmaInt == FALSE);	
	g_bPdmaInt = FALSE;
	
	outpw(REG_SPI0_SSR, inpw(REG_SPI0_SSR) & 0xfe);	// CS0

	outpw(REG_SPI0_CNTRL, inpw(REG_SPI0_CNTRL) & ~BYTE_ENDIN);
	spiTxLen(0, 0, 8);
	outp32(REG_SPI0_EDMA, inp32(REG_SPI0_EDMA) & ~EDMA_GO);

	return Successful;
}
