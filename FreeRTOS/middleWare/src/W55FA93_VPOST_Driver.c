/***************************************************************************
 *                                                                         *
 * Copyright (c) 2007 - 2009 Nuvoton Technology Corp. All rights reserved.*
 *                                                                         *
 ***************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     W55FA93_VPOST_Driver.c
 *
 * VERSION
 *     0.1 
 *
 * DESCRIPTION
 *
 *
 *
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *
 *
 *     
 * HISTORY
 *     2009.03.16		Created by Shu-Ming Fan
 *
 *
 * REMARK
 *     None
 *
 *
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "wb_include.h"
#include "w55fa93_vpost_driver.h"

char vpostClearVABuffer(void);

unsigned int g_nFrameBufferSize = 0;

volatile PFN_DRVVPOST_int_CALLBACK g_pfnVpostCallback[4] = {NULL, NULL, NULL, NULL};

/*---------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------------*/
void vpostVAStartTrigger(void)
{
	outpw(REG_LCM_LCDCCtl,inpw(REG_LCM_LCDCCtl) | LCDCCtl_LCDRUN); //va-enable
	// 2.Disable IP¡¦s clock
//	outpw(REG_CLKMAN, inpw(REG_CLKMAN) & ~(0x40));
}

void vpostVAStopTrigger(void)
{
     outpw(REG_LCM_LCDCCtl,inpw(REG_LCM_LCDCCtl) & ~(LCDCCtl_LCDRUN));//va-disable
     
}

void vpostVAStartTrigger_MPUContinue(void)
{
	outpw(REG_LCM_LCDCCtl,inpw(REG_LCM_LCDCCtl) | LCDCCtl_LCDRUN); //va-enable
	outp32(REG_LCM_MPUCMD, inp32(REG_LCM_MPUCMD) & ~(MPUCMD_CMD_DISn|MPUCMD_MPU_ON|MPUCMD_MPU_CS|MPUCMD_MPU_RWn) );			
	outp32(REG_LCM_MPUCMD, inp32(REG_LCM_MPUCMD) | MPUCMD_DIS_SEL );			// select Continue Mode
	outp32(REG_LCM_MPUCMD, inp32(REG_LCM_MPUCMD) & ~MPUCMD_CMD_DISn );			// turn on Display Mode							
	outp32(REG_LCM_MPUCMD, inp32(REG_LCM_MPUCMD) | MPUCMD_MPU_ON);				// trigger command output		
} 

void vpostVAStartTrigger_MPUSingle(void)
{
	outpw(REG_LCM_LCDCCtl,inpw(REG_LCM_LCDCCtl) | LCDCCtl_LCDRUN); //va-enable
	outp32(REG_LCM_MPUCMD, inp32(REG_LCM_MPUCMD) & ~(MPUCMD_CMD_DISn|MPUCMD_MPU_ON|MPUCMD_MPU_CS|MPUCMD_MPU_RWn) );			
	outp32(REG_LCM_MPUCMD, inp32(REG_LCM_MPUCMD) & ~MPUCMD_DIS_SEL );			// select Continue Mode
	outp32(REG_LCM_MPUCMD, inp32(REG_LCM_MPUCMD) & ~MPUCMD_CMD_DISn );			// turn on Display Mode							
	outp32(REG_LCM_MPUCMD, inp32(REG_LCM_MPUCMD) | MPUCMD_MPU_ON);				// trigger command output		
} 

void vpostVAStopTriggerMPU(void)
{
	outp32(REG_LCM_MPUCMD, (inp32(REG_LCM_MPUCMD) &(~ MPUCMD_CMD_DISn)) | (DRVVPOST_MPU_CMD_MODE << 29) );	// command mode 

} // DrvVPOST_MPU_LCD_Stop_Trigger

