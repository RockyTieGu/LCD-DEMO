#include "w55fa93_vpost.h"
#include "w55fa93_spi.h"
#include "w55fa93_osd.h"
#include "w55fa93_edma.h"
#include "DrvEDMA.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "blt.h"
#include "FI.h"
#include "Nvtfat.h"
#include "FreeRTOS.h"
#include "task.h"

//#define	BLT_EN

void * g_VAFrameBuf = NULL;
void * g_VAOrigFrameBuf = NULL;
unsigned short POint_COLOR,BACK_COLOR;
int g_VdmaCh = 0;
static volatile BOOL g_bEdmaInt = FALSE;
extern volatile DX_LCD_COLOR   *_VpostFrameBuffer;
unsigned char	Font_Hspace=2;
unsigned char	Font_Vspace=0;
int	rsFil;
#define FIX_FONT_W 8
#define FIX_FONT_H 16
extern  unsigned char	Font_Hspace;
extern unsigned char	Font_Vspace;
extern unsigned int gDisplayFormat;
/*8*16字符*/
unsigned char  const fix_ascii[]=
{
/*   */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/* ! */
0x00,0x00,0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,0x18,0x18,0x00,0x00,

/* " */
0x00,0x12,0x36,0x24,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/* # */
0x00,0x00,0x00,0x24,0x24,0x24,0xFE,0x48,0x48,0x48,0xFE,0x48,0x48,0x48,0x00,0x00,

/* $ */
0x00,0x00,0x10,0x38,0x54,0x54,0x50,0x30,0x18,0x14,0x14,0x54,0x54,0x38,0x10,0x10,

/* % */
0x00,0x00,0x00,0x44,0xA4,0xA8,0xA8,0xA8,0x54,0x1A,0x2A,0x2A,0x2A,0x44,0x00,0x00,

/* & */
0x00,0x00,0x00,0x30,0x48,0x48,0x48,0x50,0x6E,0xA4,0x94,0x88,0x89,0x76,0x00,0x00,

/* ' */
0x00,0x60,0x60,0x20,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/* ( */
0x00,0x02,0x04,0x08,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x08,0x08,0x04,0x02,0x00,

/* ) */
0x00,0x40,0x20,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x10,0x20,0x40,0x00,

/* * */
0x00,0x00,0x00,0x00,0x10,0x10,0xD6,0x38,0x38,0xD6,0x10,0x10,0x00,0x00,0x00,0x00,

/* + */
0x00,0x00,0x00,0x00,0x10,0x10,0x10,0x10,0xFE,0x10,0x10,0x10,0x10,0x00,0x00,0x00,

/* , */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x20,0xC0,

/* - */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/* . */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x00,0x00,

/* / */
0x00,0x00,0x01,0x02,0x02,0x04,0x04,0x08,0x08,0x10,0x10,0x20,0x20,0x40,0x40,0x00,

/* 0 */
0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x24,0x18,0x00,0x00,

/* 1 */
0x00,0x00,0x00,0x10,0x70,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00,

/* 2 */
0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x04,0x04,0x08,0x10,0x20,0x42,0x7E,0x00,0x00,

/* 3 */
0x00,0x00,0x00,0x3C,0x42,0x42,0x04,0x18,0x04,0x02,0x02,0x42,0x44,0x38,0x00,0x00,

/* 4 */
0x00,0x00,0x00,0x04,0x0C,0x14,0x24,0x24,0x44,0x44,0x7E,0x04,0x04,0x1E,0x00,0x00,

/* 5 */
0x00,0x00,0x00,0x7E,0x40,0x40,0x40,0x58,0x64,0x02,0x02,0x42,0x44,0x38,0x00,0x00,

/* 6 */
0x00,0x00,0x00,0x1C,0x24,0x40,0x40,0x58,0x64,0x42,0x42,0x42,0x24,0x18,0x00,0x00,

/* 7 */
0x00,0x00,0x00,0x7E,0x44,0x44,0x08,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,

/* 8 */
0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x24,0x18,0x24,0x42,0x42,0x42,0x3C,0x00,0x00,

/* 9 */
0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x26,0x1A,0x02,0x02,0x24,0x38,0x00,0x00,

/* : */
0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,

/* ; */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x20,

/* < */
0x00,0x00,0x00,0x02,0x04,0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x00,

/* = */
0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,

/* > */
0x00,0x00,0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x00,

/* ? */
0x00,0x00,0x00,0x3C,0x42,0x42,0x62,0x02,0x04,0x08,0x08,0x00,0x18,0x18,0x00,0x00,

/* @ */
0x00,0x00,0x00,0x38,0x44,0x5A,0xAA,0xAA,0xAA,0xAA,0xB4,0x42,0x44,0x38,0x00,0x00,

/* A */
0x00,0x00,0x00,0x10,0x10,0x18,0x28,0x28,0x24,0x3C,0x44,0x42,0x42,0xE7,0x00,0x00,

/* B */
0x00,0x00,0x00,0xF8,0x44,0x44,0x44,0x78,0x44,0x42,0x42,0x42,0x44,0xF8,0x00,0x00,

/* C */
0x00,0x00,0x00,0x3E,0x42,0x42,0x80,0x80,0x80,0x80,0x80,0x42,0x44,0x38,0x00,0x00,

/* D */
0x00,0x00,0x00,0xF8,0x44,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x44,0xF8,0x00,0x00,

/* E */
0x00,0x00,0x00,0xFC,0x42,0x48,0x48,0x78,0x48,0x48,0x40,0x42,0x42,0xFC,0x00,0x00,

/* F */
0x00,0x00,0x00,0xFC,0x42,0x48,0x48,0x78,0x48,0x48,0x40,0x40,0x40,0xE0,0x00,0x00,

/* G */
0x00,0x00,0x00,0x3C,0x44,0x44,0x80,0x80,0x80,0x8E,0x84,0x44,0x44,0x38,0x00,0x00,

/* H */
0x00,0x00,0x00,0xE7,0x42,0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x42,0xE7,0x00,0x00,

/* I */
0x00,0x00,0x00,0x7C,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00,

/* J */
0x00,0x00,0x00,0x3E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x88,0xF0,

/* K */
0x00,0x00,0x00,0xEE,0x44,0x48,0x50,0x70,0x50,0x48,0x48,0x44,0x44,0xEE,0x00,0x00,

/* L */
0x00,0x00,0x00,0xE0,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x42,0xFE,0x00,0x00,

/* M */
0x00,0x00,0x00,0xEE,0x6C,0x6C,0x6C,0x6C,0x54,0x54,0x54,0x54,0x54,0xD6,0x00,0x00,

/* N */
0x00,0x00,0x00,0xC7,0x62,0x62,0x52,0x52,0x4A,0x4A,0x4A,0x46,0x46,0xE2,0x00,0x00,

/* O */
0x00,0x00,0x00,0x38,0x44,0x82,0x82,0x82,0x82,0x82,0x82,0x82,0x44,0x38,0x00,0x00,

/* P */
0x00,0x00,0x00,0xFC,0x42,0x42,0x42,0x42,0x7C,0x40,0x40,0x40,0x40,0xE0,0x00,0x00,

/* Q */
0x00,0x00,0x00,0x38,0x44,0x82,0x82,0x82,0x82,0x82,0xB2,0xCA,0x4C,0x38,0x06,0x00,

/* R */
0x00,0x00,0x00,0xFC,0x42,0x42,0x42,0x7C,0x48,0x48,0x44,0x44,0x42,0xE3,0x00,0x00,

/* S */
0x00,0x00,0x00,0x3E,0x42,0x42,0x40,0x20,0x18,0x04,0x02,0x42,0x42,0x7C,0x00,0x00,

/* T */
0x00,0x00,0x00,0xFE,0x92,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x38,0x00,0x00,

/* U */
0x00,0x00,0x00,0xE7,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00,

/* V */
0x00,0x00,0x00,0xE7,0x42,0x42,0x44,0x24,0x24,0x28,0x28,0x18,0x10,0x10,0x00,0x00,

/* W */
0x00,0x00,0x00,0xD6,0x92,0x92,0x92,0x92,0xAA,0xAA,0x6C,0x44,0x44,0x44,0x00,0x00,

/* X */
0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x18,0x18,0x18,0x24,0x24,0x42,0xE7,0x00,0x00,

/* Y */
0x00,0x00,0x00,0xEE,0x44,0x44,0x28,0x28,0x10,0x10,0x10,0x10,0x10,0x38,0x00,0x00,

/* Z */
0x00,0x00,0x00,0x7E,0x84,0x04,0x08,0x08,0x10,0x20,0x20,0x42,0x42,0xFC,0x00,0x00,

/* [ */
0x00,0x1E,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x1E,0x00,

/* \ */
0x00,0x00,0x40,0x40,0x20,0x20,0x10,0x10,0x10,0x08,0x08,0x04,0x04,0x04,0x02,0x02,

/* ] */
0x00,0x78,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x78,0x00,

/* ^ */
0x00,0x1C,0x22,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/* _ */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,

/* ` */
0x00,0x60,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/* a */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x1E,0x22,0x42,0x42,0x3F,0x00,0x00,

/* b */
0x00,0x00,0x00,0xC0,0x40,0x40,0x40,0x58,0x64,0x42,0x42,0x42,0x64,0x58,0x00,0x00,

/* c */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x22,0x40,0x40,0x40,0x22,0x1C,0x00,0x00,

/* d */
0x00,0x00,0x00,0x06,0x02,0x02,0x02,0x1E,0x22,0x42,0x42,0x42,0x26,0x1B,0x00,0x00,

/* e */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x7E,0x40,0x40,0x42,0x3C,0x00,0x00,

/* f */
0x00,0x00,0x00,0x0F,0x11,0x10,0x10,0x7E,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00,

/* g */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x44,0x44,0x38,0x40,0x3C,0x42,0x42,0x3C,

/* h */
0x00,0x00,0x00,0xC0,0x40,0x40,0x40,0x5C,0x62,0x42,0x42,0x42,0x42,0xE7,0x00,0x00,

/* i */
0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x70,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00,

/* j */
0x00,0x00,0x00,0x0C,0x0C,0x00,0x00,0x1C,0x04,0x04,0x04,0x04,0x04,0x04,0x44,0x78,

/* k */
0x00,0x00,0x00,0xC0,0x40,0x40,0x40,0x4E,0x48,0x50,0x68,0x48,0x44,0xEE,0x00,0x00,

/* l */
0x00,0x00,0x00,0x70,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00,

/* m */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x49,0x49,0x49,0x49,0x49,0xED,0x00,0x00,

/* n */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xDC,0x62,0x42,0x42,0x42,0x42,0xE7,0x00,0x00,

/* o */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00,

/* p */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD8,0x64,0x42,0x42,0x42,0x44,0x78,0x40,0xE0,

/* q */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x22,0x42,0x42,0x42,0x22,0x1E,0x02,0x07,

/* r */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEE,0x32,0x20,0x20,0x20,0x20,0xF8,0x00,0x00,

/* s */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3E,0x42,0x40,0x3C,0x02,0x42,0x7C,0x00,0x00,

/* t */
0x00,0x00,0x00,0x00,0x00,0x10,0x10,0x7C,0x10,0x10,0x10,0x10,0x10,0x0C,0x00,0x00,

/* u */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC6,0x42,0x42,0x42,0x42,0x46,0x3B,0x00,0x00,

/* v */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x28,0x10,0x10,0x00,0x00,

/* w */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xD7,0x92,0x92,0xAA,0xAA,0x44,0x44,0x00,0x00,

/* x */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x6E,0x24,0x18,0x18,0x18,0x24,0x76,0x00,0x00,

/* y */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x28,0x18,0x10,0x10,0xE0,

/* z */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x44,0x08,0x10,0x10,0x22,0x7E,0x00,0x00,

/* { */
0x00,0x03,0x04,0x04,0x04,0x04,0x04,0x08,0x04,0x04,0x04,0x04,0x04,0x04,0x03,0x00,

/* | */
0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,

/* } */
0x00,0x60,0x10,0x10,0x10,0x10,0x10,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x60,0x00,

/* ~ */
0x30,0x4C,0x43,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
int vpostLCMInit(PLCDFORMATEX plcdformatex, unsigned int *pFramebuf)
{
	return vpostLCMInit_CENTRY(plcdformatex, pFramebuf);
}

int vpostLCMDeinit(void)
{
	fsCloseFile(rsFil);
	return vpostLCMDeinit_CENTRY();
}

void* vpostGetFrameBuffer(void)
{
    return g_VAFrameBuf;
}

void vpostSetFrameBuffer(unsigned int pFramebuf)
{ 
	g_VAFrameBuf = (void *)pFramebuf;
	g_VAFrameBuf = (void*)((unsigned int)g_VAFrameBuf | 0x80000000);
    outpw(REG_LCM_FSADDR, (unsigned int)pFramebuf);
}


void LCDDelay(unsigned int nCount)
{
	unsigned volatile int i;
		
	for(;nCount!=0;nCount--)
		for(i=0;i<10;i++);
}

void EdmaIrqHandler(unsigned int arg)
{ 	
	EDMA_Free(g_VdmaCh);  	
	g_bEdmaInt = TRUE;
}

void LCDUpdateScreen(unsigned short	buff)
{
	
	volatile UINT32 src_addr, dest_addr;
	
	
	sysprintf("1_VpostFrameBuffer:%x \r\n", _VpostFrameBuffer);
	
	sys_flush_and_clean_dcache();
	dest_addr =(UINT32)_VpostFrameBuffer;//OSD_FrameRGB565;
	if(buff==BUFF0)src_addr=(UINT32)LCDOSDBuffer1| NON_CACHE_BIT;
	else if(buff==BUFF1)src_addr=(UINT32)LCDOSDBuffer2| NON_CACHE_BIT;
	#if 0 //播放视频结束后，不能用EDMA 
	//taskENTER_CRITICAL();
	g_VdmaCh = VDMA_FindandRequest();
	EDMA_SetDirection(g_VdmaCh , eDRVEDMA_DIRECTION_INCREMENTED, eDRVEDMA_DIRECTION_INCREMENTED);
	EDMA_SetupSingle(g_VdmaCh, src_addr, dest_addr, _LCD_WIDTH*_LCD_HEIGHT*sizeof(DX_LCD_COLOR));
	EDMA_SetupHandlers(g_VdmaCh, eDRVEDMA_BLKD_FLAG, EdmaIrqHandler, 0);
	EDMA_Trigger(g_VdmaCh);
	#else
	memcpy((UINT8 *)dest_addr, (UINT8 *)src_addr, _LCD_WIDTH*_LCD_HEIGHT*sizeof(DX_LCD_COLOR));
	#endif
	//taskEXIT_CRITICAL();
	sysprintf("8.0\n");
	//while(g_bEdmaInt == FALSE);	
	sysprintf("9.0\n");
	sysprintf("2_VpostFrameBuffer:%x \r\n", _VpostFrameBuffer);
	g_bEdmaInt = FALSE;	

}

void LCDQuickUpdateScreen(unsigned short	buff)
{

	volatile unsigned int src_addr, dest_addr;
	sys_flush_and_clean_dcache();
	dest_addr =(unsigned int)_VpostFrameBuffer;//OSD_FrameRGB565;
	if(buff==BUFF0)src_addr=(unsigned int)LCDOSDBuffer1| NON_CACHE_BIT;
	else if(buff==BUFF1)src_addr=(unsigned int)LCDOSDBuffer2| NON_CACHE_BIT;

	g_VdmaCh = VDMA_FindandRequest();
	EDMA_SetDirection(g_VdmaCh , eDRVEDMA_DIRECTION_INCREMENTED, eDRVEDMA_DIRECTION_INCREMENTED);
	EDMA_SetupSingle(g_VdmaCh, src_addr, dest_addr, _LCD_WIDTH*_LCD_HEIGHT*sizeof(DX_LCD_COLOR));
	EDMA_SetupHandlers(g_VdmaCh, eDRVEDMA_BLKD_FLAG, EdmaIrqHandler, 0);
	EDMA_Trigger(g_VdmaCh);
	//while(g_bEdmaInt == FALSE);	
	g_bEdmaInt = FALSE;	

}


void LCDBufferMove(unsigned int src_addr,unsigned int dest_addr,unsigned int size)
{
#ifdef	BLT_EN
	BlitBufferCopy(src_addr, dest_addr);
#else
	sys_flush_and_clean_dcache();
	g_VdmaCh = VDMA_FindandRequest();
	EDMA_SetDirection(g_VdmaCh , eDRVEDMA_DIRECTION_INCREMENTED, eDRVEDMA_DIRECTION_INCREMENTED);
	EDMA_SetupSingle(g_VdmaCh, src_addr| NON_CACHE_BIT, dest_addr, size);
	EDMA_SetupHandlers(g_VdmaCh, eDRVEDMA_BLKD_FLAG, EdmaIrqHandler, 0);
	EDMA_Trigger(g_VdmaCh);
	//while(g_bEdmaInt == FALSE);	
	g_bEdmaInt = FALSE;	
#endif	
}

void LCDUpdateRect(VPOST_RECT* rect,unsigned short	buff)
{
	unsigned int i,j;
	if(rect->x0>_LCD_WIDTH ||rect->x1>_LCD_WIDTH)return;
	if(rect->y0>_LCD_HEIGHT ||rect->y1>_LCD_HEIGHT)return;
#if 0//def	BLT_EN	
	if(buff==BUFF0)
		BlitRectCopy((unsigned int)LCDOSDBuffer1, (unsigned int)OSD_FrameRGB565, rect);
	else
		BlitRectCopy((unsigned int)LCDOSDBuffer2, (unsigned int)OSD_FrameRGB565, rect);
#else
	if(buff==0){
		for(i=rect->y0;i<rect->y1;i++){
			for(j=rect->x0;j<rect->x1;j++){
				_VpostFrameBuffer[i*_LCD_WIDTH+j]=LCDOSDBuffer1[i][j];
				}
			}	
		}
	else{
		for(i=rect->y0;i<rect->y1;i++){
			for(j=rect->x0;j<rect->x1;j++)
				_VpostFrameBuffer[i*_LCD_WIDTH+j]=LCDOSDBuffer2[i][j];
			}	
		}
#endif	
}

void UpdateBufferToLCD(LCDBUFFSRC src,VPOST_RECT* rect)
{
//	VPOST_RECT rect;
	unsigned int i,j;

#if 1
	for(i=rect->y0;i<rect->y1;i++){
		for(j=rect->x0;j<rect->x1;j++){
			_VpostFrameBuffer[i*_LCD_WIDTH+j] = src[i][j];
		}
	}
#else
	BlitRectCopy((unsigned int)src, (unsigned int)_VpostFrameBuffer, rect);
#endif
}
void LCDBUffMove221(VPOST_RECT* rect)
{
	unsigned int i,j;

	if(rect->x0>_LCD_WIDTH ||rect->x1>_LCD_WIDTH)return;
	if(rect->y0>_LCD_HEIGHT ||rect->y1>_LCD_HEIGHT)return;
#ifdef	BLT_EN		
	BlitRectCopy((unsigned int)LCDOSDBuffer2, (unsigned int)LCDOSDBuffer1, rect);
#else
	for(i=rect->y0;i<rect->y1;i++){
		for(j=rect->x0;j<rect->x1;j++)
			LCDOSDBuffer1[i][j]=LCDOSDBuffer2[i][j];
		}	
#endif	
}

void LCDBUffMove(LCDBUFFSRC src, LCDBUFFSRC dest, VPOST_RECT* rect)
{
	unsigned int i,j;

	if(rect->x0>_LCD_WIDTH ||rect->x1>_LCD_WIDTH)return;
	if(rect->y0>_LCD_HEIGHT ||rect->y1>_LCD_HEIGHT)return;
#ifdef	BLT_EN		
	BlitRectCopy((unsigned int)src, (unsigned int)dest, rect);
#else
	for(i=rect->y0;i<rect->y1;i++){
		for(j=rect->x0;j<rect->x1;j++)
			dest[i][j]=src[i][j];
		}	
#endif	
}

void LCDBUffMove112(VPOST_RECT* rect)
{
	unsigned int i,j;

	if(rect->x0>_LCD_WIDTH ||rect->x1>_LCD_WIDTH)return;
	if(rect->y0>_LCD_HEIGHT ||rect->y1>_LCD_HEIGHT)return;
#ifdef	BLT_EN		
	BlitRectCopy((unsigned int)LCDOSDBuffer2, (unsigned int)LCDOSDBuffer1, rect);
#else
	for(i=rect->y0;i<rect->y1;i++){
		for(j=rect->x0;j<rect->x1;j++)
			LCDOSDBuffer2[i][j]=LCDOSDBuffer1[i][j];
		}	
#endif	
}



unsigned short src[2];
void LCD_Clear(unsigned int color,unsigned short buff)
{
//#ifdef	BLT_EN
#if 0
	FillFrameBuffer((unsigned int)LCDOSDBuffer1, _LCD_WIDTH, _LCD_HEIGHT, gDisplayFormat, (color>>8)&0xF8, ((color>>5)&0x3F)<<2, (color&0x1f)<<3, 0xff);
#else
	unsigned int i,j;

		if(buff==BUFF0)
		{
		for(i=0;i<_LCD_HEIGHT;i++)
			for(j=0;j<_LCD_WIDTH;j++)
				LCDOSDBuffer1[i][j]=color;
		}else if(BUFF1 == buff){
		for(i=0;i<_LCD_HEIGHT;i++)
			for(j=0;j<_LCD_WIDTH;j++)
				LCDOSDBuffer2[i][j]=color;
		}
	
#endif
}
extern unsigned int	gDisplayFormat;
void Vpost_FillRect(unsigned short sx,unsigned short sy,unsigned short ex,unsigned short ey,unsigned short color,unsigned short buff)
{
//	unsigned int i,j;
	LCDBUFFSRC  src;
	
	if(sx>(_LCD_WIDTH) ||ex>(_LCD_WIDTH))return;
	if(sy>(_LCD_HEIGHT) ||ey>(_LCD_HEIGHT))return;
	if(sy > ey ||sx > ex)return;
#if 0
	if(buff==0){
		for(i=sy;i<ey;i++)
			for(j=sx;j<ex;j++)
				LCDOSDBuffer1[i][j]=color;
		}
	else{
		for(i=sy;i<ey;i++)
			for(j=sx;j<ex;j++)
				LCDOSDBuffer2[i][j]=color;
		}
#else

	if(buff == BUFF0) src = (LCDBUFFSRC)LCDOSDBuffer1;
	else if(buff == BUFF1) src = (LCDBUFFSRC)LCDOSDBuffer2;

//	fillRect((unsigned int)LCDOSDBuffer1, sx, sy, (ex -sx), (ey -sy), gDisplayFormat, (color>>8)&0xF8, ((color>>5)&0x3F)<<2, (color&0x1f)<<3, 0xff);
	//fillRect((unsigned int)src, sx, sy, ex, ey, gDisplayFormat, (color>>8)&0xF8, ((color>>5)&0x3F)<<2, (color&0x1f)<<3, 0xff);

#endif
}
void FillRectToLCDBuff(LCDBUFFSRC src,unsigned short sx,unsigned short sy,unsigned short ex,unsigned short ey,unsigned short color)
{
	unsigned int i,j;

	if(sx>(_LCD_WIDTH) ||ex>(_LCD_WIDTH))return;
	if(sy>(_LCD_HEIGHT) ||ey>(_LCD_HEIGHT))return;
		for(i=sy;i<ey;i++)
			for(j=sx;j<ex;j++)
				src[i][j]=color;

}

void Vpost_DrawHLine(unsigned short sx,unsigned short ex,unsigned short y,unsigned short color,unsigned short buff)
{
	unsigned int j;

	if(sx>(_LCD_WIDTH-1) ||ex>(_LCD_WIDTH-1))return;
	if(y>(_LCD_HEIGHT-1) ||y>(_LCD_HEIGHT-1))return;
	if(buff==BUFF0){
		for(j=sx;j<ex;j++)
			LCDOSDBuffer1[y][j]=color;
		}
	else{
		for(j=sx;j<ex;j++)
			LCDOSDBuffer2[y][j]=color;
		}
}

void Vpost_DrawVLine(unsigned short x,unsigned short sy,unsigned short ey,unsigned short color,unsigned short buff)
{
	unsigned int j;

	if(x>(_LCD_WIDTH-1) ||x>(_LCD_WIDTH-1))return;
	if(sy>(_LCD_HEIGHT-1) ||sy>(_LCD_HEIGHT-1))return;
	if(buff==BUFF0){
		for(j=sy;j<ey;j++)
			LCDOSDBuffer1[j][x]=color;
		}
	else{
		for(j=sy;j<ey;j++)
			LCDOSDBuffer2[j][x]=color;
		}
}
void Vpost_PictureFrame(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,unsigned short color,unsigned short buff)
{
	Vpost_DrawHLine(x1, x2, y1, color, buff);
	Vpost_DrawHLine(x1, x2, y2, color, buff);
	Vpost_DrawVLine(x1, y1, y2, color, buff);
	Vpost_DrawVLine(x2, y1, y2, color, buff);
}
void LCD_DrawLine(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,unsigned short dcolor)
{
	unsigned short t; 
	short xerr=0,yerr=0,delta_x,delta_y,distance; 
	short incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ ){//画线输出 
		LCDOSDBuffer1[uCol][uRow]=dcolor;
	//	LCD_DrawPoint(uRow,uCol,dcolor);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) { 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) { 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   
void UpdateDrawRect(unsigned short x,unsigned short y,unsigned short width,unsigned short height,unsigned short color)
{
	unsigned short i,j,k;
	unsigned short tmp[6]={4,3,2,2,1,1};
	unsigned short tmp2[6]={1,1,2,2,3,4};

	for(i=y;i<y+6;i++){
		k=x+width-tmp[i-y];
		for(j=x+tmp[i-y];j<k;j++)
			_VpostFrameBuffer[i*_LCD_WIDTH+j] = color;
			//LCDOSDBuffer1[i][j]=color;
		}
	for(i=y+6;i<y+height-6;i++){
		for(j=x;j<x+width;j++)
			_VpostFrameBuffer[i*_LCD_WIDTH+j] = color;
			//LCDOSDBuffer1[i][j]=color;
		}
	for(i=y+height-6;i<y+height;i++){
		k=x+width-tmp2[i-y-height+6];
		for(j=x+tmp2[i-y-height+6];j<k;j++)
			_VpostFrameBuffer[i*_LCD_WIDTH+j] = color;
			//LCDOSDBuffer1[i][j]=color;	
		}
}
/*****************************************************************************
在屏幕上用指定颜色画一个圆，原点(x0,y0)，半径r。
*****************************************************************************/
void Draw_Circle(unsigned short x0,unsigned short y0,unsigned short r,unsigned short dcolor)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b){          
		LCDOSDBuffer1[y0-a][x0-b]=dcolor;      //3            
		LCDOSDBuffer1[y0-a][x0+b]=dcolor;             
		LCDOSDBuffer1[y0+b][x0-a]=dcolor;        
		LCDOSDBuffer1[y0-a][x0-b]=dcolor;                  
		LCDOSDBuffer1[y0-b][x0-a]=dcolor;  
		LCDOSDBuffer1[y0+a][x0+b]=dcolor;           //4 
		LCDOSDBuffer1[y0-b][x0+a]=dcolor;  
		LCDOSDBuffer1[y0+b][x0+a]=dcolor;   
		LCDOSDBuffer1[y0+a][x0-b]=dcolor;
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else{
			di+=10+4*(a-b);   
			b--;
		} 
	//	LCD_DrawPoint(x0+a,y0+b,dcolor);
		LCDOSDBuffer1[y0+b][x0+a]=dcolor;  
	}
} 

