/*-----------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                       */
/*                                                                                   */
/* Copyright (c) 2008 by Nuvoton Technology Corporation                              */
/* All rights reserved                                                               */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
#ifndef _FMILIB_H_
#define _FMILIB_H_

//#define OPT_SW_WP
#define OPT_FA93
#define OPT_MARK_BAD_BLOCK_WHILE_ERASE_FAIL

#define FMI_SD_CARD		0
#define FMI_SM_CARD		1

#define FMI_ERR_ID	0xFFFF0100

#define FMI_TIMEOUT				(FMI_ERR_ID|0x01)
#define FMI_NO_MEMORY			(FMI_ERR_ID|0x02)
/* SD error */
#define FMI_NO_SD_CARD			(FMI_ERR_ID|0x10)
#define FMI_ERR_DEVICE			(FMI_ERR_ID|0x11)
#define FMI_SD_INIT_TIMEOUT		(FMI_ERR_ID|0x12)
#define FMI_SD_SELECT_ERROR		(FMI_ERR_ID|0x13)
#define FMI_SD_WRITE_PROTECT	(FMI_ERR_ID|0x14)
#define FMI_SD_INIT_ERROR		(FMI_ERR_ID|0x15)
#define FMI_SD_CRC7_ERROR		(FMI_ERR_ID|0x16)
#define FMI_SD_CRC16_ERROR		(FMI_ERR_ID|0x17)
#define FMI_SD_CRC_ERROR		(FMI_ERR_ID|0x18)
#define FMI_SD_CMD8_ERROR		(FMI_ERR_ID|0x19)

/* NAND error */
#define FMI_SM_INIT_ERROR		(FMI_ERR_ID|0x20)
#define FMI_SM_RB_ERR			(FMI_ERR_ID|0x21)
#define FMI_SM_STATE_ERROR		(FMI_ERR_ID|0x22)
#define FMI_SM_ECC_ERROR		(FMI_ERR_ID|0x23)
#define FMI_SM_STATUS_ERR		(FMI_ERR_ID|0x24)
#define FMI_SM_ID_ERR			(FMI_ERR_ID|0x25)
#define FMI_SM_INVALID_BLOCK	(FMI_ERR_ID|0x26)
#define FMI_SM_MARK_BAD_BLOCK_ERR	(FMI_ERR_ID|0x27)

/* MS error */
#define FMI_NO_MS_CARD			(FMI_ERR_ID|0x30)
#define FMI_MS_INIT_ERROR		(FMI_ERR_ID|0x31)
#define FMI_MS_int_TIMEOUT		(FMI_ERR_ID|0x32)
#define FMI_MS_BUSY_TIMEOUT		(FMI_ERR_ID|0x33)
#define FMI_MS_CRC_ERROR		(FMI_ERR_ID|0x34)
#define FMI_MS_int_CMDNK		(FMI_ERR_ID|0x35)
#define FMI_MS_int_ERR			(FMI_ERR_ID|0x36)
#define FMI_MS_int_BREQ			(FMI_ERR_ID|0x37)
#define FMI_MS_int_CED_ERR		(FMI_ERR_ID|0x38)
#define FMI_MS_READ_PAGE_ERROR	(FMI_ERR_ID|0x39)
#define FMI_MS_COPY_PAGE_ERR	(FMI_ERR_ID|0x3a)
#define FMI_MS_ALLOC_ERR		(FMI_ERR_ID|0x3b)
#define FMI_MS_WRONG_SEGMENT	(FMI_ERR_ID|0x3c)
#define FMI_MS_WRONG_PHYBLOCK	(FMI_ERR_ID|0x3d)
#define FMI_MS_WRONG_TYPE		(FMI_ERR_ID|0x3e)
#define FMI_MS_WRITE_DISABLE	(FMI_ERR_ID|0x3f)

#define NAND_TYPE_SLC		0x01
#define NAND_TYPE_MLC		0x00

#define NAND_PAGE_512B		512
#define NAND_PAGE_2KB		2048
#define NAND_PAGE_4KB		4096
#define NAND_PAGE_8KB		8192