/* For align 32 */
static unsigned int shift_pointer(unsigned int ptr, unsigned int align)   
{
	unsigned int alignedPTR;
	unsigned int remain;
	
	//printf("pointer position is %x\n",ptr);
	if( (ptr%align)!=0)
	{
		remain = ptr % align;
		alignedPTR = ptr + (align - remain);
		return alignedPTR;
	}
	return ptr;
}
char vpostAllocVABuffer(PLCDFORMATEX plcdformatex,unsigned int nBytesPixel)
{
	g_nFrameBufferSize = plcdformatex->nFrameBufferSize+256;
    g_VAOrigFrameBuf = (unsigned int *)malloc(g_nFrameBufferSize);
	g_VAFrameBuf = (void*)shift_pointer((unsigned int)g_VAOrigFrameBuf,256);
	if (g_VAFrameBuf == NULL)
		return FALSE;
    
    vpostClearVABuffer();
    
    outpw(REG_LCM_FSADDR, (unsigned int)g_VAFrameBuf);
    outpw(REG_FEADDR, inpw(REG_LCM_FSADDR) + plcdformatex->nFrameBufferSize);
    
    //if (sysCacheState())//set uncache buffer for CPU
    //if (sysGetCacheState())//set uncache buffer for CPU
    //{
        g_VAFrameBuf = (void*)((unsigned int)g_VAFrameBuf | 0x80000000);
    //}
    
	return TRUE;
}
char vpostAllocVABufferFromAP(unsigned int *pFramebuf)
{ 
	g_VAFrameBuf = (void *)pFramebuf;
	g_VAFrameBuf = (void*)((unsigned int)g_VAFrameBuf | 0x80000000);
    outpw(REG_LCM_FSADDR, (unsigned int)pFramebuf);
    
	return TRUE;
}
char vpostClearVABuffer(void)
{
	if (g_VAOrigFrameBuf != NULL)
    {
        memset((void*)((unsigned int)g_VAOrigFrameBuf | 0x80000000),0x00,g_nFrameBufferSize);
        return TRUE;
    }    
    else
        return FALSE;
}

char vpostFreeVABuffer(void)
{
    if (g_VAOrigFrameBuf != NULL)
    {
        free(g_VAOrigFrameBuf);
    }    
    g_VAOrigFrameBuf = NULL;
    g_VAFrameBuf = NULL;
    return TRUE;
}

void vpostSetLCDEnable(char bYUVBL, unsigned char ucVASrcType, char bLCDRun)
{
	outpw(REG_LCM_LCDCCtl, (inpw(REG_LCM_LCDCCtl) & ~(LCDCCtl_YUVBL|LCDCCtl_FBDS|LCDCCtl_LCDRUN))
    | (((bYUVBL & 0x1) << 16)
    | ((ucVASrcType & 0x7) << 1)
    | (bLCDRun ? bLCDRun : 0)));
}

void vpostSetLCDConfig(char bLCDSynTv, unsigned char u8LCDDataSel, unsigned char u8LCDTYPE)
{
	outpw(REG_LCM_LCDCPrm, (inpw(REG_LCM_LCDCPrm) & ~(LCDCPrm_LCDSynTv | LCDCPrm_LCDDataSel | LCDCPrm_LCDTYPE))
    | (bLCDSynTv ? LCDCPrm_LCDSynTv : 0)
    | ((u8LCDDataSel & 0x7) << 2)
    | (u8LCDTYPE & 0x3));
}


// LCM timng sync with TV or not
void vpostsetLCM_TimingType
(
	E_DRVVPOST_TIMING_TYPE eTimingTpye
)
{
	if (eTimingTpye == eDRVVPOST_SYNC_TV)
		outpw(REG_LCM_LCDCPrm, inpw(REG_LCM_LCDCPrm) | LCDCPrm_LCDSynTv);
	else		
		outpw(REG_LCM_LCDCPrm, (inpw(REG_LCM_LCDCPrm) & (~LCDCPrm_LCDSynTv)));
}

void vpostSetLCM_TypeSelect
(
	E_DRVVPOST_LCM_TYPE eType
)
{
	eType &= LCDCPrm_LCDTYPE;
	outpw(REG_LCM_LCDCPrm, (inpw(REG_LCM_LCDCPrm) & (~LCDCPrm_LCDTYPE)) | eType);
}

