/*************************************************************************
 * Nuvoton Electronics Corporation confidential
 *
 * Copyright (c) 2008 by Nuvoton Electronics Corporation
 * All rights reserved
 * 
 * FILENAME
 *     nvtfat.h
 *
 * REMARK
 *     None
 **************************************************************************/

#ifndef _NVTFAT_H_
#define _NVTFAT_H_

#define MAX_PATH_LEN            	520		/* maximum length of full path, counted by 
										   	   character, note that one character may be 
										       compsoed of two bytes */
#define MAX_FILE_NAME_LEN       	514


/*
 * Declared for pre-referenced data strucutres
 */
struct storage_driver_S;
struct physical_disk_S;
struct partition_S;
struct logical_disk_S;
struct fs_op_S;
struct file_op_S;
struct file_S;	
struct file_find_S;	
struct file_stat_S;

#define STORAGE_DRIVER_T		struct storage_driver_S
#define PDISK_T					struct physical_disk_S
#define PARTITION_T				struct partition_S
#define LDISK_T 				struct logical_disk_S
#define DISK_OP_T				struct disk_op_S
#define FILE_OP_T				struct file_op_S
#define FILE_T					struct file_S
#define FILE_FIND_T				struct file_find_S	
#define FILE_STAT_T				struct file_stat_S

/*
 *  Include other header files
 */
#include "nvtfat_fat.h"


/*================================================= Error Code Definitions ==*/

/*--- ERROR CODEs ---*/
#define FS_OK              			0

/* GENERAL ERRORs */									   		
#define ERR_FILE_EOF                0xFFFF8200	/* end of file */
#define ERR_GENERAL_FILE_ERROR      0xFFFF8202 	/* general file error */
#define ERR_NO_FREE_MEMORY          0xFFFF8204	/* no free memory */
#define ERR_NO_FREE_BUFFER          0xFFFF8206	/* no available sector buffers */
#define ERR_NOT_SUPPORTED			0xFFFF8208	/* feature or function was not supported */
#define ERR_UNKNOWN_OP_CODE       	0xFFFF820A	/* unrecognized operation code */
#define ERR_intERNAL_ERROR			0xFFFF820C	/* file system internal error */
#define ERR_SYSTEM_LOCK				0xFFFF820E	/* file system locked by ScanDisk or Defragment */

/* FILE ERRORs */									   		
#define ERR_FILE_NOT_FOUND          0xFFFF8220	/* file not found */
#define ERR_FILE_INVALID_NAME       0xFFFF8222	/* invalid file name */
#define ERR_FILE_INVLAID_HANDLE     0xFFFF8224	/* invalid file hFile */
#define ERR_FILE_IS_DIRECTORY       0xFFFF8226  /* try to open a directory file */
#define ERR_FILE_IS_NOT_DIRECTORY   0xFFFF8228	/* is not a directory file */
#define ERR_FILE_CREATE_NEW         0xFFFF822A  /* can not create new entry    */
#define ERR_FILE_OPEN_MAX_LIMIT		0xFFFF822C	/* has opened too many files */
#define ERR_FILE_EXIST				0xFFFF822E	/* file already exist */
#define ERR_FILE_INVALID_OP			0xFFFF8230	/* invalid file operation */
#define ERR_FILE_INVALID_ATTR		0xFFFF8232	/* invalid file attribute */
#define ERR_FILE_INVALID_TIME		0xFFFF8234	/* invalid time specified */
#define ERR_FILE_TRUNC_UNDER		0xFFFF8236	/* truncate file underflow, size < pos */  
#define ERR_FILE_NO_MORE			0xFFFF8238	/* No really an error, used to identify end of file enumeration of a directory */
#define ERR_FILE_IS_CORRUPT			0xFFFF823A	/* file is corrupt */

/* PATH ERRORs */											
#define ERR_PATH_INVALID            0xFFFF8260	/* path name was invalid */
#define ERR_PATH_TOO_LONG           0xFFFF8262	/* path too long */
#define ERR_PATH_NOT_FOUND          0xFFFF8264	/* path not found */

/* DRIVE ERRORs */											
#define ERR_DRIVE_NOT_FOUND         0xFFFF8270	/* drive not found, the disk may have been unmounted */
#define ERR_DRIVE_INVALID_NUMBER    0xFFFF8272	/* invalid drive number */
#define ERR_DRIVE_NO_FREE_SLOT      0xFFFF8274	/* can not mount more drive */