typedef struct fmi_sm_info_t
{
	unsigned int	uSectorPerFlash;
	unsigned int	uBlockPerFlash;
	unsigned int	uPagePerBlock;
	unsigned int	uSectorPerBlock;
	unsigned int	uLibStartBlock;
	unsigned int	nPageSize;
	unsigned int	uBadBlockCount;
	char	bIsMulticycle;
	char	bIsMLCNand;
	char	bIsNandECC4;
	char	bIsNandECC8;
	char	bIsNandECC12;
	char	bIsNandECC15;			
	char	bIsCheckECC;
} FMI_SM_INFO_T;

extern FMI_SM_INFO_T *pSM0, *pSM1;


typedef struct fmi_sd_info_t
{
	unsigned int	uCardType;		// sd2.0, sd1.1, or mmc
	unsigned int	uRCA;			// relative card address
	char	bIsCardInsert;
} FMI_SD_INFO_T;

extern FMI_SD_INFO_T *pSD0;
extern FMI_SD_INFO_T *pSD1;
extern FMI_SD_INFO_T *pSD2;


// function prototype
void fmiInitDevice(void);
void fmiSetFMIReferenceClock(unsigned int uClock);

int  fmiSD_CardSel(int cardSel);
int  fmiSD_Read(unsigned int uSector, unsigned int uBufcnt, unsigned int uDAddr);
int  fmiSD_Write(unsigned int uSector, unsigned int uBufcnt, unsigned int uSAddr);

// for file system
//int  fmiInitSDDevice(void);
int  fmiInitSDDevice(int cardSel);

//PDISK_T *fmiGetpDisk(unsigned int uCard);

// callback function
void fmiSetCallBack(unsigned int uCard, void * pvRemove, void * pvInsert);

/* extern function */
#define SIC_SET_CLOCK		0
#define SIC_SET_CALLBACK	1
#define SIC_GET_CARD_STATUS 2

// MMC run 20MHz
#define MMC_FREQ    20000
#define EMMC_FREQ   26000

#define SD_FREQ		24000
#define SDHC_FREQ	24000


#define NO_SD_CARD_DETECT

void sicOpen(void);
void sicClose(void);
int sicSdOpen(void);
//int sicSdOpen(int cardSel);
void sicSdClose(void);
//void sicSdClose(int cardSel);

int sicSdOpen0(void);
int sicSdOpen1(void);
int sicSdOpen2(void);
void sicSdClose0(void);
void sicSdClose1(void);
void sicSdClose2(void);

void sicIoctl(int sicFeature, int sicArg0, int sicArg1, int sicArg2);

int sicSdRead(int sdSectorNo, int sdSectorCount, int sdTargetAddr);
int sicSdRead0(int sdSectorNo, int sdSectorCount, int sdTargetAddr);
int sicSdRead1(int sdSectorNo, int sdSectorCount, int sdTargetAddr);
int sicSdRead2(int sdSectorNo, int sdSectorCount, int sdTargetAddr);

int sicSdWrite(int sdSectorNo, int sdSectorCount, int sdSourceAddr);
int sicSdWrite0(int sdSectorNo, int sdSectorCount, int sdSourceAddr);
int sicSdWrite1(int sdSectorNo, int sdSectorCount, int sdSourceAddr);
int sicSdWrite2(int sdSectorNo, int sdSectorCount, int sdSourceAddr);


void fmiSMClose(int chipSel);

/* gnand use */
#include "w55fa93_gnand.h"

int nandInit0(NDISK_T *NDISK_info);
int nandpread0(int PBA, int page, unsigned char *buff);
int nandpwrite0(int PBA, int page, unsigned char *buff);
int nand_is_page_dirty0(int PBA, int page);
int nand_is_valid_block0(int PBA);
int nand_block_erase0(int PBA);
int nand_chip_erase0(void);
int nand_ioctl(int param1, int param2, int param3, int param4);

int nandInit1(NDISK_T *NDISK_info);
int nandpread1(int PBA, int page, unsigned char *buff);
int nandpwrite1(int PBA, int page, unsigned char *buff);
int nand_is_page_dirty1(int PBA, int page);
int nand_is_valid_block1(int PBA);
int nand_block_erase1(int PBA);
int nand_chip_erase1(void);

#endif //_FMILIB_H_