// only for 8-bit sync LCM 
void vpostSetSerialSyncLCM_Interface
(
	E_DRVVPOST_8BIT_SYNCLCM_intERFACE eInterface
)
{
	eInterface &= (LCDCPrm_LCDDataSel >>2);
	outpw(REG_LCM_LCDCPrm, (inpw(REG_LCM_LCDCPrm) & (~LCDCPrm_LCDDataSel)) | (eInterface <<2));
}

// only for 8-bit sync LCM in RGB Through Mode
void vpostSetSerialSyncLCM_ColorOrder
(
	E_DRVVPOST_SERAIL_SYNCLCM_COLOR_ORDER eEvenLineOrder,
	E_DRVVPOST_SERAIL_SYNCLCM_COLOR_ORDER eOddLineOrder	
)
{
	eEvenLineOrder &= (LCDCPrm_SRGB_EL_SEL >>6);
	eOddLineOrder &= (LCDCPrm_SRGB_OL_SEL >>4);	
	outpw(REG_LCM_LCDCPrm, (inpw(REG_LCM_LCDCPrm) & (~LCDCPrm_SRGB_EL_SEL)) | (eEvenLineOrder <<6));
	outpw(REG_LCM_LCDCPrm, (inpw(REG_LCM_LCDCPrm) & (~LCDCPrm_SRGB_OL_SEL)) | (eOddLineOrder <<4));	
}

// only for 8-bit CCIR656 mode 
void vpostSetSerialSyncLCM_CCIR656ModeSelect
(
	E_DRVVPOST_CCIR656_MODE eMode
)
{
	eMode &= (LCDCCtl_HAW_656 >>30);
	outpw(REG_LCM_LCDCCtl, (inpw(REG_LCM_LCDCCtl) & (~LCDCCtl_HAW_656)) | (eMode <<30));
}

// only for 16/18/24-bit sync LCM 
void vpostSetParalelSyncLCM_Interface
(
	E_DRVVPOST_PARALLEL_SYNCLCM_intERFACE eInterface
)
{
	eInterface &= (LCDCCtl_PRDB_SEL >>20);
	outpw(REG_LCM_LCDCCtl, (inpw(REG_LCM_LCDCCtl) & (~LCDCCtl_PRDB_SEL)) | (eInterface <<20));
}

void vpostSetFrameBuffer_DataType
(
	E_DRVVPOST_FRAME_DATA_TYPE eType
)
{
	eType &= (LCDCCtl_FBDS >>1);
	outpw(REG_LCM_LCDCCtl, (inpw(REG_LCM_LCDCCtl) & (~LCDCCtl_FBDS)) | (eType <<1));	

} //vpostSetFrameBuffer_DataType	

void vpostSetFrameBuffer_BaseAddress
(
	unsigned int u32BufferAddress
)
{
	u32BufferAddress |= 0x80000000;

	outpw(REG_LCM_FSADDR, u32BufferAddress);

} //DrvVPOST_SetFrameBufferAddress	

void vpostSetYUVEndianSelect
(
	E_DRVVPOST_ENDIAN eEndian
)
{
	eEndian &= (LCDCCtl_YUVBL >>16);
	outpw(REG_LCM_LCDCCtl, (inpw(REG_LCM_LCDCCtl) & (~LCDCCtl_YUVBL)) | (eEndian <<16));	

} //vpostSetYUVEndianSelect	