/* DIRECTORY ERRORS */											
#define ERR_DIR_BUILD_EXIST         0xFFFF8290  /* try to build an already exist directory */
#define ERR_DIR_REMOVE_MISS         0xFFFF8292  /* try to remove a nonexistent directory */
#define ERR_DIR_REMOVE_ROOT         0xFFFF8294  /* try to remoe root directory */
#define ERR_DIR_REMOVE_NOT_EMPTY    0xFFFF8296  /* try to remove a non-empty directory */
#define ERR_DIR_DIFFERENT_DRIVE     0xFFFF8298  /* specified files on different drive */
#define ERR_DIR_ROOT_FULL           0xFFFF829A  /* root directory full */
#define ERR_DIR_SET_SIZE			0xFFFF829C	/* try to set file size of a directory */
#define ERR_DIR_MOVE_DISK			0xFFFF829E	/* cannot move the whole directory from disk to another disk */

/* ACCESS ERRORs */										
#define ERR_READ_VIOLATE            0xFFFF82C0  /* user has no read privilege */
#define ERR_WRITE_VIOLATE			0xFFFF82C2	/* user has no write privilege */
#define ERR_ACCESS_VIOLATE			0xFFFF82C4	/* can not access */
#define ERR_READ_ONLY            	0xFFFF82C6  /* try to write a read-only file */
#define ERR_WRITE_CAP				0xFFFF82C8	/* try to write file which was opened with read-only */
#define ERR_OPEN_WRITE_OPENED		0xFFFF82CA	/* try to open-write a file, which has been opened */
#define ERR_DEL_OPENED				0xFFFF82CC	/* try to delete a file, which has been opened */

/* DISK ERRORs */										
#define ERR_NO_DISK_MOUNT			0xFFFF8300	/* there's no any disk mounted */
#define ERR_DISK_CHANGE_DIRTY       0xFFFF8302 	/* disk change, buffer is dirty */
#define ERR_DISK_REMOVED            0xFFFF8304  /* portable disk has been removed */
#define ERR_DISK_WRITE_PROTECT      0xFFFF8306	/* disk is write-protected */
#define ERR_DISK_FULL               0xFFFF8308	/* disk is full */
#define ERR_DISK_BAD_PARTITION      0xFFFF830A	/* bad partition */
#define ERR_DISK_UNKNOWN_PARTITION	0xFFFF830C	/* unknow or not supported partition type */
#define ERR_DISK_UNFORMAT			0xFFFF830E	/* partition was not formatted */
#define ERR_DISK_UNKNOWN_FORMAT     0xFFFF8310	/* unknown disk format */
#define ERR_DISK_SECTOR_SIZE		0xFFFF8311	/* secotr size too large, not supported */
#define ERR_DISK_BAD_BPB			0xFFFF8312	/* bad BPB, disk may not be formatted */
#define ERR_DISK_IO					0xFFFF8314	/* disk I/O failure */
#define ERR_DISK_IO_TIMEOUT			0xFFFF8316	/* disk I/O time-out */
#define ERR_DISK_FAT_BAD_CLUS		0xFFFF8318	/* bad cluster number in FAT table */
#define ERR_DISK_IO_BUSY			0xFFFF831A	/* I/O device is busy writing, must retry. direct-write mode only */
#define ERR_DISK_INVALID_PARM		0xFFFF831C	/* invalid parameter */
#define ERR_DISK_CANNOT_LOCK		0xFFFF831E	/* cannot lock disk, the disk was in-use or locked by other one */
#define ERR_DISK_PDISK_REMOVE		0xFFFF8320	/* physical disk disconnect error */
#define ERR_DISK_LDISK_REMOVE		0xFFFF8322	/* logical disk disconnect error */

/* FILE SEEK ERRORs */											
#define ERR_SEEK_SET_EXCEED         0xFFFF8350  /* file seek set exceed end-of-file */
#define ERR_ACCESS_SEEK_WRITE       0xFFFF8352  /* try to seek a file which was opened for written */

/* OTHER ERRORs */										
#define ERR_FILE_SYSTEM_NOT_INIT    0xFFFF83A0	/* file system was not initialized */
#define ERR_ILLEGAL_ATTR_CHANGE     0xFFFF83A2  /* illegal file attribute change */
#define ERR_CHECKDISK_FILE_OPENED	0xFFFF83A4	/* there's file opened, cannot do scandisk operation */ 
#define ERR_CHECKDISK_LOCK			0xFFFF83A6	/* service locked by check disk operation */

