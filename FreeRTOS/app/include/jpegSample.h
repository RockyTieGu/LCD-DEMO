/****************************************************************
 *                                                              *
 * Copyright (c) Nuvoton Technology Corp. All rights reserved.  *
 *                                                              *
 ****************************************************************/
#include "wbtypes.h"
#include "w55fa93_vpost.h"
#define DEC_IPW_BUFFERSIZE		8192				/* Buffer size for Decode Input Wait */						

/* ParsingJPEG Error Code */	
#define ERR_MODE		-1
#define ERR_FORMAT		-2        
#define ERR_SUCCESS		 0
#define ERR_ONLY_NORMAL	 1  

#define ENC_WIDTH		480					/* Encode Width	*/
#define ENC_HEIGHT		640					/* Encode Height */


/* Jpeg Output File Name Function parameter */
extern CHAR	decodePath[];

/* Test Control Flag */

extern unsigned int g_u32EncWidth, g_u32EncHeight;  

extern unsigned int g_u32DecFormat;

extern volatile unsigned char * g_pu8JpegBuffer;						/* The source bit stream data for decoding */
extern unsigned char * g_pu8DecFrameBuffer;					/* The buffer for decoding output */

        

extern unsigned char * g_pu8DecFrameBuffer;					/* The buffer for decoding output */
extern unsigned char __align(32) g_au8BitstreamBuffer[];	/* The buffer for encoding output */

/*-----------------------------------------------------------------------*/
/*  Decode Input Wait parameter				                             */
/*-----------------------------------------------------------------------*/   
extern unsigned char __align(32) g_au8DecInputWaitBuffer[];	/* Buffer for Decode Input Wait */
extern unsigned int g_u32IpwUsedSize;
extern unsigned int g_u32BitstreamSize;

/*-----------------------------------------------------------------------*/
/*  Decode Output Wait parameter				                         */
/*-----------------------------------------------------------------------*/          
extern unsigned char * apBuffer[];							/* Decode Output buffer pointer array */
extern unsigned int g_u32OpwUsedSize;						/* JPEG Bitstream Size for Decode Input Wait */
extern unsigned int u32MCU_Line;							/* One MCU Line data size */
extern unsigned int u32TotalSize;							/* Total size for JPEG Decode Ouput */
extern unsigned int g_u32OpwBufferIndex;				/* Decode output Buffer index */



#define PLANAR_DEC_BUFFER_SIZE	0x100000	/* Decode Output Buffer size for Planar */
               	

  
int JpegDecTest (char *filestr, LCD_BUFFER mode);

/* Jpeg Output File Name Function */
CHAR *intToStr(unsigned int u32quotient);
/*  Header Decode Complete Callback function */
char JpegOldDecHeaderComplete(void);
/* Decode Input Wait Callback function */   
char JpegDecInputWait(unsigned int u32Address,unsigned int u32Size);
/* Decode Output Wait Callback function */   
char JpegDecOutputWait(unsigned int u32Address, unsigned int u32Size);

void JpedInitDecOutputWaitBuffer(unsigned int u32Width,unsigned int u32Height, unsigned int u32jpegFormat);

int ParsingOldJPEG(unsigned char * JPEG_Buffer,unsigned int Length, unsigned int * pu32Width, unsigned int * pu32Height, unsigned int * u32Format,char bPrimary);
void JpegDec(void);
unsigned int GetData(void);
void GetString(void);