/*****************************************************************************
在屏幕上用指定颜色填充一个圆，原点(x0,y0)，半径r。
*****************************************************************************/
#define sABS(x)  ((x) > (0) ? (x) : (-x))
void Fill_Circle(unsigned short x0,unsigned short y0, unsigned short r,unsigned short dcolor)
{
	int a0,a1,b0,b1,c0,c1,d;
	int i,j;
	
	d = r*r;
	a0 = x0-r;
	if(x0<r)a0=0;
	b0 = y0 - r;
	if(y0<r)b0=0;
	a1 = x0+r;
	if(a1>_LCD_WIDTH)a1=_LCD_WIDTH-1;
	b1 = y0+r;
	if(b1>_LCD_HEIGHT)b1=_LCD_HEIGHT-1;
	for(i=b0;i<b1;i++){
		for(j=a0;j<a1;j++){
		//	c0 = sABS(j-x0);
			if(j>x0)c0=j-x0;
			else c0=x0-j;
			c0 = c0*c0;
		//	c1 = sABS(i-y0);
			if(i>y0)c1=i-y0;
			else c1=y0-i;
			c1 = c1*c1;
			c1 += c0;
			if(c1<=d)LCDOSDBuffer1[i][j]=dcolor;//LCD_DrawPoint(j,i,dcolor);
			}
		}
}