void vpostSetDataBusPin(E_DRVVPOST_DATABUS eDataBus)
{

	if (eDataBus == eDRVVPOST_DATA_8BITS)
	{
		outpw(REG_GPBFUN, inpw(REG_GPBFUN) | 0xC0000000);		// enable LPCLK pin
		outpw(REG_GPCFUN, inpw(REG_GPCFUN) | 0x0000FFFF);		// enable LVDATA[7:0] pins
		outpw(REG_GPDFUN, inpw(REG_GPDFUN) | 0x00FC0000);		// enable HSYNC/VSYNC/VDEN pins	
	}
	else if (eDataBus == eDRVVPOST_DATA_9BITS)
	{
		outpw(REG_GPBFUN, inpw(REG_GPBFUN) | 0xC0000000);		// enable LPCLK pin
		outpw(REG_GPCFUN, inpw(REG_GPCFUN) | 0x0003FFFF);		// enable LVDATA[8:0] pins
		outpw(REG_GPDFUN, inpw(REG_GPDFUN) | 0x00FC0000);		// enable HSYNC/VSYNC/VDEN pins	
	}
	else if (eDataBus == eDRVVPOST_DATA_16BITS)
	{
		outpw(REG_GPBFUN, inpw(REG_GPBFUN) | 0xC0000000);		// enable LPCLK pin
		outpw(REG_GPCFUN, inpw(REG_GPCFUN) | 0xFFFFFFFF);		// enable LVDATA[15:0] pins
		outpw(REG_GPDFUN, inpw(REG_GPDFUN) | 0x00FC0000);		// enable HSYNC/VSYNC/VDEN pins	
	}
	else if (eDataBus == eDRVVPOST_DATA_18BITS)
	{
		outpw(REG_GPBFUN, inpw(REG_GPBFUN) | 0xC0000000);		// enable LPCLK pin
		outpw(REG_GPCFUN, inpw(REG_GPCFUN) | 0xFFFFFFFF);		// enable LVDATA[15:0] pins
		outpw(REG_GPDFUN, inpw(REG_GPDFUN) | 0x00FC0000);		// enable HSYNC/VSYNC/VDEN pins	
		outpw(REG_GPEFUN, inpw(REG_GPEFUN) | 0x0000000F);		// enable LVDATA[17:16] pins		
	}
	else if (eDataBus == eDRVVPOST_DATA_24BITS)
	{
		outpw(REG_GPBFUN, inpw(REG_GPBFUN) | 0xC0000000);		// enable LPCLK pin
		outpw(REG_GPCFUN, inpw(REG_GPCFUN) | 0xFFFFFFFF);		// enable LVDATA[15:0] pins
		outpw(REG_GPDFUN, inpw(REG_GPDFUN) | 0x00FC0000);		// enable HSYNC/VSYNC/VDEN pins	
		outpw(REG_GPEFUN, inpw(REG_GPEFUN) | 0x0000000F);						// enable LVDATA[17:16] pins
		outpw(REG_GPBFUN, (inpw(REG_GPBFUN) & (~0x03FFC000)) | 0x02AA8000);	// enable LVDATA[23:18] pins	
	}
}

// only for Sync LCM timing async with TV 
void vpostSetSyncLCM_HTiming
(
	S_DRVVPOST_SYNCLCM_HTIMING *psHTiming
)
{
#if 1
	psHTiming->u8PulseWidth --;
	psHTiming->u8BackPorch --;
	psHTiming->u8FrontPorch --;
#endif

	outpw(REG_LCM_TCON1, (inpw(REG_LCM_TCON1) & ~(TCON1_HSPW | TCON1_HBPD | TCON1_HFPD))
	    | ((psHTiming->u8PulseWidth & 0xFF) << 16)
    	| ((psHTiming->u8BackPorch & 0xFF) << 8)
	    | (psHTiming->u8FrontPorch & 0xFF));
	    
} // vpostSetSyncLCM_HTiming

// only for Sync LCM timing async with TV 
void vpostSetSyncLCM_VTiming
(
	S_DRVVPOST_SYNCLCM_VTIMING *psVTiming
)
{

#if 1
	psVTiming->u8PulseWidth --;
	psVTiming->u8BackPorch --;
	psVTiming->u8FrontPorch --;
#endif

	outpw(REG_LCM_TCON2, (inpw(REG_LCM_TCON2) & ~(TCON2_VSPW | TCON2_VBPD | TCON2_VFPD))
	    | ((psVTiming->u8PulseWidth & 0xFF) << 16)
	    | ((psVTiming->u8BackPorch & 0xFF) << 8)
	    | (psVTiming->u8FrontPorch & 0xFF));
	
} // vpostSetSyncLCM_VTiming