/*============================================= disk and file system types ==*/
#define FILE_SYSTEM_FAT12       		12
#define FILE_SYSTEM_FAT16       		16
#define FILE_SYSTEM_FAT32       		32
#define FILE_SYSTEM_NTFS        		64
#define FILE_SYSTEM_ISO9660     		99

#define DISK_TYPE_MASK					0x00FFFFFF
#define DISK_TYPE_HARD_DISK     		0x00000001
#define DISK_TYPE_RAM_DISK      		0x00000002
#define DISK_TYPE_CDR           		0x00000004
#define DISK_TYPE_SMART_MEDIA           0x00000008
#define DISK_TYPE_CF           			0x00000010
#define DISK_TYPE_SD_MMC       			0x00000020
#define DISK_TYPE_FM_CARD    			0x00002000	/* A bit OR */
#define DISK_TYPE_USB_DEVICE    		0x00004000	/* A bit OR */
#define DISK_TYPE_DMA_MODE              0x00008000	/* A bit OR */
#define DISK_TYPE_READ_ONLY             0x00010000
#define DISK_TYPE_SET_INSTANCE(n)		(n << 24)
#define DISK_INSTANCE_IS(n)				((n >> 24) & 0xff)


#define PARTITION_TYPE_UNKNOWN			0x00
#define PARTITION_TYPE_FAT12			0x01
#define PARTITION_TYPE_FAT16_OLD		0x04
#define PARTITION_TYPE_EXTENDED_DOS		0x05
#define PARTITION_TYPE_FAT16			0x06
#define PARTITION_TYPE_NTFS				0x07
#define PARTITION_TYPE_FAT32			0x0B
#define PARTITION_TYPE_FAT32_LBA		0x0C
#define PARTITION_TYPE_FAT16_LBA		0x0E
#define PARTITION_TYPE_DOS_LBA			0x0F
#define PARTITION_TYPE_LINUX_NATIVE		0x83



/*
 *  Storage driver structure - Each type of media should support its own storage
 *  driver. The storage driver control the physical I/O access to a storage device.
 */
typedef struct storage_driver_S 
{                       
	int (*init)(PDISK_T *);
	int (*read)(PDISK_T *, unsigned int, int, unsigned char *);
	int (*write)(PDISK_T *, unsigned int, int, unsigned char *, char);
	int	(*ioctl)(PDISK_T *, int, void *);
} 
#undef STORAGE_DRIVER_T
	STORAGE_DRIVER_T;

#define BLOCKING_WRITE		0
#define NON_BLOCKING_WRITE	1

/*
 *  Storage I/O control
 */
#define STORAGE_IOCTL_EJECT_DOOR   	1	/* eject door */
#define STORAGE_IOCTL_CLOSE_DOOR   	2   /* close door */
#define STORAGE_IOCTL_LOCK_DOOR   	3   /* lock door */
#define STORAGE_IOCTL_FORMAT   		11  /* format disk */
#define STORAGE_IOCTL_SPEED   		21  /* speed control */
#define STORAGE_IOCTL_POWER_MODE	31  /* power saving control */


typedef struct pt_rec		/* partition record */
{
	unsigned char       ucState;			/* Current state of partition */
	unsigned char       uStartHead;			/* Beginning of partition head */
	unsigned char       ucStartSector;		/* Beginning of partition sector */
	unsigned char       ucStartCylinder;	/* Beginning of partition cylinder */
	unsigned char       ucPartitionType;	/* Partition type, refer to the subsequent definitions */
	unsigned char       ucEndHead;			/* End of partition - head */
	unsigned char       ucEndSector;		/* End of partition - sector */
	unsigned char       ucEndCylinder;		/* End of partition - cylinder */
	unsigned int      uFirstSec;			/* Number of Sectors Between the MBR and the First Sector in the Partition */
	unsigned int		uNumOfSecs;			/* Number of Sectors in the Partition */
}	PT_REC_T;


