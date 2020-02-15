/****************************************************************************
*                                                                           *
* Copyright (c) 2009 Nuvoton Tech. Corp. All rights reserved.               *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   W55FA93_adc.h
*
* VERSION
*   1.0
*
* DESCRIPTION
*   ADC library header file
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*
* HISTORY
*
* REMARK
*   None
****************************************************************************/
#ifndef __W55FA93_ADC__
#define __W55FA93_ADC__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wb_include.h"

typedef void (*PFN_ADC_CALLBACK)(void);	

#define ADC_TS_4WIRE	0

#define ADC_NONBLOCK	0
#define ADC_BLOCK	1

#define ERR_ADC				(0xFFFF0000 | ((ADC_BA>>16) & 0xFF00) |((ADC_BA>>8) & 0xFF))

typedef enum{ 
	eADC_ADC_int = 0,                                           
   	eADC_AUD_int,
	eADC_LVD_int,
	eADC_WT_int   	                                                                                  
}E_ADC_int;

typedef enum{                            
	eADC_TSCREEN_NORMAL = 0,         	  
	eADC_TSCREEN_SEMI,       
	eADC_TSCREEN_AUTO,
	eADC_TSCREEN_TRIG                                                           
}E_ADC_TSC_MODE;  

#define E_DRVADC_INVALID_int		(ERR_ADC | 00)	
#define E_DRVADC_INVALID_CHANNEL	(ERR_ADC | 01)	
#define E_DRVADC_INVALID_TIMING		(ERR_ADC | 02)	
#define E_ADC_INVALID_int			E_DRVADC_INVALID_int
#define E_ADC_INVALID_CHANNEL		E_DRVADC_INVALID_CHANNEL
#define E_ADC_INVALID_TIMING		E_DRVADC_INVALID_TIMING	


extern void adc_init(void);
extern int adc_open(unsigned char type, unsigned short hr, unsigned short vr);
extern int adc_read(unsigned char mode, unsigned short *x, unsigned short *y);	/* Read Touch panel */
extern unsigned int adc_normalread(unsigned int u32Channel, unsigned short * pu16Data);		/* Low battery */
extern void adc_close(void);
extern unsigned int adc_enableInt(E_ADC_int eIntType);
extern unsigned int adc_disableInt(E_ADC_int eIntType);
extern unsigned int adc_installCallback(E_ADC_int eIntType,PFN_ADC_CALLBACK pfnCallback,PFN_ADC_CALLBACK* pfnOldCallback);
extern void adc_setTouchScreen(E_ADC_TSC_MODE eTscMode,unsigned int u32DleayCycle,char bIsPullup,char bMAVFilter);

/***********************************************************************************************/
int audio_Open(E_SYS_SRC_CLK eSrcClock, unsigned int u32ConvClock);
void adc_StartRecord(void);
void adc_StopRecord(void);
short * adc_GetRecordData(void);
void ADC_SetAutoGainTiming(unsigned int u32Period, unsigned int u32Attack, unsigned int u32Recovery, unsigned int u32Hold);
void ADC_GetAutoGainTiming(unsigned int * pu32Period,unsigned int * pu32Attack,unsigned int * pu32Recovery,unsigned int * pu32Hold);
#endif


