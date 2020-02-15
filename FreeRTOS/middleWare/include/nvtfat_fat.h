/*************************************************************************
 * Nuvoton Electronics Corporation confidential
 *
 * Copyright (c) 2008 by Nuvoton Electronics Corporation
 * All rights reserved
 * 
 * FILENAME
 *     nvtfat_fat.h
 *
 * REMARK
 *     None
 **************************************************************************/

#ifndef	_NVTFAT_FAT_H_
#define	_NVTFAT_FAT_H_

#include "wbtypes.h"
#include "nvtfat.h"
/* Defined for long filenames */
#define	LFN_MAX_FILENAME			514		/* maximum length of LFN in bytes */

/* Macro to	convert	Cluster	number to logical sector number	*/
#define	ClustNo2SecNo(n) ((unsigned int)(n - 2) * ptLDisk->tFatInfo.uBpbSecPerClus + ptLDisk->tFatInfo.uDataSecStart)

#define SECTOR_TYPE_FILE			0
#define SECTOR_TYPE_DIR				1
#define SECTOR_TYPE_FAT				2
#define SECTOR_TYPE_PART			3

typedef	struct sector_buf_S
{
	unsigned char		*pucData;			/* cached sector contents                             */
	LDISK_T 	*ptLDisk; 			/* LDISK_T * associated	with this sector              */
	unsigned int		uSectorNo;			/* logical sector number                              */
	char 		bIsDirty;			/* dirty or not                                       */
	int  		nReaderCnt;			/* how many threads are reading it                    */
	unsigned int		jiffy;				/* access time stamp                                  */
	void 		*os_owner;			/* for OS                                             */
	void 		*os_priv;			/* for OS                                             */
} 	SEC_BUFF_T;

/* For FAT and DATA sector check out */
#define READ_ACCESS				0
#define WRITE_ACCESS			1


/*======================================================== FAT information ==*/
typedef struct fat_info_S
{
	/*- The followings are BPB fields commonly used by FAT16/FAT32 -*/
	
	unsigned int      uBpbBytesPerSec;    /* BPB[11], size:2 */
	unsigned int      uBpbSecPerClus;     /* BPB[13], size:1 */
	unsigned short      usBpbRsvdSecCnt;    /* BPB[14], size:2 */
	unsigned char       ucBpbNumFATs;       /* BPB[16], size:1 */
	unsigned short      usBpbRootEntCnt;    /* BPB[17], size:2 */
	unsigned int      uBpbTotalSectors;   /* FAT12/FAT16 - BPB[19], size:2 */
									/* FAT32 - BPB[32], size: 4 */
	unsigned char       ucBpbMedia;         /* BPB[21], size:1 */
	unsigned int      uBpbFatSize;        /* FAT12/FAT16 - BPB[22], size:2 */
									/* FAT32 - BPB[36], size:4 */
	unsigned short      usBpbSecPerTrk;     /* BPB[24], size:2 */
	unsigned short      usBpbNumHeads;      /* BPB[26], size:2 */
	unsigned int      uBpbHiddSec;        /* BPB[28], size:4 */
	
	unsigned int      uBsVolID;           /* FAT12/FAT16 - BPB[39], size:4 */
									/* FAT32 - BPB[67], size:4 */
	CHAR        sBsVolLab[12];      /* FAT12/FAT16 - BPB[43], size:11 */
									/* FAT32 - BPB[71], size:4 */

	unsigned int   	uBpbRootClus;       /* BPB[44], size:4 */
	unsigned short   	usBpbFsInfo;        /* BPB[48], size:2 */
	
	/*- The following is FAT32 structure start from offset 36 -*/
	
	/* unsigned int   uBpbBytesPerSec; */ /* BPB[36], size:4 */
	/* unsigned short   usBsDrvNum;  */     /* BPB[40], size:2 */
	/* unsigned short   usBpbFsVer; */      /* BPB[42], size:2 */
	/* unsigned short   usBpbBkBootSec; */  /* BPB[50], size:2 */
	/* bpb_Reserved */              /* BPB[52], size:12 */
	/* usBsDrvNum */                /* BPB[64], size:1 */
	/* bs_Reserved1 */              /* BPB[65], size:1 */
	/* bs_BootSig */                /* BPB[66], size:1 */

	/*- The followings are proprietary used by our driver -*/
	unsigned int      uEndClusMark;    	/* FAT12 = 0xFF7, FAT16 = 0xFFF7, FAT32 = 0x0FFFFFF7 */
	unsigned char		ucFatType;			/* TYPE_FAT12/TYPE_FAT16/TYPE_FAT32 */
	unsigned int      uSecSzMask;      	/* uBpbBytesPerSec - 1 */
	unsigned int      uBytesPerClus;   	/* uBpbBytesPerSec * uBpbSecPerClus */
	unsigned int		uLastSecNo;			/* the last physical sector number on this partition */
	unsigned int      uLastClusNo;    	/* last cluster number on this partition */
	unsigned int      uFatSecStart;    	/* sector number of the first FAT sector */
	unsigned int      uDataSecStart;   	/* sector number of first data sector */
	unsigned int      uRootDirStartSec;	/* FAT12/16 first sector number of the root directory */
	unsigned int		uFsInfoSecNo;		/* FAT32 - the sector number of pucFSInfo sector */
	unsigned int		uFreeClusSearchIdx;	/* the next start point to search a free cluster */
	unsigned int		uTotalFreeClus;		/* total number of free clusters */
	unsigned char		pucFSInfo[512];		/* pucFSInfo sector */
}  	FAT_INFO_T;