typedef struct physical_disk_S
{
	int			nDiskType;
	CHAR		szManufacture[32];	/* Human readable string descibing this disk */
	CHAR		szProduct[32];		/* Human readable string descibing this disk */
	CHAR		szSerialNo[64];
	int			nCylinderNum;
	int			nHeadNum;
	int			nSectorNum;
	unsigned int		uTotalSectorN;
	int			nSectorSize;
	unsigned int		uDiskSize;			/* disk size in Kbytes */
	int			nPartitionN;		/* number of partitions */
	PARTITION_T	*ptPartList;		/* partition list */
	int			nPreferDriveNo;

	/* file system internal use */
	PDISK_T		*ptSelf;			
	STORAGE_DRIVER_T *ptDriver;  
	PDISK_T		*ptPDiskAllLink;	/* link for all physical disk */
	void		*pvPrivate; 		/* driver-specific client data */

	void 		*os_mutex;			/* for OS                                             */
	void 		*os_priv;			/* for OS                                             */
}	
#undef PDISK_T
	PDISK_T;


typedef struct partition_S
{
	/* The following 16 bytes are a direct mapping of paration record */
	unsigned char       ucState;			/* Current state of partition */
	unsigned char       ucStartHead;		/* Beginning of partition head */
	unsigned char       ucStartSector;		/* Beginning of partition sector */
	unsigned char       ucStartCylinder;	/* Beginning of partition cylinder */
	unsigned char       ucPartitionType;	/* Partition type, refer to the subsequent definitions */
	unsigned char       ucEndHead;			/* End of partition - head */
	unsigned char       ucEndSector;		/* End of partition - sector */
	unsigned char       ucEndCylinder;		/* End of partition - cylinder */
	unsigned int      uFirstSec;			/* Number of Sectors Between the MBR and the First Sector in the Partition */
	unsigned int		uNumOfSecs;			/* Number of Sectors in the Partition */

	/* Please use the followings */
	int			nSectorN;			/* sectors per track, -1 means unknown */
	int			nHeadN;				/* number of heads, -1 means unknown */
	int			nCylinderN;			/* number of cylinders, -1 means unknown */
	unsigned int		uPartRecSecN;		/* logical sector number of the sector where partition record resides */
	unsigned int		uStartSecN;			/* Beginning logical sector number of this partition */
	unsigned int		uTotalSecN;			/* Total number of sectors in this partition */
	int			nErrorCode;			/* error on this partition */
	LDISK_T		*ptLDisk;			/* the only on logical disk on this partition, if exist */
	PARTITION_T	*ptNextPart;		/* link to the next partition */
}	
#undef PARTITION_T
	PARTITION_T;



/* Here the disk drive is a logical disk drive */
typedef struct logical_disk_S
{
	PDISK_T		*ptPDisk;			/* the physical disk it was located */
	int    		nDriveNo;			/* 'A', 'B', ... 'Z' */
	unsigned char   	ucFileSysType;
	char	   	bIsDiskRemoved;		/* set is this disk has been removed but not unmounted */
	CHAR    	szVolLabel[16];
	unsigned int 		uDiskSize;			/* disk size counted by sectors */
	unsigned int		uFreeSpace;			/* free space counted by sectors */
	FAT_INFO_T  tFatInfo;        
	DISK_OP_T  	*ptDiskOP;			/* disk operations */
	FILE_OP_T   *ptFileOP;			/* file operations */
	FILE_T   	*ptFileList;        /* files opened on this disk */
	LDISK_T  	*ptLinkForAllLDisk;	/* link used for all logical disk chain */

	void 		*os_mutex;			/* for OS                                             */
	void 		*os_priv;			/* for OS                                             */
}	
#undef LDISK_T
 	LDISK_T;

typedef struct fdrv_t
{
	int		nDiskType;
	int		nDiskInstance;
	int		nDriveNo;
	int		nPartitionNo;
	PDISK_T	*ptPDisk;
}  	FDRV_T;


/*
 *  File system operqations
 *  These service routines are provided by a specific file system. They are used
 *  to fulfill operations on a specific file system. They are system level 
 *  operations, instead of file level operations.
 */
typedef struct disk_op_S
{
	int (*delfile)(LDISK_T *, CHAR *, CHAR *);	/* delete a file */
	int (*mkdir)(LDISK_T *, CHAR *, CHAR *);	/* create a new directory */
	int (*rmdir)(LDISK_T *, CHAR *, CHAR *);	/* remove a existent directory */
	int (*rename)(LDISK_T *, CHAR *, CHAR *, CHAR *, CHAR *, char);	/* rename file/directory */
	int (*move)(LDISK_T *, CHAR *, CHAR *, CHAR *, CHAR *, char);	/* move file/directory */
	int (*volume_label)(LDISK_T *, CHAR *, CHAR *);	/* get/change disk volume label */
	//int (*format)(LDISK_T *);				/* format this disk */
	//int (*scan_disk)(LDISK_T *);			/* scan this disk */
	//int (*defragment)(LDISK_T *);			/* defragment this disk */
}
#undef DISK_OP_T
	DISK_OP_T;


