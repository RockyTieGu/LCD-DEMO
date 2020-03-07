//#include "W55fa93_spi.h"
#include "w55fa93_vpost.h"
#include "w55fa93_sic.h"
#include "W55FA93_gnand.h"
#include "w55fa93_edma.h"
#include "w55fa93_osd.h"
#include "w55fa93_spi.h"
#include "DrvEDMA.h"
#include "Nvtfat.h"
#include "jpegcodec.h"
#include "jpegsample.h"
#include "avifile.h"
#include "ctp.h"
#include "usbd.h"
#include "mass_storage_class.h"
//#include "blt.h"
#include <math.h>
#include "conn.h"
//#include "W55FA93_adc.h"
#include "rtc.h"
#include "mass.h"
#include "spu.h"
#include "hmi.h"
//#include "GUI.h"
//#include "GUIDEMO.h"
#include "RTP.h"
#include "Midd_uart.h"
#include "Board_bsp.h"
#include "LCD_Power.h"
#include "key.h"

__align(32) DX_LCD_COLOR LCDOSDBuffer1[_LCD_HEIGHT][_LCD_WIDTH];
__align(32) DX_LCD_COLOR LCDOSDBuffer2[_LCD_HEIGHT][_LCD_WIDTH];
__align(32) DX_LCD_COLOR OSD_FrameRGB565[_LCD_HEIGHT*_LCD_WIDTH];
extern __align(256) unsigned char  _VpostFrameBufferPool[VPOST_FRAME_BUFSZ];
extern volatile DX_LCD_COLOR   *_VpostFrameBuffer;
void systemInit(void)
{	
	WB_UART_T uart;
	RTC_TIME_DATA_T sInitTime;
	LCDFORMATEX lcdformatex;
	OSDFORMATEX osdFormat;
	RTC_TIME_DATA_T sCurTime;
	
	sysEnableCache(CACHE_WRITE_BACK);
	UART_Init(LOW_UART_232);
	//UART_Init(HIGHT_UART_485);
	BackLightOFF();
	sysprintf("\r\nCENTRY LCD_DEMO HW Version: %s\r\n",CENTRY_LCD_HW_VERSION);
	sysprintf("CENTRY LCD_DEMO SW Version: %s\r\n",CENTRY_LCD_SW_VERSION);
	sysprintf("CENTRY CURRENT LCD TYPE: %s\r\n",CENTRY_LCD_TYPE);
#if 0
	_VpostFrameBuffer = (DX_LCD_COLOR *)((unsigned int)_VpostFrameBufferPool | 0x80000000);
	lcdformatex.ucVASrcFormat = DRVVPOST_FRAME_RGB565;
	vpostLCMInit(&lcdformatex, (unsigned int *)_VpostFrameBuffer);
	/*     UIÏÔÊ¾¸ÄÎªOSD²ã*/	
#endif
	RTC_Init();					
	EDMA_Init();
	fsInitFileSystem();
	spiFlashInit();
	NVT_MountMem();
	jpegOpen();
//	FreeTypeFontInit();
	if(RTC_Read(RTC_CURRENT_TIME, &sCurTime) == E_RTC_SUCCESS )
	{
		sysprintf("%4d-%2d-%2d %2d:%2d:%2d\n", \
		sCurTime.u32Year,sCurTime.u32cMonth,sCurTime.u32cDay,\
		sCurTime.u32cHour,sCurTime.u32cMinute,sCurTime.u32cSecond);
	}
	
	sysprintf("systemInit exe !\n");
}

void sys_Init(void)
{
	/* CACHE_ON	*/
	sysEnableCache(CACHE_WRITE_BACK);
	sysSetSystemClock(eSYS_UPLL, 	//E_SYS_SRC_CLK eSrcClk,	
						240000,		//unsigned int u32PllKHz, 	
						240000,		//unsigned int u32SysKHz,
						120000,		//unsigned int u32CpuKHz,
						120000,		//unsigned int u32HclkKHz,
						 60000);		//unsigned int u32ApbKHz	
	systemInit();
	sysprintf("sys_Init !\n");
}

void OtherPeripheral_Init(void)
{
	sysSetTimerReferenceClock (TIMER0, 60000000);
	sysStartTimer(TIMER0, 50000, PERIODIC_MODE);	//100us
	
	PWM_Open();
	spuOpen(eDRVSPU_FREQ_8000);
	//RTP_Init();
	MP3Control_Init();
	//ADCKEY_Init();
	//CTP_Init();
	boardKey_Init();
	sysprintf("OtherPeripheral_Init finish !\n");
}