void Fill_Sector(unsigned short x0,unsigned short y0,unsigned short rs,unsigned short rl,unsigned short ang_s,unsigned short ang_e)
{

}
static void Draw8Point(unsigned int x0,unsigned int y0, unsigned int xoff, unsigned int yoff,unsigned short color) 
{
	LCDOSDBuffer1[y0+yoff][x0+xoff]=color;
	LCDOSDBuffer1[y0+yoff][x0-xoff]=color;
	LCDOSDBuffer1[y0+xoff][x0+yoff]=color;
	LCDOSDBuffer1[y0-xoff][x0+yoff]=color;
	if (yoff) {
		LCDOSDBuffer1[y0-yoff][x0+xoff]=color;
		LCDOSDBuffer1[y0-yoff][x0-xoff]=color;
		LCDOSDBuffer1[y0+xoff][x0-yoff]=color;
		LCDOSDBuffer1[y0-xoff][x0-yoff]=color;
	}
}

void Vpost_DrawCircle(unsigned int x0, unsigned int y0, unsigned int r,unsigned short color) {
  int i;
  int imax = ((int)((int)r*707))/1000+1;
  int sqmax = (int)r*(int)r+(int)r/2;
  int y=r;
  Draw8Point(x0,y0,r,0,color);
  for (i=1; i<= imax; i++) {
    if ((i*i+y*y) >sqmax) {
      Draw8Point(x0,y0,i,y,color);
      y--;
    }
    Draw8Point(x0,y0,i,y,color);
  }
}