/*
 *  File operations
 *  These service routines are provided by a specific file system. They are used
 *  to fulfill operations on a specific file opened on a file system. They are
 *  file level operations, instead of system level operations.
 */
typedef struct file_op_S
{
	int (*fopen)(FILE_T *, CHAR *, CHAR *);		/* open a file */
	int (*fread)(FILE_T *, unsigned char *, int, int *);/* read data from file */
	int (*fwrite)(FILE_T *, unsigned char *, int, int*);/* write data to file */
	int (*fclose)(FILE_T *);              		/* close file */
	int (*fsizing)(FILE_T *, __int64);			/* change file size */
	__int64 (*fseek)(FILE_T *, __int64);			/* setting the file position pointer */
	int (*fget_stat)(FILE_T *, FILE_STAT_T *); 	/* get file status */
	int (*fset_stat)(FILE_T *, FILE_STAT_T *); 	/* set file status */
	int (*find_first)(FILE_T *, FILE_FIND_T *);
	int (*find_next)(FILE_T *, FILE_FIND_T *);
	int (*find_close)(FILE_T *, FILE_FIND_T *);
	int (*fioctl)(FILE_T *, int, void *); 		/* file system specific control */
} 
#undef FILE_OP_T
	FILE_OP_T;



typedef struct file_S 
{
	//CHAR		suFileName[MAX_PATH_LEN+2];	/* full path file name */
	CHAR		szFnameAscii[MAX_FILE_NAME_LEN/2];
	unsigned int   	uFlag;                   
	LDISK_T  	*ptLDisk;
	FAT_FCB_T  	tFatFcb;              	/* File Control Block used by FAT file system */
	FILE_OP_T   *ptFileOP;				/* file operations */
	FILE_T  	*ptFileAllLink;	       	/* used for global file link */
	FILE_T  	*ptFileDiskLink;      	/* used for file link on disk */
} 
#undef FILE_T
	FILE_T;                    


typedef enum file_sorting_t
{
	NO_SORTING = 0,						/* without any file sorting */
	NAME_INCREASING,                    /* sorting by file name in increasing order */
	NAME_DECREASING,                    /* sorting by file name in decreasing order */ 
	CDATE_INCREASING,                   /* sorting by file creation date in increasing order */
	CDATE_DECREASING,                   /* sorting by file creation date in decreasing order */
	MDATE_INCREASING,                   /* sorting by file modification date in increasing order */
	MDATE_DECREASING,                   /* sorting by file modification date in decreasing order */
	SIZE_INCREASING,                    /* sorting by file size in increasing order */
	SIZE_DECREASING                     /* sorting by file size in decreasing order */
}   FILE_SORTING_T;


typedef struct file_find_S 
{
	int			hFile;
	CHAR		suLongName[MAX_FILE_NAME_LEN+2];
	CHAR		szShortName[14];
	unsigned char    	ucAttrib;
	unsigned char    	ucCTimeMS;        	/* create time minisecond */
	unsigned char    	ucCTimeSec;         /* create time */
	unsigned char    	ucCTimeMin;
	unsigned char    	ucCTimeHour;
	unsigned char    	ucCDateDay;         /* create date */
	unsigned char    	ucCDateMonth;
	unsigned char    	ucCDateYear;
	unsigned char    	ucLDateDay;         /* last access date */
	unsigned char    	ucLDateMonth;
	unsigned char    	ucLDateYear;
	unsigned char    	ucWTimeSec;			/* write time */
	unsigned char    	ucWTimeMin;
	unsigned char    	ucWTimeHour;
	unsigned char    	ucWDateDay;			/* write date */
	unsigned char    	ucWDateMonth;
	unsigned char    	ucWDateYear;
	__int64		n64FileSize;
	
	CHAR        *suPattern[8];
	char        bIsPatternExcludeDir;  /* pattern rule did not apply to directory */
	FILE_SORTING_T  eFileSorting;
	char        bIsDirFirst;		/* only valid if eFileSorting != NO_SORTING */
	                                /* TRUE: direcotry will be searched first */

	/* The following members are library internal used, user MUST NOT modified them! */
	char        bIsAllDirSearched;
	char		bHasSearchPattern;
	int			nCandidatePos;
	int			nLastChoicePos;
	struct file_find_S *ptCandidate;
	struct file_find_S *ptLastChoice;
} 
#undef FILE_FIND_T
	FILE_FIND_T;


