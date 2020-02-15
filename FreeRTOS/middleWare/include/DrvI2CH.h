/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved. *
 *                                                              *
 ****************************************************************/

#ifndef _DRVI2CH_H_
#define _DRVI2CH_H_

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include "wbtypes.h"
#include "wb_reg.h"

#ifdef  __cplusplus 
extern "C"
{
#endif

#define ERRCODE	unsigned int

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define DRVI2CH_Ack_No	0
#define DRVI2CH_Ack_Have	1

//Error Code Definition
//E_DRVI2CH_WRONG_LENGTH				Invalid sequential transfer number. 
//E_DRVI2CH_ARBIT_LOSE				Arbitration lose
//E_DRVI2CH_TIME_OUT					I2C time out 
//E_DRVI2CH_NACK						Device return NACK

#define E_DRVI2CH_WRONG_LENGTH     I2C_ERR_ID  /* Wrong length */
#define E_DRVI2CH_ARBIT_LOSE        	(I2C_ERR_ID | 1)  /* Arbitration lose */
#define E_DRVI2CH_TIME_OUT         	(I2C_ERR_ID | 2)  /* Time out */
#define E_DRVI2CH_NACK              		(I2C_ERR_ID | 3)  /* Return NACK */


//typedef enum {I2C_ACK = 0x1, I2C_WRITE = 0x2, I2C_READ = 0x4, eDRVI2CH_STOP = 0x8, eDRVI2CH_START = 0x10} I2C_CMD;

typedef enum 
{
	eDRVI2CH_ACK = 1, 
	eDRVI2CH_WRITE = 2,
	eDRVI2CH_READ = 4,	
	eDRVI2CH_STOP = 8,
	eDRVI2CH_START  =0x10
	
} E_DRVI2CH_CMD;


/*---------------------------------------------------------------------------------------------------------*/
/* Interface function declarations                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
//typedef void (*PFN_I2CH_TIMEDELAY)(unsigned int);
typedef int (*PFN_DRVI2CH_int_CALLBACK)(unsigned char*, unsigned int);

ERRCODE 
DrvI2CH_Open(
	unsigned int u32I2cClock
);

void DrvI2CH_Close(void);

void DrvI2CH_ISR(void);

ERRCODE
DrvI2CH_InstallCallBack(
	PFN_DRVI2CH_int_CALLBACK  pfnCallback,
	PFN_DRVI2CH_int_CALLBACK *pfnOldCallback
);

void DrvI2CH_EnableInt(void);

void DrvI2CH_DisableInt(void);

char 
DrvI2CH_IsIntEnabled(void);

char
DrvI2CH_PollInt(void);

void DrvI2CH_ClearInt(void);

void DrvI2CH_InitSDASCK (void);

char 
DrvI2CH_IsBusy(void);

char 
DrvI2CH_IsBusBusy(void);

char 
DrvI2CH_IsArbitLost(void);

ERRCODE 
DrvI2CH_SetBurstCnt(
	unsigned char u8BurstCnt
);

void DrvI2CH_SetTxData(
	unsigned int u32TxData
);

void DrvI2CH_SendCmd(
	E_DRVI2CH_CMD eCmd
);

char 
DrvI2CH_IsACK(void);

unsigned char 
DrvI2CH_GetRxData(void);

//static void DrvI2CH_Delay(
//	unsigned int nCount
//);
	
ERRCODE 
DrvI2CH_WaitReady(void);

ERRCODE 
DrvI2CH_SendStart(void);

ERRCODE 
DrvI2CH_SendStop(void);

ERRCODE 
DrvI2CH_WriteByte(
	char bStart, 
	unsigned char u8Data, 
	char bCheckAck, 
	char bStop
);

ERRCODE 
DrvI2CH_ReadByte(
	char bStart, 
	unsigned char * pu8ReadData, 
	char bSendAck, 
	char bStop
);

unsigned int 
DrvI2CH_GetVersion(void);

#ifdef __cplusplus
}
#endif

#endif	/*_DRVI2CH_H_*/