//========================================================================
#ifndef	LOADFROMFLASH
void LoadRes(void)
{
	CHAR	suFileName[40];
	int		FtFile,cnt;
	unsigned char  *pDst=(unsigned char *)((unsigned int)RES_ADDR| NON_CACHE_BIT);
	
	sysprintf("load resource\n");
//	spiFlashPDMARead(FLASH_RES_ADDR,TOTAL_LEN,(unsigned int*)pDst);		//长度根据resource.bin的长度定
	fsAsciiToUnicode("C:\\resource.bin", suFileName, TRUE);
	FtFile=fsOpenFile(suFileName, NULL, O_RDONLY|O_FSEEK);
	fsFileSeek(FtFile, 0, SEEK_SET);
	fsReadFile(FtFile, pDst, TOTAL_LEN, &cnt);
	fsCloseFile(FtFile);
}
#endif

void openRes(void)
{
	CHAR	suFileName[40];
	
	fsAsciiToUnicode("C:\\resource.bin", suFileName, TRUE);
	rsFil=fsOpenFile(suFileName, NULL, O_RDONLY|O_FSEEK);
	if (rsFil ==0){
		sysprintf("fail open C:\\resource.bin\n");
	}
}

void TestSDFun()
{
	char	suFileName[20];
	unsigned char    buf[100];
	int realLen = 0 ;
	int fil;
	int i;
	
	fsAsciiToUnicode("X:\\1.txt", suFileName, TRUE);
	fil=fsOpenFile(suFileName, NULL, O_RDONLY);
	if (fil ==0){
		sysprintf("fail open X:\\1.TXT\n");
	}
	fsReadFile(fil, buf, sizeof(buf)-1, &realLen);
	fsCloseFile(fil);
	buf[realLen] = '\0';
	for(i = 0; i < realLen; i++)
		sysprintf("%x = %c \n", buf[i], buf[i]);
	sysprintf("read file len:%d char:%s\n", realLen,buf);
}
#ifdef	LOADFROMFLASH	
__align(4096) unsigned short iconbuff[_LCD_WIDTH*_LCD_HEIGHT*2];
#endif
void showIcon(unsigned int index,unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned short alpha)
{
	unsigned int i,TotalByte;//,startAddr;
	unsigned short alphadata;//tt,r1,g1,b1,r2,g2,b2,;
	unsigned short px,py,aa;
	unsigned short  *pDst;
	unsigned int c1,c2,c3,ac;
	unsigned short ch,cl;
	int		cnt;//FtFile,;
//	CHAR	suFileName[40];
	
	if(index==0xFFFFFFFF)return;
	TotalByte = w*h;
	if(TotalByte==0)return;
	if(TotalByte> _LCD_WIDTH*_LCD_HEIGHT)return;
	if(alpha)TotalByte *=2;
#ifdef	LOADFROMFLASH	
	pDst = (unsigned short *)((unsigned int)iconbuff | NON_CACHE_BIT);		
	fsFileSeek(rsFil, index, SEEK_SET);
	fsReadFile(rsFil, (unsigned char *)pDst, TotalByte*2, &cnt);
#else
	pDst = (unsigned short *)(RES_ADDR+index);
#endif
	px = x;
	py = y;
	if(alpha==0){
		for(i=0;i<TotalByte;i++){
		#ifdef	COLOR_INVERT
			LCDOSDBuffer1[py][px]=~pDst[i];
		#else
			LCDOSDBuffer1[py][px]=pDst[i];
		#endif
			px++;
			if(px==(x+w)){
				px=x;
				py++;
				}
			}
		}
	else{
		for(i=0;i<TotalByte;i+=2){
			alphadata = pDst[i]&0xff;
			if(alphadata==0xff)
		#ifdef	COLOR_INVERT		
				LCDOSDBuffer1[py][px]=~pDst[i+1];
		#else
				LCDOSDBuffer1[py][px]=pDst[i+1];
		#endif
			else if(alphadata){
		#ifdef	COLOR_INVERT
				aa = ~pDst[i+1];
		#else
				aa = pDst[i+1];
		#endif
				c1=((aa&0x07e0)<<16)|(aa&0xf81F);
				c2=((LCDOSDBuffer1[py][px]&0x07e0)<<16)|(LCDOSDBuffer1[py][px]&0xf81F);
				ac = alphadata>>3;
				c3 = (c1-c2)*ac/32 + c2;
				ch = (c3>>16)&0x07e0;
				cl = c3&0xf81F;	
				LCDOSDBuffer1[py][px]=ch|cl;
				}
			px++;
			if(px>=x+w){
				px=x;
				py++;
				}
			}
		}
	return;//BMP显示结束.    					   
}

