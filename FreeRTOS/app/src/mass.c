/****************************************************************************
*                                                                           *
* Copyright (c) 2009 Nuvoton Tech. Corp. All rights reserved.               *
*                                                                           *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mass.h"

#define NAND1_1_SIZE	 32	/* MB unit */
static NDISK_T ptNDisk;

#define  NANDFLASH_PRINTF	sysprintf





static NDRV_T _nandDiskDriver0 =
{
    nandInit0,
    nandpread0,
    nandpwrite0,
    nand_is_page_dirty0,
    nand_is_valid_block0,
    nand_ioctl,
    nand_block_erase0,
    nand_chip_erase0,
    0
};
char connected(void)
{
    return (char)(udcIsAttached());
}


void mass(NDISK_T *disk, int i32TotalSector)
{
#if 0
	WB_CLKFREQ_T clock;		
	
	udcFlashInit((char *)disk);  	
    sysGetClockFreq(&clock);
    udcInit(clock.pll_clk_freq);	
	udcMassBulk((T_CallbackFunc*) connected);	
	udcDeinit();
	return;
#else	

#if 0
	/* initial nuvoton file system */
	fsInitFileSystem();
	fsAssignDriveNumber('C', DISK_TYPE_SMART_MEDIA, 0, 1);     // NAND 0, 2 partitions
	fsAssignDriveNumber('D', DISK_TYPE_SMART_MEDIA, 0, 2);     // NAND 0, 2 partitions

	/* initialize FMI (Flash memory interface controller) */
	sicIoctl(SIC_SET_CLOCK, 192000, 0, 0);  /* clock from PLL */
	sicOpen();
#endif
  	
	mscdInit();		
	mscdFlashInit((NDISK_T *)disk, i32TotalSector);	
	udcInit();
	mscdMassEvent(connected);
	udcDeinit();
	udcClose();	
#endif	
}

void NVT_MountMem(void)
{
	UINT32 block_size, free_size, disk_size; 
	UINT32 u32TotalSize; 

	fsAssignDriveNumber('C', DISK_TYPE_SMART_MEDIA, 0, 1);
	fsAssignDriveNumber('D', DISK_TYPE_SMART_MEDIA, 0, 2);         
	fsAssignDriveNumber('X', DISK_TYPE_SD_MMC, 0, 1); 
  /* For detect VBUS stable */ 
	sicIoctl(SIC_SET_CLOCK, 240000, 0, 0);  /* clock from PLL */   
	//--- Enable AHB clock for SIC/SD/NAND, interrupt ISR, DMA, and FMI engineer	
	sicOpen();
	if(sicSdOpen0() <= 0)
		sysprintf("fail open SD0!\n");
	else
		sysprintf("success open SD0!\n");
	TestSDFun();
	/* In here for USB VBus stable. Othwise, USB library can not judge VBus correct  */ 	
#ifdef UDC_EN
	udcOpen();	 
#endif	 	  	
	/* Initialize GNAND */
	if(GNAND_InitNAND(&_nandDiskDriver0, &ptNDisk, TRUE) < 0) {
		NANDFLASH_PRINTF("GNAND_InitNAND error\n");
		return;
	}	
	if(GNAND_MountNandDisk(&ptNDisk) < 0) {
		NANDFLASH_PRINTF("GNAND_MountNandDisk error\n");
		return;		
	}
	
	/* Get NAND disk information*/ 
	u32TotalSize = ptNDisk.nZone* ptNDisk.nLBPerZone*ptNDisk.nPagePerBlock*ptNDisk.nPageSize;
	NANDFLASH_PRINTF("Total Disk Size %d\n", u32TotalSize);	
	/* Format NAND if necessery */
		/* Format NAND if necessery */
	//extern INT  fsTwoPartAndFormatAll(PDISK_T *ptPDisk, INT firstPartSize, INT secondPartSize);
	if ((fsDiskFreeSpace('C', &block_size, &free_size, &disk_size) < 0) || \
	    (fsDiskFreeSpace('D', &block_size, &free_size, &disk_size) < 0))
	{   
				NANDFLASH_PRINTF("unknow disk type, format device .....\n");	
		    	if (fsTwoPartAndFormatAll((PDISK_T *)ptNDisk.pDisk, NAND1_1_SIZE*1024, (u32TotalSize- NAND1_1_SIZE*1024)) < 0) 
				{
					NANDFLASH_PRINTF("Format failed\n");					
				return;
				}
	    fsSetVolumeLabel('C', "NAND1-1\n", strlen("NAND1-1"));
		fsSetVolumeLabel('D', "NAND1-2\n", strlen("NAND1-2"));	
	}
		/* Get NAND disk information*/ 
	u32TotalSize = ptNDisk.nZone* ptNDisk.nLBPerZone*ptNDisk.nPagePerBlock*ptNDisk.nPageSize;
	NANDFLASH_PRINTF("NandFlash Total Disk Size %d bytes\n", u32TotalSize);	
}