typedef struct file_stat_S
{
	unsigned int   	uFlag;              /* forwarded from FAL */
	unsigned char    	ucAttrib;			/* file attribute */
	unsigned char       ucDirNTRes;
	unsigned char    	ucCTimeMs;        	/* create time minisecond */
	unsigned char    	ucCTimeSec;         /* create time */
	unsigned char    	ucCTimeMin;
	unsigned char    	ucCTimeHour;
	unsigned char    	ucCDateDay;         /* create date */
	unsigned char    	ucCDateMonth;
	unsigned char    	ucCDateYear;
	unsigned char    	ucLDateDay;          /* last access date */
	unsigned char    	ucLDateMonth;
	unsigned char    	ucLDateYear;
	unsigned char    	ucWTimeSec;			/* write time */
	unsigned char    	ucWTimeMin;
	unsigned char    	ucWTimeHour;
	unsigned char    	ucWDateDay;			/* write date */
	unsigned char    	ucWDateMonth;
	unsigned char    	ucWDateYear;
	unsigned int		uDev;				/* ptLDisk value to fake Linux dev */
	unsigned int		uUnique;				/* a unique value used to fake Linux inode number */
	__int64		n64FilePos;
	__int64   	n64FileSize;
}
#undef FILE_STAT_T
	FILE_STAT_T;


/*===================================================== file mode and flag ==*/
/* File open mode */
#define O_RDONLY 		0x0001    	/* Open for read only*/
#define O_WRONLY 		0x0002      /* Open for write only*/
#define O_RDWR   		0x0003      /* Read/write access allowed.*/
#define O_APPEND     	0x0004      /* Seek to eof on each write*/
#define O_CREATE     	0x0008      /* Create the file if it does not exist.*/
#define O_TRUNC      	0x0010      /* Truncate the file if it already exists*/
#define O_EXCL       	0x0020      /* Fail if creating and already exists */
#define O_DIR  			0x0080      /* Open a directory file */
#define O_EXCLUSIVE		0x0200		/* Open a file with exclusive lock */
#define O_NODIRCHK		0x0400		/* no dir/file check */
#define O_FSEEK			0x1000		/* Open with cluster chain cache to enhance file seeking performance */
#define O_IOC_VER2		0x2000		
#define O_intERNAL		0x8000		

/* operation */
#define OP_READ        	0x0010000   /* can read */
#define OP_WRITE       	0x0020000   /* can write */
#define OP_RDWR        	0x0030000   /* can read/write */
#define OP_DELETE     	0x0100000   /* file deletion */
#define OP_SETTIME		0x0200000	/* set file time used */
#define OP_DIRTY    	0x0400000   /* file has ever been changed */
#define OP_ROOT			0x0800000	/* is root directory */
#define OP_NULL_RDONLY  0x1000000
#define OP_WAS_DEL		0x2000000	/* file has been deleted by other process */
#define OP_ERROR		0x8000000	/* file has been deleted by other process */

/* file attributes */
#define FA_RDONLY       0x01        /* Read only attribute */
#define FA_HIDDEN       0x02        /* Hidden file */
#define FA_SYSTEM       0x04        /* System file */
#define FA_LABEL        0x08        /* Volume label */
#define FA_DIR          0x10        /* Directory    */
#define FA_ARCHIVE      0x20        /* Archive */

#define SEEK_SET        0
#define SEEK_CUR        1
#define SEEK_END        2


/* scan disk stage */
#define SDS_SCAN_FAT		1
#define SDS_BACKUP_FAT		2
#define SDS_SCAN_DIR		3
#define SDS_RECLAIM_CLUS	4
#define SDS_DUP_FAT			5

typedef int (FS_DW_CB)(unsigned char *);
typedef int (FS_SD_CB)(int);
typedef void (FS_DEL_CB_T)(PDISK_T *, unsigned int, int);
typedef void (FS_MOUNT_CB_T)(LDISK_T *);
typedef void (FS_UNMOUNT_CB_T)(LDISK_T *);