void showIconOnBuff(LCDBUFFSRC src, unsigned int index,unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned short alpha)
{
	unsigned int i,TotalByte;//,startAddr;
	unsigned short alphadata;//,r1,g1,b1,r2,g2,b2,tt;
	unsigned short px,py,aa;
	unsigned short  *pDst;
	unsigned int c1,c2,c3,ac;
	unsigned short ch,cl;
	int		cnt;//FtFile,;
//	CHAR	suFileName[40];
	
	if(index==0xFFFFFFFF)return;
	TotalByte = w*h;
	if(TotalByte==0)return;
	if(TotalByte> _LCD_WIDTH*_LCD_HEIGHT)return;
	if(alpha)TotalByte *=2;
#ifdef	LOADFROMFLASH	
	pDst = (unsigned short *)((unsigned int)iconbuff | NON_CACHE_BIT);		
	fsFileSeek(rsFil, index, SEEK_SET);
	fsReadFile(rsFil, (unsigned char *)pDst, TotalByte*2, &cnt);
#else
	pDst = (unsigned short *)(RES_ADDR+index);
#endif
	px = x;
	py = y;
	if(alpha==0){
		for(i=0;i<TotalByte;i++){
		#ifdef	COLOR_INVERT
			src[py][px]=~pDst[i];
		#else
			src[py][px]=pDst[i];
		#endif
			px++;
			if(px==(x+w)){
				px=x;
				py++;
				}
			}
		}
	else{
		for(i=0;i<TotalByte;i+=2){
			alphadata = pDst[i]&0xff;
			if(alphadata==0xff)
		#ifdef	COLOR_INVERT		
				src[py][px]=~pDst[i+1];
		#else
				src[py][px]=pDst[i+1];
		#endif
			else if(alphadata){
		#ifdef	COLOR_INVERT
				aa = ~pDst[i+1];
		#else
				aa = pDst[i+1];
		#endif
				c1=((aa&0x07e0)<<16)|(aa&0xf81F);
				c2=((src[py][px]&0x07e0)<<16)|(src[py][px]&0xf81F);
				ac = alphadata>>3;
				c3 = (c1-c2)*ac/32 + c2;
				ch = (c3>>16)&0x07e0;
				cl = c3&0xf81F;	
				src[py][px]=ch|cl;
				}
			px++;
			if(px>=x+w){
				px=x;
				py++;
				}
			}
		}
	return;//BMP显示结束.    					   
}