// only for Sync LCM timing async with TV 
void vpostSetSyncLCM_ImageWindow
(
	S_DRVVPOST_SYNCLCM_WINDOW *psWindow
)
{
	if (psWindow->u16ClockPerLine) psWindow->u16ClockPerLine--;
	
	outpw(REG_LCM_TCON3, ((inpw(REG_LCM_TCON3) & ~(TCON3_PPL | TCON3_LPP))
	    | ((psWindow->u16ClockPerLine & 0xFFFF) << 16)
	    | (psWindow->u16LinePerPanel & 0xFFFF)));
	
	outpw(REG_LCM_TCON4, (inpw(REG_LCM_TCON4) & ~(TCON4_TAPN)) | ((psWindow->u16PixelPerLine & 0x7FF) << 16));

} // vpostSetSyncLCM_ImageWindow


// only for Sync LCM timing async with TV 
void vpostSetSyncLCM_SignalPolarity
(
	S_DRVVPOST_SYNCLCM_POLARITY *psPolarity
)
{
	outpw(REG_LCM_TCON4, ((inpw(REG_LCM_TCON4) & ~(TCON4_VSP | TCON4_HSP | TCON4_DEP | TCON4_PCLKP))
	    | (((psPolarity->bIsVsyncActiveLow ? 0x0 : 0x1) & 0x1) << 3)
	    | (((psPolarity->bIsHsyncActiveLow ? 0x0 : 0x1) & 0x1) << 2)
	    | (((psPolarity->bIsVDenActiveLow ? 0x0 : 0x1) & 0x1) << 1)
	    | (((psPolarity->bIsDClockRisingEdge ? 0x0 : 0x1) & 0x1))));
} // vpostSetSyncLCM_SignalPolarity


void vpostSetTVEnableConfig(unsigned char u8FBSIZE, unsigned char u8LCDSource, unsigned char u8TVSource, char bNotchEnable, 
							char bTvDAC, char bTvInter, char bTvSystem, char bTvEncoderEnable)
{
	outpw(REG_LCM_TVCtl, (inpw(REG_LCM_TVCtl) & ~(TVCtl_FBSIZE | TVCtl_LCDSrc | TVCtl_TvSrc | TVCtl_Tvdac | TVCtl_NotchE | TVCtl_TvInter | TVCtl_TvSys | TVCtl_TvSleep))
    | (u8FBSIZE << 14)
    | (u8LCDSource << 10)
    | (u8TVSource << 8)
    | ((bNotchEnable ? TVCtl_NotchE : (0 << 5)))
    | ((bTvDAC ? TVCtl_Tvdac : (0 << 4)))
    | ((bTvInter ? TVCtl_TvInter : (0 << 3)))
    | ((bTvSystem ? TVCtl_TvSys : (0 << 2)))
    | (bTvEncoderEnable ? TVCtl_TvSleep : 0));
}

void vpostSetLCM_ImageSource
(
	E_DRVVPOST_IMAGE_SOURCE eSource
)
{
	outpw(REG_LCM_TVCtl, (inpw(REG_LCM_TVCtl) & (~TVCtl_LCDSrc)) | (eSource << 10));	
}


void vpostMPULCDWriteAddr16Bit(unsigned short u16AddrIndex)					// LCD register address
{
	int nFlag;

//	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_CMD_DISn );						// turn on Command Mode	
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) & ~(MPUCMD_MPU_ON|MPUCMD_MPU_CS|MPUCMD_WR_RS|MPUCMD_MPU_RWn) );	// CS=0, RS=0	

	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | (DRVVPOST_MPU_CMD_MODE << 29) );	// R/W command/paramter mode
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) & ~MPUCMD_MPU_RWn );					// Write Command/Data Selection			
	outpw(REG_LCM_MPUCMD, (inpw(REG_LCM_MPUCMD) & ~MPUCMD_MPU_CMD) | u16AddrIndex );		// WRITE register address
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_MPU_ON);					// trigger command output
	
	while(inpw(REG_LCM_MPUCMD) & MPUCMD_BUSY);									// wait command to be sent
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) & (~MPUCMD_MPU_ON) );				// reset command ON flag
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_MPU_CS |MPUCMD_WR_RS);		// CS=1, RS=1	
	
	nFlag = 1000;
	while( nFlag--);	//delay for a while on purpose.

	/*	This polling method failed on Ampire.
	    Maybe due to timing mismatch. (too fast for LCD panel??)
	//while(inpw(MPUCMD)&BUSY); 
	*/
    	
} // vpostMPULCDWriteAddr16Bit

