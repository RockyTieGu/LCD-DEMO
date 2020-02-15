/*-----------------------------------------------------------------------------------*/
/* Nuvoton Electronics Corporation confidential                                      */
/*                                                                                   */
/* Copyright (c) 2008 by Nuvoton Electronics Corporation                             */
/* All rights reserved                                                               */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/
#ifndef _GNAND_H_
#define _GNAND_H_


#define GNAND_OK					0
#define GNERR_READ_L2P_MISS			1 	/* read block not found in L2P        */
#define GNAND_ERR					0xFFFFC000	

/* GENERAL ERRORs */									   		
#define GNERR_GENERAL               (GNAND_ERR+1)		/* general error                      */
#define GNERR_MEMORY_OUT      		(GNAND_ERR+0x5)		/* memory not enough                  */
#define GNERR_GNAND_FORMAT    		(GNAND_ERR+0x10) 	/* not GNAND format                   */
#define GNERR_FAT_FORMAT            (GNAND_ERR+0x15) 	/* NAND disk was not formatted as FAT */
#define GNERR_BLOCK_OUT				(GNAND_ERR+0x20) 	/* there's no available free blocks   */
#define GNERR_P2LN_SYNC				(GNAND_ERR+0x25) 	
#define GNERR_READONLY_NAND			(GNAND_ERR+0x26)	/* XtraROM */ 	

/* for NAND driver return value */
#define GNERR_IO_ERR				(GNAND_ERR+0x30) 	/* NAND read/write/erase access failed*/
#define GNERR_NAND_NOT_FOUND		(GNAND_ERR+0x40) 	/* NAND driver cannot find NAND disk  */
#define GNERR_UNKNOW_ID				(GNAND_ERR+0x42) 	/* Not supported NAND ID              */
#define GNERR_UNKNOW_ID0			(GNAND_ERR+0x43) 	/* Not supported ID                   */


/*-------------------------------------------------------------------*/
/*  NAND driver function set                                         */
/*-------------------------------------------------------------------*/
#define NDISK_T		struct ndisk_t
#define NDRV_T		struct ndrv_t


typedef struct p2lm_t
{
	unsigned short  lba;			/* logical block address                 */
	unsigned short  age;			/* times this block has been used        */
}  P2LM_T;


typedef struct l2pm_t
{
	unsigned short	pba;           	/* physical block address                */
	unsigned short  reserved;      	/* reserved for future used              */
}  L2PM_T;


#define NAND_TYPE_SLC		0x01
#define NAND_TYPE_MLC		0x00


/*-------------------------------------------------------------------*/
/*  NAND disk infotmation. This information was provided             */
/*  by NAND driver.                                                  */
/*-------------------------------------------------------------------*/
struct ndisk_t
{
	int  	vendor_ID;
	int  	device_ID;
	int  	NAND_type;
	int  	nZone;            	/* number of zones                       */
	int  	nBlockPerZone;     	/* blocks per zone                       */
	int  	nPagePerBlock;     	/* pages per block                       */
	int  	nLBPerZone;        	/* logical blocks per zone               */
	int  	nPageSize; 
	int  	nStartBlock;        /* available start block                 */
	int  	nBadBlockCount;     /* bad block count                       */
	NDRV_T	*driver;			/* NAND driver to work on this NAND disk */
	int  	nNandNo;
	void  	*pDisk;
	int  	reserved[60];
	int	    need2P2LN;
	int  	p2ln_block1;
	
	/* for GNAND internal used */
	P2LM_T	*p2lm;
	L2PM_T	*l2pm;
	unsigned char	*dp_tbl;		/* dirty page bit map */
	unsigned short  db_idx[16];		/* data block search index, up to 8 zone */
	unsigned short	p2ln_block;		/* P2LN block No. */
	unsigned short	op_block;		/* OP block No. */
	int  	op_offset;		/* operation index */
	unsigned char   last_op[32];	/* the last op code in op table */
		
	int		err_sts;
	struct ndisk_t  *next;
};


struct ndrv_t
{
	int  (*init)(NDISK_T *NDInfo);
	int  (*pread)(int nPBlockAddr, int nPageNo, unsigned char *buff);
	int  (*pwrite)(int nPBlockAddr, int nPageNo, unsigned char *buff);
	int  (*is_page_dirty)(int nPBlockAddr, int nPageNo);
	int  (*is_valid_block)(int nPBlockAddr);
	int  (*ioctl)(int param1, int param2, int param3, int param4);
	int  (*block_erase)(int nPBlockAddr);
	int  (*chip_erase)(void);
	void *next;
} ;


/*-------------------------------------------------------------------*/
/* Export functions                                                  */
/*-------------------------------------------------------------------*/
int  GNAND_InitNAND(NDRV_T *ndriver, NDISK_T *ptNDisk, char bEraseIfNotGnandFormat);
int  GNAND_MountNandDisk(NDISK_T *ptNDisk);
int  GNAND_read(NDISK_T *ptNDisk, unsigned int nSectorNo, int nSectorCnt, unsigned char *buff);
int  GNAND_write(NDISK_T *ptNDisk, unsigned int nSectorNo, int nSectorCnt, unsigned char *buff);
int  GNAND_block_erase(NDISK_T *ptNDisk, int pba);
int  GNAND_chip_erase(NDISK_T *ptNDisk);
void GNAND_UnMountNandDisk(NDISK_T *ptNDisk);

#endif 	/* _GNAND_H_ */