#define FAT12_END_CLUS_MARK			0xFF8			
#define FAT16_END_CLUS_MARK			0xFFF8
#define FAT32_END_CLUS_MARK			0x0FFFFFF8

#define FAT12_BAD_CLUS_MARK			0xFF7
#define FAT16_BAD_CLUS_MARK			0xFFF7
#define FAT32_BAD_CLUS_MARK			0x0FFFFFF7


#define TYPE_FAT12					12
#define TYPE_FAT16					16
#define TYPE_FAT32					32


/*================================================ FAT directory structure ==*/
/* 
 * A FAT directory entry size is 32 bytes and sector size is 512 bytes.
 * So, it must be word-aligned.
 */
/* physical file name entry */
typedef struct dir_ent_S
{
	unsigned char       pucDirName[8];		/* Offset: 0,  Size: 11 */
	unsigned char		pucDirExtName[3];	
	unsigned char       ucDirAttrib;		/* Offset: 11, Size: 1  */
	unsigned char       ucDirNTRes;			/* Offset: 12, Size: 1  */
	unsigned char       ucDirCTimeTenth;	/* Offset: 13, Size: 1  */
	unsigned char       pucDirCTime[2];    	/* Offset: 14, Size: 2  */
	unsigned char       pucDirCDate[2];     /* Offset: 16, Size: 2  */
	unsigned char       pucDirLADate[2];  	/* Offset: 18, Size: 2  */
	unsigned char       pucDirFirstClusHi[2];/* Offset: 20, Size: 2  */
	unsigned char       pucDirWTime[2];     /* Offset: 22, Size: 2  */
	unsigned char       pucDirWDate[2];		/* Offset: 24, Size: 2  */
	unsigned char       pucDirFirstClusLo[2];/* Offset: 26, Size: 2  */
	unsigned char       pucDirFileSize[4];	/* Offset: 28, Size: 4  */
} 	DIR_ENT_T;


/* physical long file name entry */
typedef struct lfn_ent_S
{
	unsigned char       ucLDirOrd;			/* Offset: 0,  Size: 1 */
	unsigned char       pucLDirName1[10];   /* Offset: 1,  Size: 10, Unicode Chars 1~5 */
	unsigned char       ucLDirAttrib;       /* Offset: 11, Size: 1, always 0x0F */
	unsigned char       ucLDirType;         /* Offset: 12, Size: 1 */
	unsigned char       ucLDirChksum;       /* Offset: 13, Size: 1, Checksum of 8.3 name */
	unsigned char       pucLDirName2[12];   /* Offset: 14, Size: 12, Unicode Chars 6-11 */
	unsigned char       pucLDirFirstClusLo[2]; 	/* Offset: 26, Size: 2, always 0  */
	unsigned char       pucLDirName3[4];    /* Offset: 28, Size: 4, Unicode Chars 12-13 */
} 	LFN_ENT_T;