void vpostMPULCDWriteData16Bit(unsigned short  u16WriteData)				// LCD register data
{
//	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_CMD_DISn );	// turn on Command Mode	
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) & ~(MPUCMD_MPU_ON|MPUCMD_MPU_CS|MPUCMD_MPU_RWn) );			// CS=0 	

	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_WR_RS );							// RS=1	
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | (DRVVPOST_MPU_CMD_MODE << 29) );	// R/W command/paramter mode
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) & ~MPUCMD_MPU_RWn );					// Write Command/Data Selection			
	outpw(REG_LCM_MPUCMD, (inpw(REG_LCM_MPUCMD) & ~MPUCMD_MPU_CMD) | u16WriteData);	// WRITE register data
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_MPU_ON);					// trigger command output
	
	while(inpw(REG_LCM_MPUCMD) & MPUCMD_BUSY);									// wait command to be sent
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) & (~MPUCMD_MPU_ON) );				// reset command ON flag
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_MPU_CS);					// CS=1
   	
} // vpostMPULCDWriteData16Bit

unsigned short vpostMPULCDReadData16Bit(void)										// LCD register data
{
	unsigned short ReadData=0x00;
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) & ~(MPUCMD_MPU_ON|MPUCMD_MPU_CS|MPUCMD_WR_RS|MPUCMD_MPU_RWn) );	// CS=0, RS=0	

	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_WR_RS );					// RS=1	
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | (DRVVPOST_MPU_CMD_MODE << 29) );	// R/W command/paramter mode
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_MPU_RWn );					// Read Command/Data Selection			
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_MPU_ON);					// trigger command output
	
	while(inpw(REG_LCM_MPUCMD) & MPUCMD_BUSY);											// wait command to be sent
	ReadData = inpw(REG_LCM_MPUCMD) & MPUCMD_MPU_CMD;							// READ register data	
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) & (~MPUCMD_MPU_ON) );				// reset command ON flag
	outpw(REG_LCM_MPUCMD, inpw(REG_LCM_MPUCMD) | MPUCMD_MPU_CS | MPUCMD_WR_RS);		// CS=1, RS=1
    
    return ReadData;
} // vpostMPULCDReadData16Bit

void vpostEnableInt(E_DRVVPOST_int eInt)
{
	outpw(REG_LCM_LCDCInt, (inpw(REG_LCM_LCDCInt) & ~(LCDCInt_MPUCPLINTEN| LCDCInt_TVFIELDINTEN| LCDCInt_VINTEN | LCDCInt_HINTEN)) | 
	((eInt << 16) & (LCDCInt_MPUCPLINTEN| LCDCInt_TVFIELDINTEN | LCDCInt_VINTEN | LCDCInt_HINTEN)));
}

void vpostDisableInt(E_DRVVPOST_int eInt)
{
	outpw(REG_LCM_LCDCInt, (inpw(REG_LCM_LCDCInt) | (LCDCInt_MPUCPLINTEN| LCDCInt_TVFIELDINTEN | LCDCInt_VINTEN | LCDCInt_HINTEN)) & 
	~((eInt << 16) & (LCDCInt_MPUCPLINTEN| LCDCInt_TVFIELDINTEN | LCDCInt_VINTEN | LCDCInt_HINTEN)));
}

void vpostClearInt(E_DRVVPOST_int eInt)
{
	outpw(REG_LCM_LCDCInt, inpw(REG_LCM_LCDCInt) & ~eInt);
}

char vpostIsIntEnabled(E_DRVVPOST_int eInt)
{
	if (inpw(REG_LCM_LCDCInt) & (eInt << 16))
		return TRUE;
		
	else
		return FALSE;		
}