/*===================================================== Exported Functions ==*/
/* NVTFAT File System APIs */
extern int  fsInitFileSystem(void);
extern int  fsAssignDriveNumber(int nDriveNo, int disk_type, int instance, int partition);
extern void fsInstallIoWriteCallBack(FS_DW_CB *cb_func);
extern void fsInstallFileDelCallBack(FS_DEL_CB_T *cb_func);

/* Disk operations */
extern int  fsUnmountPhysicalDisk(PDISK_T *ptPDisk);
extern int  fsMountLogicalDisk(LDISK_T *ptLDisk);
extern int  fsUnmountLogicalDisk(LDISK_T *ptLDisk);
extern int  fsDiskFreeSpace(int nDriveNo, unsigned int *puBlockSize, unsigned int *puFreeSize, unsigned int *puDiskSize);
extern PDISK_T  *fsGetFullDiskInfomation(void);
extern void fsReleaseDiskInformation(PDISK_T *ptPDiskList);
extern int  fsScanDisk(int nDriveNo);
extern int  fsFormatFlashMemoryCard(PDISK_T *ptPDisk);
extern int  fsFormatFixedDrive(int nDriveNo);
extern int  fsFormatDisk(int disk_type, int nand_part);
extern int  fsTwoPartAndFormatAll(PDISK_T *ptPDisk, int firstPartSize, int secondPartSize);
extern int  fsCreateDiskPartition(PDISK_T *ptPDisk, unsigned int *puSizeList, int nCount);			/* 2011-0621 add for multiple partition */ 
extern int  fsSetReservedArea(unsigned int uStartSecter);							/* 2011-0624 add for reserved area */				

extern void fsDiskWriteComplete(unsigned char *pucBuff);
extern int  fsGetLDiskVolID(int nDriveNo, unsigned int *uVolID);
extern int  fsSetVolumeLabel(int nDriveNo, CHAR *szLabel, int nLen);
extern int  fsGetVolumeLabel(int nDriveNo, CHAR *szLabel, int nLen);
extern int  get_vdisk(int nDriveNo, LDISK_T **plDisk);

/* File operations */
extern int  fsOpenFile(CHAR *suFileName, CHAR *szAsciiName, unsigned int uFlag);
extern int  fsCloseFile(int hFile);
extern int  fsReadFile(int hFile, unsigned char *pucPtr, int nBytes, int *pnReadCnt);
extern int  fsWriteFile(int hFile, unsigned char *pucBuff, int nBytes, int *pnWriteCnt);
extern __int64  fsFileSeek(int hFile, __int64 n64Offset, short usWhence);
extern char	fsIsEOF(int hFile);
extern int  fsSetFileSize(int hFile, CHAR *suFileName, CHAR *szAsciiName, __int64 n64NewSize);
extern int  fsGetFilePosition(int hFile, unsigned int *puPos);
extern __int64  fsGetFileSize(int hFile);
extern int  fsGetFileStatus(int hFile, CHAR *suFileName, CHAR *szAsciiName, FILE_STAT_T *ptFileStat);
extern int  fsSetFileStatus(int hFile, CHAR *suFileName, CHAR *szAsciiName, FILE_STAT_T *ptFileStat);
extern int  fsSetFileAttribute(int hFile, CHAR *suFileName, CHAR *szAsciiName, unsigned char ucAttrib, FILE_STAT_T *ptFileStat);
extern int  fsSetFileTime(int hFile, CHAR *suFileName, CHAR *szAsciiName, unsigned char ucYear, unsigned char ucMonth, unsigned char ucDay, unsigned char ucHour, unsigned char unMin, unsigned char ucSec);
extern int  fsMergeFile(CHAR *suFileNameA, CHAR *szAsciiNameA, CHAR *suFileNameB, CHAR *szAsciiNameB);
extern int  fsDeleteFile(CHAR *suFileName, CHAR *szAsciiName);
extern int  fsRenameFile(CHAR *suOldName, CHAR *szOldAsciiName, CHAR *suNewName, CHAR *szNewAsciiName, char bIsDirectory);
extern int  fsMoveFile(CHAR *suOldName, CHAR *szOldAsciiName, CHAR *suNewName, CHAR *szNewAsciiName, int bIsDirectory);
extern int  fsCopyFile(CHAR *suSrcName, CHAR *szSrcAsciiName, CHAR *suDstName, CHAR *szDstAsciiName);
extern int	fsFindFirst(CHAR *suDirName, CHAR *szAsciiName, FILE_FIND_T *ptFindObj);
extern int  fsFindNext(FILE_FIND_T *ptFindObj);
extern int	fsFindClose(FILE_FIND_T *ptFindObj);
extern char fsIsFileOpened(CHAR *suFileName);