/* FAT file attributes */
#define A_NORMAL       	0x00        /* Normal file, no attributes */
#define A_RDONLY       	0x01        /* Read only attribute */
#define A_HIDDEN       	0x02        /* Hidden file */
#define A_SYSTEM       	0x04        /* System file */
#define A_LABEL        	0x08        /* Volume label */
#define A_DIR          	0x10        /* Directory    */
#define A_ARCHIVE       0x20        /* Archive */
#define A_LFN           0x0F		/* (A_RDONLY | A_HIDDEN | A_SYSTEM | A_LABEL) */
#define A_LFN_MASK      0x3F		/* (A_LFN | A_DIR | A_ARCHIVE) */


/*================================================ Run-time data structure ==*/
/*
 *  Each FILE_T has its own FAT configuration on a FAT file system. Refer to the
 *  FILE_T defined in nvtfat.h.
 */
typedef struct file_control_block_S
{
	/* For directory entry */	
	//CHAR		suLongName[LFN_MAX_FILENAME];
	CHAR		szShortName[14];
	unsigned char       ucDirAttrib;		/* file attribute */
	unsigned char       ucDirNTRes;
	unsigned char	    ucDirCTimeTenth;    /* Millisecond stamp at file creation time */	unsigned short      dirCtime;           /* Time file was created */
	unsigned short      usDirCDate;         /* Date file was created */
	unsigned short      usDirLDate;         /* Last access date */
	unsigned short      usDirWTime;			/* Time of last write. Note that file creation is considered a write. */
	unsigned short      usDirWDate;			/* Date of last write. Note that file creation is considered a write. */
	
	unsigned int		uDirEntSecNo;		/* Number of the sector where this directory entry is located. */
	int			nDirEntInSecOffset;	/* Directory entry offset of this file in <uDirEntSecNo> sector. */

	/* for run-time file operation */
	char		bIsFat16Root;		/* is FAT12/FAT16 root directory */

	char		bIsRWBuffDirty;		/* read/write buffer is dirty or not */
	unsigned char		*pucRWBuff;			/* file r/w buffer */
	__int64		n64RWBuffMapPos;	/* the file position of the r/w buffer mapped to */
	unsigned int		uRWBuffSecNo;		/* the start sector number of r/w buffer */
	int 		nRWBuffSecCnt;		/* the start sector number of r/w buffer */
	int			nClusCnt;			/* number of clusters cached in r/w buffer */
	unsigned int		uRWBuffSize;		/* the size of read/write buffer */

	unsigned int		*pucCCBuff;			/* cluster chain buffer */
	unsigned int		uCCBuffSize;		/* cluster chain buffer size */
	unsigned int		uCCEntCnt;			/* cluster chain entry count */
	unsigned int		uCCDoneFPos;		/* cluster chain done file position */

	unsigned int  	uFirstClusNo;   	/* first cluster number of this file */
	unsigned int		uLastClusNo;
	unsigned int  	uCurClusNo;         /* the number of the cluster currently used */
	__int64		n64FileSize;		/* The total length of this file */
	__int64  		n64FilePos;         /* current position in file */
}  	FAT_FCB_T;


#ifdef ECOS
extern cyg_mutex_t  _fat_mutex;
#define FAT_MUTEX_INIT()		cyg_mutex_init(&_fat_mutex)   
#define FAT_MUTEX_LOCK()        cyg_mutex_lock(&_fat_mutex)
#define FAT_MUTEX_UNLOCK()		cyg_mutex_unlock(&_fat_mutex)
#else  /* Non-OS */
#define FAT_MUTEX_INIT()		
#define FAT_MUTEX_LOCK()        
#define FAT_MUTEX_UNLOCK()
#endif