static void vpostISR(void)
{
	unsigned int u32InterruptFlag;	
	
	u32InterruptFlag = inp32(REG_LCM_LCDCInt);
	if (u32InterruptFlag & eDRVVPOST_Hint)
	{
		if (vpostIsIntEnabled(eDRVVPOST_Hint) == TRUE)
		{
			if ( g_pfnVpostCallback[0] != NULL)
				g_pfnVpostCallback[0](0, eDRVVPOST_Hint);
		}
		vpostClearInt(eDRVVPOST_Hint);
	}			
	if (u32InterruptFlag & eDRVVPOST_Vint)
	{
		if (vpostIsIntEnabled(eDRVVPOST_Vint) == TRUE)
		{
			if ( g_pfnVpostCallback[1] != NULL)
				g_pfnVpostCallback[1](0, eDRVVPOST_Vint);
		}
		vpostClearInt(eDRVVPOST_Vint);		
	}			
	if (u32InterruptFlag & eDRVVPOST_TVFIELDint)
	{
		if (vpostIsIntEnabled(eDRVVPOST_TVFIELDint) == TRUE)
		{
			if ( g_pfnVpostCallback[2] != NULL)
				g_pfnVpostCallback[2](0, eDRVVPOST_TVFIELDint);
		}
		vpostClearInt(eDRVVPOST_TVFIELDint);		
	}			
	if (u32InterruptFlag & eDRVVPOST_MPUCPLint)
	{
		if (vpostIsIntEnabled(eDRVVPOST_MPUCPLint) == TRUE)
		{
			if ( g_pfnVpostCallback[3] != NULL)
				g_pfnVpostCallback[3](0, eDRVVPOST_MPUCPLint);
		}
		vpostClearInt(eDRVVPOST_MPUCPLint);		
	}			
}	

int vpostInstallCallBack(
	E_DRVVPOST_int eIntSource,
	PFN_DRVVPOST_int_CALLBACK	pfnCallback,
	PFN_DRVVPOST_int_CALLBACK 	*pfnOldCallback
)
{
	switch(eIntSource)
	{
		case eDRVVPOST_Hint:
			*pfnOldCallback = g_pfnVpostCallback[0];		// return previous installed callback function pointer
			g_pfnVpostCallback[0] = pfnCallback;       		// install current callback function
			break;
			
		case eDRVVPOST_Vint:
			*pfnOldCallback = g_pfnVpostCallback[1];	
			g_pfnVpostCallback[1] = pfnCallback;       
			break;
	
		case eDRVVPOST_TVFIELDint:
			*pfnOldCallback = g_pfnVpostCallback[2];	
			g_pfnVpostCallback[2] = pfnCallback;     
			break;

		case eDRVVPOST_MPUCPLint:
			*pfnOldCallback = g_pfnVpostCallback[3];	 
			g_pfnVpostCallback[3] = pfnCallback;       
			break;
			
		default:
			return 1;
	}

	sysInstallISR(1, IRQ_VPOST, (void *) vpostISR);
	sysEnableInterrupt(IRQ_VPOST);		
	return 0;
}

void vpostSetMPULCM_ImageWindow
(
	volatile S_DRVVPOST_MPULCM_WINDOW *psWindow
)
{
	outp32(REG_LCM_TCON3, ((inp32(REG_LCM_TCON3) & ~(TCON3_PPL | TCON3_LPP))
	    | ((psWindow->u16PixelPerLine & 0xFFFF) << 16)
	    | (psWindow->u16LinePerPanel & 0xFFFF)));
	
	outp32(REG_LCM_TCON4, (inp32(REG_LCM_TCON4) & ~(TCON4_TAPN)) | ((psWindow->u16PixelPerLine & 0x3FF) << 16));
	
} // vpostSetMPULCM_ImageWindow

void vpostSetMPULCM_TimingConfig
(
	volatile S_DRVVPOST_MPULCM_TIMING *psTiming
)
{	
	outp32(REG_LCM_MPUTS, (psTiming->u8CSnF2DCt<<24)
		|(psTiming->u8WRnR2CSnRt<<16)
		|(psTiming->u8WRnLWt<<8)
		|(psTiming->u8CSnF2WRnFt));
	
} // vpostSetMPULCM_TimingConfig

void vpostSetMPULCM_BusModeSelect
(
	E_DRVVPOST_MPULCM_DATABUS eBusMode
)
{	
	outp32(REG_LCM_MPUCMD, (inp32(REG_LCM_MPUCMD) & ~MPUCMD_MPU_SI_SEL)
		|(eBusMode <<16));
	
} // vpostSetMPULCM_BusModeSelect