void BKAlphaSet(unsigned short alpha)
{
	unsigned short i,j;
	unsigned short r,g,b;

	if(alpha>255)return;
	for(i=0;i<_LCD_HEIGHT;i++){
		for(j=0;j<_LCD_WIDTH;j++){
			r = LCDOSDBuffer1[i][j]>>11;
			g = (LCDOSDBuffer1[i][j]>>5)&0x3f;
			b = LCDOSDBuffer1[i][j]&0x1f;
			r = (r*alpha)>>8;
			g = (g*alpha)>>8;
			b = (b*alpha)>>8;
			LCDOSDBuffer1[i][j] = (((r<<11)|(g<<5))&0xffe0)|b;
			}
		}
}


void BKAlphaSet20(void)
{
	unsigned short i,j;
	unsigned short r,g,b;
	
	for(i=0;i<_LCD_HEIGHT;i++){
		for(j=0;j<_LCD_WIDTH;j++){
			if(LCDOSDBuffer1[i][j]==0)continue;
			r = LCDOSDBuffer1[i][j]>>13;
			g = (LCDOSDBuffer1[i][j]>>7)&0x0f;
			b = (LCDOSDBuffer1[i][j]>>2)&0x07;
			LCDOSDBuffer1[i][j] = (((r<<11)|(g<<5))&0xffe0)|b;
			}
		}
}