/*====================================== External reference in library only ==*/
extern DISK_OP_T _fs_tFatDiskOperation;
extern FILE_OP_T _fs_tFatFileOperation;
extern char		 _fs_bLockFileSystem;

extern int  fs_fat_parse_partition(PDISK_T *ptPDisk, PARTITION_T *ptPartition, unsigned int uBpbSecNo);
extern int  fs_fat_format_partition(PDISK_T *ptPDisk, PARTITION_T *ptPartition, int nType);

extern int   fs_fat32_scan_disk(LDISK_T *ptLDisk);
extern int   fs_fat32_defragment_disk(LDISK_T *ptLDisk);

/* Cache */
extern void fs_fat_init_sector_cache(void);
extern void fs_fat_debug_sector_cache(void);

extern int  fs_fat_check_out_sec(LDISK_T *ptLDisk, unsigned int uSectorNo, int nAccess, SEC_BUFF_T **ptSecBuff);
extern void  fs_fat_check_in_sec(SEC_BUFF_T *ptSecBuff, int nAccess, char bIsDirty);
extern int  fs_fat_flush_sector_cache(LDISK_T *ptLDisk);
extern void fs_fat_clear_sector_cache(LDISK_T *ptLDisk);

/* FAT32 pucFSInfo sector */
//extern int  fs_fat_update_fs_info(LDISK_T *ptLDisk);

extern int  fsScanDisk(int nDriveNo);

/* FAT table */
extern int  read_fs_info(LDISK_T *ptLDisk);
extern int  fs_read_fat_table(LDISK_T *ptLDisk, unsigned int uClusNo, unsigned int *puEntryValue);
extern int  fs_write_fat_table(LDISK_T *ptLDisk, unsigned int uClusNo, unsigned int uEntryValue);
extern int  fs_fat_reclaim_clusters(LDISK_T *ptLDisk, unsigned int uStartClusNo);
extern int  fs_fat_allocate_cluster(FILE_T *ptFile);

/* directory entry */
extern int  fs_fat_delete_file(LDISK_T *ptLDisk, CHAR *suFullName, CHAR *szAsciiName, char bIsDirectory);
extern int  fs_fat_create_file(FILE_T *ptFile, CHAR *suFullName, CHAR *szAsciiName, unsigned int *uClusNoOfParent);
extern int  fs_fat_merge_file(LDISK_T *ptLDisk, CHAR *suFileNameA, CHAR *szAsciiNameA, CHAR *suFileNameB, CHAR *szAsciiNameB);
extern int  fs_fat_get_next_dir_entry(FILE_T *ptFile, unsigned char *pucBuff, CHAR **szLongName, DIR_ENT_T *ptDirEnt, unsigned int *uStartPos);
extern void fs_fat_set_dire_mtime(FAT_FCB_T *ptFcb, DIR_ENT_T *ptDire, int bType);
extern void fs_fat_set_sdir_name(CHAR *szShortName, DIR_ENT_T *ptDire);
extern void fs_fat_get_sdir_name(CHAR *szShortName, DIR_ENT_T *ptDire);
extern unsigned char  *fs_fat_get_ldir_name(unsigned char *pucNameBuff, LFN_ENT_T *ptLDire);
extern void fs_fat_set_dire_info(FAT_FCB_T *ptFcb, DIR_ENT_T *ptDire);
extern void fs_fat_get_dire_info(FAT_FCB_T *ptFcb, DIR_ENT_T *ptDire);
extern int  fs_fat_search_file(FILE_T *ptFile, CHAR *szFileName);
extern int  fs_fat_delete_tree(LDISK_T *ptLDisk, CHAR *suFullName, CHAR *szAsciiName);
extern int  fs_update_first_cluster(FILE_T *ptFile, unsigned int uClusNo);

extern void fs_fat_dump_fcb(FAT_FCB_T *ptFcb);
extern void fsDumpUniStr(unsigned char *suStr);

#endif	/* _NVTFAT_FAT_H_ */
