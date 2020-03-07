#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wb_include.h"
#include "jpegcodec.h"
#include "nvtfat.h"
#include "w55fa93_sic.h"
#include "jpegSample.h"
#include "w55fa93_vpost.h"
#include "jpegSample.h"
#include "FreeRTOS.h"
#include "task.h"
#include "GUI_X.h"

extern CHAR g_u8String[100];
extern unsigned int g_u32StringIndex;
//CHAR decodePath[100];

#if (LCD_BITS_MODE == 16)
unsigned int g_u32DecFormat = JPEG_DEC_PRIMARY_PACKET_RGB565;			/* Decode Output format */
#elif ((LCD_BITS_MODE == 18) || (LCD_BITS_MODE == 24)) 
unsigned int g_u32DecFormat = JPEG_DEC_PRIMARY_PACKET_RGB888;			/* Decode Output format */
#endif

volatile unsigned char *g_pu8JpegBuffer;											/* The source bit stream data for decoding */
   
/*-----------------------------------------------------------------------*/
/*  Decode Input Wait parameter				                             */
/*-----------------------------------------------------------------------*/          
unsigned char __align(32) g_au8DecInputWaitBuffer[DEC_IPW_BUFFERSIZE];	/* Buffer for Decode Input Wait */
unsigned int g_u32IpwUsedSize  = 0;									/* Buffer Control parameter for Decode Input Wait (bitstream size put to Bitstream Buffer )*/
unsigned int g_u32BitstreamSize = 0;									/* JPEG Bitstream Size for Decode Input Wait */
     
/*-----------------------------------------------------------------------*/
/*  Decode Output Wait parameter				                         */
/*-----------------------------------------------------------------------*/          
unsigned char * apBuffer[100];											/* Decode Output buffer pointer array */
unsigned int g_u32OpwUsedSize;										/* JPEG Bitstream Size for Decode Input Wait */
unsigned int u32MCU_Line;												/* One MCU Line data size */
unsigned int u32TotalSize;											/* Total size for JPEG Decode Ouput */
unsigned int g_u32OpwBufferIndex = 0;									/* Decode output Buffer index */
/*-----------------------------------------------------------------------*/
/*  Decode Function          				                             */
/*-----------------------------------------------------------------------*/  
int nReadLen;
extern DX_LCD_COLOR __align(32) JpgBuff[_LCD_HEIGHT][_LCD_WIDTH];
extern volatile DX_LCD_COLOR   *_VpostFrameBuffer;
extern volatile LCD_BUFFER setBufferType;

//extern __align(32) unsigned short OSD_FrameRGB565[_LCD_HEIGHT*_LCD_WIDTH*2];
void JpegDec(void)
{
	//unsigned int i,j;
	unsigned int u32Width,u32Height,u32Format;	
	
	sysprintf("JpegDec\r\n");
	if(ParsingOldJPEG((unsigned char *)((unsigned int)g_pu8JpegBuffer | 0x80000000), nReadLen, &u32Width, &u32Height, &u32Format, TRUE) == ERR_MODE)
	{
		sysprintf("\tNot Support the JPEG sampling\n");	
    	goto END_ERROR_FORMAT;
	}
		 
/*******************************************************************/	
/* 						Decode JPEG Bitstream					   */
/*******************************************************************/	    

	/* JPEG Init */
	jpegInit();

	/* Set Bit stream Address */   
	jpegIoctl(JPEG_IOCTL_SET_BITSTREAM_ADDR,(unsigned int) g_pu8JpegBuffer, 0);	
   
	/* Decode mode */	
	jpegIoctl(JPEG_IOCTL_SET_DECODE_MODE, g_u32DecFormat, 0);	
 
	/* Set JPEG Header Decode End Call Back Function */
	jpegIoctl(JPEG_IOCTL_SET_HEADERDECODE_CALBACKFUN, (unsigned int) JpegOldDecHeaderComplete, 0);	

	/* Trigger JPEG decoder */
	jpegIoctl(JPEG_IOCTL_DECODE_TRIGGER, 0, 0);  

	/* Wait for complete */
	if(jpegWait()){
		}

END_ERROR_FORMAT:	
	
	return;
}
int JpegDecTest (char *filestr, LCD_BUFFER mode)
{	
	unsigned int u32BitstreamSize;
	int nStatus/*, nReadLen*/;
	int hFile=0;
	CHAR	suFileName[100];	
		
/*******************************************************************/	
/* 							Read JPEG file						   */
/*******************************************************************/		
	//GUI_X_Lock();	
	strcpy(decodePath, filestr);
	fsAsciiToUnicode(decodePath, suFileName, TRUE);
	
	hFile = fsOpenFile(suFileName, NULL, O_RDONLY);
	if (hFile > 0){
		sysprintf("\tOpen file:[%s], file handle:%d\n", decodePath, hFile);
	}else{
		sysprintf("\tFailed to open file: %s (%x)\n", decodePath, hFile);
		return 0 ;
	}	
		
	u32BitstreamSize = fsGetFileSize(hFile);
	sysprintf("\tBit stream  size for Decode is %d\n", u32BitstreamSize);
	/* Allocate the Bitstream Data Buffer for Decode Operation */	
	g_pu8JpegBuffer = (unsigned char *)malloc(sizeof(CHAR) * u32BitstreamSize);
	if(g_pu8JpegBuffer == NULL)
	{
		sysprintf("\t Malloc fail for jpeg dec!\n");
		return 0;
	}
    sysprintf("g_pu8JpegBuffer address:0x%x\n", g_pu8JpegBuffer);

	nStatus = fsReadFile(hFile, (unsigned char *)((unsigned int)g_pu8JpegBuffer | 0x80000000), u32BitstreamSize, &nReadLen);
	if (nStatus < 0)
		sysprintf("\tRead error!!\n");		
	fsCloseFile(hFile);  

	setBufferType = mode;
	JpegDec();
	free(g_pu8JpegBuffer);
	g_pu8JpegBuffer = NULL;
	sysprintf("½âÂëÍê³É!\n");
	//GUI_X_Unlock();
	return 1;
}