void BKAlphaSet50(void)
{
	unsigned short i,j;
	unsigned short r,g,b;
	
	for(i=0;i<_LCD_HEIGHT;i++){
		for(j=0;j<_LCD_WIDTH;j++){
			if(LCDOSDBuffer1[i][j]==0)continue;
			r = LCDOSDBuffer1[i][j]>>12;
			g = (LCDOSDBuffer1[i][j]>>6)&0x1f;
			b = (LCDOSDBuffer1[i][j]>>1)&0x0f;
			LCDOSDBuffer1[i][j] = (((r<<11)|(g<<5))&0xffe0)|b;
			}
		}
}

void DrawBigBackGround(unsigned short x,unsigned short y)
{
	unsigned short i,j;
	unsigned short tmp[196];
	
	tmp[0]=tmp[195]=11;
	tmp[1]=tmp[194]=9;
	tmp[2]=tmp[193]=8;
	tmp[3]=tmp[192]=7;
	tmp[4]=tmp[191]=6;
	tmp[5]=tmp[190]=5;
	tmp[6]=tmp[189]=4;
	tmp[7]=tmp[188]=3;
	tmp[8]=tmp[187]=2;
	tmp[9]=tmp[186]=2;
	tmp[10]=tmp[185]=1;
	tmp[11]=tmp[184]=1;
	tmp[12]=tmp[183]=1;
	for(i=13;i<183;i++)tmp[i]=0;
	for(i=y;i<y+196;i++){
		for(j=x+tmp[i-y];j<x+278-tmp[i-y];j++)
			LCDOSDBuffer1[i][j]=LCD_GUI_LIGHTGREEN;
		}
}

void DrawMidBackGround(unsigned short x,unsigned short y)
{
	unsigned short i,j;
	unsigned short tmp[146];
	
	tmp[0]=tmp[145]=8;
	tmp[1]=tmp[144]=7;
	tmp[2]=tmp[143]=6;
	tmp[3]=tmp[142]=5;
	tmp[4]=tmp[141]=4;
	tmp[5]=tmp[140]=3;
	tmp[6]=tmp[139]=2;
	tmp[7]=tmp[138]=2;
	tmp[8]=tmp[137]=1;
	tmp[9]=tmp[136]=1;
	for(i=10;i<136;i++)tmp[i]=0;
	for(i=y;i<y+146;i++){
		for(j=x+tmp[i-y];j<x+252-tmp[i-y];j++)
			LCDOSDBuffer1[i][j]=LCD_GUI_LIGHTGREEN;
		}
}

