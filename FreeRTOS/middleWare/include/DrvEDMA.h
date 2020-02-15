/****************************************************************
 *                                                             *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved. *
 *                                                              *
 ****************************************************************/
 /****************************************************************
 Total:
 	
 Revision History:
 	05/26/2009, first creation
 ****************************************************************/
 
#ifndef __DRVEDMA_H__
#define __DRVEDMA_H__

#include "wbio.h"
#include "wb_reg.h"

#ifdef  __cplusplus
extern "C"
{
#endif

#define	EE_SUCCESS	0

#define DESTINATION_DIRECTION_BIT   6
#define SOURCE_DIRECTION_BIT        4
#define TRANSFER_WIDTH_BIT          19
#define MODE_SELECT_BIT             2

#define MAX_TRANSFER_BYTE_COUNT     0x00FFFFFF
#define MAX_CHANNEL_NUM   4

#define ERRCODE		 	  unsigned int
#define 	ERR_EDMA				(0xFFFF0000 | ((EDMA_BA>>16) & 0xFF00) |((EDMA_BA>>8) & 0xFF))

#define E_DRVEDMA_FALSE_INPUT      	(ERR_EDMA | 00)

// For interrupt CallBack Function
typedef void (*PFN_DRVEDMA_CALLBACK)(unsigned int);

/****************************************************************
	Enumerate Type
 ****************************************************************/
typedef enum
{
	eDRVEDMA_DISABLE =0,
	eDRVEDMA_ENABLE
}E_DRVEDMA_OPERATION;

typedef enum
{
	eDRVEDMA_CHANNEL_0 =0,
	eDRVEDMA_CHANNEL_1,
	eDRVEDMA_CHANNEL_2,
	eDRVEDMA_CHANNEL_3,
	eDRVEDMA_CHANNEL_4	
}E_DRVEDMA_CHANNEL_INDEX;

typedef enum
{
	eDRVEDMA_TARGET_SOURCE =0,
	eDRVEDMA_TARGET_DESTINATION
}E_DRVEDMA_TARGET;


typedef enum
{
	eDRVEDMA_WRAPAROUND_NO_int =0,    
	eDRVEDMA_WRAPAROUND_EMPTY,	            // empty
	eDRVEDMA_WRAPAROUND_THREE_FOURTHS,	    // 3/4 
	eDRVEDMA_WRAPAROUND_HALF=4,             // 1/2 
	eDRVEDMA_WRAPAROUND_QUARTER=8           // 1/4 
}E_DRVEDMA_WRAPAROUND_SELECT;

typedef enum
{
	eDRVEDMA_TABORT_FLAG =1,
	eDRVEDMA_BLKD_FLAG,
	eDRVEDMA_SG_FLAG=8,	
	eDRVEDMA_WRA_EMPTY_FLAG=0x100,	          // empty	    
	eDRVEDMA_WRA_THREE_FOURTHS_FLAG=0x200,	  // 3/4 
	eDRVEDMA_WRA_HALF_FLAG=0x400,             // 1/2 
	eDRVEDMA_WRA_QUARTER_FLAG=0x800           // 1/4 
}E_DRVEDMA_int_FLAG;

typedef enum
{
	eDRVEDMA_DIRECTION_INCREMENTED =0,
	eDRVEDMA_DIRECTION_DECREMENTED,
	eDRVEDMA_DIRECTION_FIXED,
	eDRVEDMA_DIRECTION_WRAPAROUND
}E_DRVEDMA_DIRECTION_SELECT;

typedef enum
{
	eDRVEDMA_WIDTH_32BITS=0,    
	eDRVEDMA_WIDTH_8BITS,
	eDRVEDMA_WIDTH_16BITS
}E_DRVEDMA_TRANSFER_WIDTH;

typedef enum
{
	eDRVEDMA_TABORT =1,
	eDRVEDMA_BLKD,
	eDRVEDMA_WAR=4,			
	eDRVEDMA_SG=8		
}E_DRVEDMA_int_ENABLE;

typedef enum
{
	eDRVEDMA_RGB888=1,    
	eDRVEDMA_RGB555=2,  
	eDRVEDMA_RGB565=4,	  
	eDRVEDMA_YCbCr422=8
}E_DRVEDMA_COLOR_FORMAT;

typedef enum
{
	eDRVEDMA_SPIMS0=0,    
	eDRVEDMA_SPIMS1,  
	eDRVEDMA_UART0,	  
	eDRVEDMA_UART1,
	eDRVEDMA_ADC
}E_DRVEDMA_APB_DEVICE;

typedef enum
{
	eDRVEDMA_READ_APB=0,    
	eDRVEDMA_WRITE_APB  
}E_DRVEDMA_APB_RW;

typedef enum
{
	eDRVEDMA_MEMORY_TO_MEMORY =0,
	eDRVEDMA_DEVICE_TO_MEMORY,
	eDRVEDMA_MEMORY_TO_DEVICE
}E_DRVEDMA_MODE_SELECT;

typedef struct {
    unsigned int u32Addr;
    E_DRVEDMA_DIRECTION_SELECT eAddrDirection;
}S_DRVEDMA_CH_ADDR_SETTING;

typedef struct {
    unsigned int u32SourceAddr;
    E_DRVEDMA_DIRECTION_SELECT eSrcDirection;    
    unsigned int u32DestAddr;
    E_DRVEDMA_DIRECTION_SELECT eDestDirection;     
    unsigned int u32TransferByteCount;
    unsigned int u32Stride;
    unsigned int u32SrcOffset;
    unsigned int u32DestOffset;    
}S_DRVEDMA_DESCRIPT_SETTING;

typedef struct {
    unsigned int u32SourceAddr;
    unsigned int u32DestAddr;
    unsigned int u32StrideAndByteCount;
    unsigned int u32Offset;
    unsigned int u32NextSGTblAddr;
}S_DRVEDMA_DESCRIPT_FORMAT;

/****************************************************************
// APIs declaration
 ****************************************************************/
unsigned int  
DrvEDMA_Open(void);

void DrvEDMA_Close(void);

char  
DrvEDMA_IsCHBusy(
	E_DRVEDMA_CHANNEL_INDEX eChannel
);

void DrvEDMA_EnableCH(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	E_DRVEDMA_OPERATION eOP
);

char  
DrvEDMA_IsEnabledCH(
	E_DRVEDMA_CHANNEL_INDEX eChannel
);

ERRCODE  
DrvEDMA_SetTransferSetting(
	E_DRVEDMA_CHANNEL_INDEX eChannel, 
	S_DRVEDMA_CH_ADDR_SETTING* psSrcAddr, 
	S_DRVEDMA_CH_ADDR_SETTING* psDestAddr, 
	unsigned int u32TransferLength
);

ERRCODE  
DrvEDMA_GetTransferSetting(
	E_DRVEDMA_CHANNEL_INDEX eChannel, 
	E_DRVEDMA_TARGET eTarget, 
	unsigned int* pu32Addr, 
	E_DRVEDMA_DIRECTION_SELECT* peDirection
);

ERRCODE  
DrvEDMA_GetTransferLength(
	E_DRVEDMA_CHANNEL_INDEX eChannel, 
	unsigned int* pu32TransferLength
);

ERRCODE  
DrvEDMA_SetAPBTransferWidth(
	E_DRVEDMA_CHANNEL_INDEX eChannel, 
	E_DRVEDMA_TRANSFER_WIDTH eTransferWidth
);

ERRCODE  
DrvEDMA_GetAPBTransferWidth(
	E_DRVEDMA_CHANNEL_INDEX eChannel, 
	E_DRVEDMA_TRANSFER_WIDTH* peTransferWidth
);

ERRCODE  
DrvEDMA_SetCHForAPBDevice(
    E_DRVEDMA_CHANNEL_INDEX eChannel, 
    E_DRVEDMA_APB_DEVICE eDevice,
    E_DRVEDMA_APB_RW eRWAPB    
);

E_DRVEDMA_CHANNEL_INDEX  
DrvEDMA_GetCHForAPBDevice(
    E_DRVEDMA_APB_DEVICE eDevice,
    E_DRVEDMA_APB_RW eRWAPB    
);

ERRCODE  
DrvEDMA_SetWrapIntType(
	E_DRVEDMA_CHANNEL_INDEX eChannel, 
	E_DRVEDMA_WRAPAROUND_SELECT eType
);

E_DRVEDMA_WRAPAROUND_SELECT  
DrvEDMA_GetWrapIntType(
	E_DRVEDMA_CHANNEL_INDEX eChannel
);

ERRCODE  
DrvEDMA_CHSoftwareReset(
	E_DRVEDMA_CHANNEL_INDEX eChannel
);

ERRCODE  
DrvEDMA_CHEnablelTransfer(
	E_DRVEDMA_CHANNEL_INDEX eChannel
);

unsigned int  
DrvEDMA_GetCurrentSourceAddr(
	E_DRVEDMA_CHANNEL_INDEX eChannel
);

unsigned int  
DrvEDMA_GetCurrentDestAddr(
	E_DRVEDMA_CHANNEL_INDEX eChannel
);

unsigned int  
DrvEDMA_GetCurrentTransferCount(
	E_DRVEDMA_CHANNEL_INDEX eChannel
);

ERRCODE  
DrvEDMA_EnableInt(
    E_DRVEDMA_CHANNEL_INDEX eChannel, 
    E_DRVEDMA_int_ENABLE eIntSource
);

void DrvEDMA_DisableInt(
	E_DRVEDMA_CHANNEL_INDEX eChannel, 
	E_DRVEDMA_int_ENABLE eIntSource
);

unsigned int  
DrvEDMA_IsIntEnabled(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	E_DRVEDMA_int_ENABLE eIntSource	
);

void DrvEDMA_ClearInt(
	E_DRVEDMA_CHANNEL_INDEX eChannel, 
	E_DRVEDMA_int_FLAG eIntFlag
);

char
DrvEDMA_PollInt(
    E_DRVEDMA_CHANNEL_INDEX eChannel, 
    E_DRVEDMA_int_FLAG eIntFlag
);

ERRCODE  
DrvEDMA_SetColorTransformFormat(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	E_DRVEDMA_COLOR_FORMAT u32SourceFormat,
	E_DRVEDMA_COLOR_FORMAT u32DestFormat
);

void DrvEDMA_GetColorTransformFormat(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	E_DRVEDMA_COLOR_FORMAT* pu32SourceFormat,
	E_DRVEDMA_COLOR_FORMAT* pu32DestFormat
);


ERRCODE  
DrvEDMA_SetColorTransformOperation(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	E_DRVEDMA_OPERATION eColorSpaceTran,
	E_DRVEDMA_OPERATION eStrideMode
);

void DrvEDMA_GetColorTransformOperation(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	E_DRVEDMA_OPERATION* peColorSpaceTran,
	E_DRVEDMA_OPERATION* peStrideMode
);

ERRCODE  
DrvEDMA_SetSourceStride(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	unsigned int u32StrideByteCount,
	unsigned int u32OffsetByteLength
);

void DrvEDMA_GetSourceStride(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	unsigned int* pu32StrideByteCount,
	unsigned int* pu32OffsetByteLength
);

ERRCODE  
DrvEDMA_SetDestinationStrideOffset(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	unsigned int u32OffsetByteLength
);

void DrvEDMA_GetDestinationStrideOffset(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	unsigned int* pu32OffsetByteLength
);

ERRCODE
DrvEDMA_SetClamping(
	E_DRVEDMA_CHANNEL_INDEX eChannel,
	E_DRVEDMA_OPERATION eOP
);

E_DRVEDMA_OPERATION  
DrvEDMA_GetClamping(
E_DRVEDMA_CHANNEL_INDEX eChannel
);

unsigned int  
DrvEDMA_GetInternalBufPointer(
	E_DRVEDMA_CHANNEL_INDEX eChannel
);

unsigned int  
DrvEDMA_GetSharedBufData(
	E_DRVEDMA_CHANNEL_INDEX eChannel, 
	unsigned int u32BufIndex
);

ERRCODE  
DrvEDMA_InstallCallBack(
    E_DRVEDMA_CHANNEL_INDEX eChannel, 
    E_DRVEDMA_int_ENABLE eIntSource,
	PFN_DRVEDMA_CALLBACK pfncallback,    
	PFN_DRVEDMA_CALLBACK *pfnOldcallback  	
);

void DrvEDMA_GetScatterGatherInfo(
	unsigned int *pu32TblSize,
	unsigned int *pu32MaxTransferBytePerTbl
);
  
ERRCODE 
DrvEDMA_SetScatterGatherSetting(
    E_DRVEDMA_CHANNEL_INDEX eChannel,
	unsigned int u32SGTblStartAddr,
	S_DRVEDMA_DESCRIPT_SETTING* psDescript
);

void DrvEDMA_EnableScatterGather(
    E_DRVEDMA_CHANNEL_INDEX eChannel
);

  
void DrvEDMA_DisableScatterGather(
    E_DRVEDMA_CHANNEL_INDEX eChannel
);

void DrvEDMA_SetScatterGatherTblStartAddr(
    E_DRVEDMA_CHANNEL_INDEX eChannel, 
	unsigned int	u32TblStartAddr
);

unsigned int  
DrvEDMA_GetVersion(void);


#ifdef  __cplusplus
}
#endif

#endif	// __DRVEDMA_H__





