/* Directory operations */
extern int  fsMakeDirectory(CHAR *suDirName, CHAR *szAsciiName);
extern int  fsRemoveDirectory(CHAR *suDirName, CHAR *szAsciiName);
extern int  fsDeleteDirTree(CHAR *suDirName, CHAR *szAsciiName);
extern int  fsGetDirectoryInfo(CHAR *suDirName, CHAR *szAsciiName,
						int *nFileCnt, int *nDirCnt, unsigned __int64 *u64TotalSize, char bSearchSubDir);
extern int  fsGetDirectoryInfo2(CHAR *suDirName, CHAR *szAsciiName, CHAR **suPattern,
						int *pnFileCnt, int *pnDirCnt, unsigned __int64 *u64TotalSize, char bSearchSubDir);

/* language support */
extern CHAR fsToUpperCase(CHAR chr);
extern int  fsUnicodeToAscii(void *pvUniStr, void *pvASCII, char bIsNullTerm);
extern int  fsAsciiToUnicode(void *pvASCII, void *pvUniStr, char bIsNullTerm);
extern void fsAsciiToUpperCase(void *pvASCII);
extern int  fsAsciiNonCaseCompare(void *pvASCII1, void *pvASCII2);
extern void fsUnicodeToUpperCase(void *pvUnicode);
extern void fsUnicodeToLowerCase(void *pvUnicode);
extern int  fsUnicodeStrLen(void *pvUnicode);
extern int  fsUnicodeNonCaseCompare(void *pvUnicode1, void *pvUnicode2);
extern int  fsUnicodeCopyStr(void *pvStrDst, void *pvStrSrc);
extern int  fsUnicodeStrCat(void *pvUniStrHead, void *pvUniStrTail);
extern void fsGetAsciiFileName(void *pvUniStr, void *pvAscii);
extern int  fsUnicodeWildCardCompare(CHAR *suStr1, CHAR *suStr2);

/* Driver supporting routines */
extern int  fsPhysicalDiskConnected(PDISK_T *ptPDisk);
extern int  fsPhysicalDiskDisconnected(PDISK_T *ptPDisk);


/* For debug and internal use, not exported funcions */
//extern CHAR *fsFindFirstSlash(CHAR *szFullName);
extern CHAR *fsFindLastSlash(CHAR *suFullName);
extern int  fsTruncatePath(CHAR *szPath, CHAR *szToken);
extern void  fsTrimEndingSpace(CHAR *szStr);
extern FILE_T *fsHandleToDescriptor(int hFile);
extern int  fsDescriptorToHandle(FILE_T *ptFile);

extern void fsDumpBufferHex(unsigned char *pucBuff, int nSize);
extern void fsDumpSectorHex(int uSectorNo, unsigned char *pucBuff, int nSize);
extern int  fsDumpDiskSector(unsigned int uSectorNo, int nSecNum);

extern LDISK_T *fsAllocateDisk(PDISK_T *ptPDisk, PARTITION_T *ptPartition);
extern unsigned char *fsAllocateSector(void);
extern int  fsFreeSector(unsigned char *pucSecAddr);

extern CHAR  *fsDebugUniStr(CHAR *suStr);
//extern char fsIsValidLongEntName(CHAR *szDirEntName);
//extern char fsIsValidShortNameChar(CHAR cChar);

extern void  lname_to_sname(CHAR *szAsciiName, int nTildeNum, CHAR *szShortName);

extern int  fsFlushIOCache(void);
extern int  fsIOWrite(PDISK_T *ptPDisk, unsigned int uStartSecNo, int nCount, unsigned char *pucOutBuff);
extern int  fsIORead(PDISK_T *ptPDisk, unsigned int uStartSecNo, int nCount, unsigned char *in_buf);
extern void fs_enable_iow_cache(void);
extern int  fs_flush_iow_cache(void);
extern void *fsCheckDriverPointer(void *pvDrive);

#endif  /* _NVTFAT_H_ */