void DrawSmallBackGround(unsigned short x,unsigned short y)
{
	unsigned short i,j;
	unsigned short tmp[144];
	
	tmp[0]=tmp[143]=8;
	tmp[1]=tmp[142]=7;
	tmp[2]=tmp[141]=6;
	tmp[3]=tmp[140]=5;
	tmp[4]=tmp[139]=4;
	tmp[5]=tmp[138]=3;
	tmp[6]=tmp[137]=2;
	tmp[7]=tmp[136]=2;
	tmp[8]=tmp[135]=1;
	tmp[9]=tmp[134]=1;
	for(i=10;i<134;i++)tmp[i]=0;
	for(i=y;i<y+144;i++){
		for(j=x+tmp[i-y];j<x+214-tmp[i-y];j++)
			LCDOSDBuffer1[i][j]=LCD_GUI_LIGHTGREEN;
		}
}

void DrawArcRect(unsigned short x,unsigned short y,unsigned short width,unsigned short height,unsigned short color)
{
	unsigned short i,j,k;
	unsigned short tmp[6]={4,3,2,2,1,1};
	unsigned short tmp2[6]={1,1,2,2,3,4};

	for(i=y;i<y+6;i++){
		k=x+width-tmp[i-y];
		for(j=x+tmp[i-y];j<k;j++)
			LCDOSDBuffer1[i][j]=color;
		}
	for(i=y+6;i<y+height-6;i++){
		for(j=x;j<x+width;j++)
			LCDOSDBuffer1[i][j]=color;
		}
	for(i=y+height-6;i<y+height;i++){
		k=x+width-tmp2[i-y-height+6];
		for(j=x+tmp2[i-y-height+6];j<k;j++)
			LCDOSDBuffer1[i][j]=color;	
		}
}

void LCD_ShowFixChar(unsigned short x,unsigned short y,char c)
{
	unsigned char *ptr=(unsigned char *)fix_ascii; 
	unsigned short i, j;
	
	ptr +=(c-32)*FIX_FONT_H*FIX_FONT_W/8;
	
	for(j = 0; j < FIX_FONT_H*FIX_FONT_W/8; j++){
		for(i = 0; i < FIX_FONT_W; i++){
			if ((*ptr &(1 << (FIX_FONT_W - 1 - i)))){
				LCDOSDBuffer1[y+j][x+i] = POint_COLOR;
			}
		}
		ptr++;
	}
}
void LCD_ShowFixString(unsigned short x,unsigned short y,const char* str)
{      
	unsigned short xx,yy;
	
	xx = x;
	yy = y;
	while(*str){
		if(xx+FIX_FONT_W>_LCD_WIDTH-1){
			xx = 0;
			yy += (FIX_FONT_H+Font_Vspace);
			}
		if(yy+FIX_FONT_H>_LCD_HEIGHT-1)return;
		LCD_ShowFixChar(xx, yy, *str);
		xx+=(FIX_FONT_W+Font_Hspace);
		str++;
	}
}

//画LCD边框,用于测试使用
void Draw_LCDFrame(void)
{
	int i,j;
	
	for(j=0;j<_LCD_HEIGHT;j++)
	{
		LCDOSDBuffer1[j++][i] = LCD_GUI_WHITE;
		for(i=0;i<_LCD_WIDTH-1;i++)
			LCDOSDBuffer1[j][i] = LCD_GUI_BLACK;
		
		LCDOSDBuffer1[j][i++] = LCD_GUI_WHITE;
	}
}
void lcd_gay(void)
{
	UINT32 i,j;
	UINT32 gapv;//, gaph;
	UINT8 k;//,color;
//	UINT16 colorBar_with ;
	
//	colorBar_with = _LCD_WIDTH/3;
	
	#if (LCD_BITS_MODE == 18)
		gapv = _LCD_HEIGHT/64;
		for(i=0;i<_LCD_WIDTH;i++)////display  blue color
	    {
			k = i/gapv+1;
			for(j=0;j<(_LCD_HEIGHT/3);j++)
				LCDOSDBuffer1[j][i] = k;
		}
		gapv = _LCD_HEIGHT/64;
		for(i=0;i<_LCD_WIDTH;i++)
	    {
			k = i/gapv+1;
			for(j= (_LCD_HEIGHT/3);j < (_LCD_HEIGHT*2/3);j++)
				LCDOSDBuffer1[j][i] = k << 8;
		}
		gapv = _LCD_HEIGHT/64;
		for(i=0;i<_LCD_WIDTH;i++)	//display red color
	    {
			k = i/gapv+1;
			for(j=_LCD_HEIGHT*2/3;j<_LCD_HEIGHT;j++)
				LCDOSDBuffer1[j][i] = k << 16;
		}
	#elif (LCD_BITS_MODE == 16)
		gapv = _LCD_HEIGHT/64;
		for(i=0;i<_LCD_WIDTH;i++)
	    {
			k = i/gapv+1;
			for(j=0;j<(_LCD_HEIGHT/3);j++)
				LCDOSDBuffer1[j][i] = k;
		}
		gapv = _LCD_HEIGHT/64;
		for(i=0;i<_LCD_WIDTH;i++)
	    {
			k = i/gapv+1;
			for(j= (_LCD_HEIGHT/3);j < (_LCD_HEIGHT*2/3);j++)
				LCDOSDBuffer1[j][i] = k << 5;
		}
		gapv = _LCD_HEIGHT/64;
		for(i=0;i<_LCD_WIDTH;i++)
	    {
			k = i/gapv+1;
			for(j=_LCD_HEIGHT*2/3;j<_LCD_HEIGHT;j++)
				LCDOSDBuffer1[j][i] = k << 11;
		}
	#endif

//	LCDQuickUpdateScreen(BUFF0);	
}

