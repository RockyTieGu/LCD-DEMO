#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wb_include.h"


#include "W55fa93_spi.h"
#include "w55fa93_vpost.h"
#include "w55fa93_sic.h"
#include "W55FA93_gnand.h"
#include "w55fa93_edma.h"
#include "w55fa93_osd.h"
#include "DrvEDMA.h"
#include "Nvtfat.h"
//#include "font.h"
//#include "hmi.h"
#include "jpegcodec.h"
#include "jpegsample.h"
#include "avifile.h"
#include "ctp.h"
//#include "rtc.h"
#include "usbd.h"
#include "mass_storage_class.h"
#include "blt.h"
//#include "ft2build.h"
//#include "freetype\freetype.h"
//#include "freetype\ftglyph.h"
#include <math.h>
#include "conn.h"
#include "W55FA93_adc.h"
#include "rtc.h"
#include "mass.h"
#include "spu.h"
#include "hmi.h"

#include "GUI.h"
#include "GUIDEMO.h"
#include "RTP.h"
#include "ucos_ii.h"
#include "Midd_uart.h"

//#define UDC_EN
__align(32) unsigned short LCDOSDBuffer1[_LCD_HEIGHT][_LCD_WIDTH];
__align(32) unsigned short LCDOSDBuffer2[_LCD_HEIGHT][_LCD_WIDTH];
__align(32) unsigned short LCDOSDBuffer3[_LCD_HEIGHT][_LCD_WIDTH];
__align(32) unsigned short LCDOSDBuffer4[_LCD_HEIGHT][_LCD_WIDTH];
__align(32) unsigned short LCDOSDBuffer5[_LCD_HEIGHT][_LCD_WIDTH];
__align(32) unsigned short OSD_FrameRGB565[_LCD_HEIGHT*_LCD_WIDTH*2];







unsigned int iserr;
unsigned char openFontFlag = 0;
FILE_STAT_T src_stat, dest_stat;
unsigned char test_E0 = 0, test_E1 = 0;

extern  __align(256) unsigned short  _VpostFrameBufferPool[VPOST_FRAME_BUFSZ];
extern unsigned short   *_VpostFrameBuffer;
void systemInit(void)
{

	RTC_TIME_DATA_T sInitTime;
	LCDFORMATEX lcdformatex;
	OSDFORMATEX osdFormat;
	RTC_TIME_DATA_T sCurTime;
	UART_Init(LOW_UART_232);
	UART_Init(HIGHT_UART_485);
	
#if 0
	_VpostFrameBuffer = (unsigned short *)((unsigned int)_VpostFrameBufferPool | 0x80000000);
	lcdformatex.ucVASrcFormat = DRVVPOST_FRAME_RGB565;
	vpostLCMInit(&lcdformatex, (unsigned int *)_VpostFrameBuffer);
	/*     UI显示改为OSD层*/	
#endif
//#if defined(RS485OPEN)
//	
//#endif

//	ClearFrameBuffer();
	RTC_Init();
//	sInitTime.u32Year = 2018;
//	sInitTime.u32cMonth = 12;	
//	sInitTime.u32cDay = 30;
//	sInitTime.u32cHour = 17;
//	sInitTime.u32cMinute = 27;
//	sInitTime.u32cSecond = 0;
//	sInitTime.u32cDayOfWeek = RTC_SUNDAY;
//	sInitTime.u8cClockDisplay = RTC_CLOCK_24;	
//	if(RTC_Open(&sInitTime) !=E_RTC_SUCCESS)
//		sysprintf("Open Fail!!\n");
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
	TestSDFun();
}
extern unsigned char ctp_flag;
void WatchDog_ISR()
{
  	sysClearWatchDogTimerInterruptStatus();
	sysClearWatchDogTimerCount();
}
//extern Pen_Holder Pen_Point;
extern SampleMenu_Dealkey hmi; 
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

}

#define APP_TASK_0_STK_SIZE        1024*100
#define APP_TASK_0                  5
OS_STK stk_Task_0[APP_TASK_0_STK_SIZE] = {0};
static void Task0(void *p_arg)
{
	static int i = 0;
	sysSetTimerReferenceClock (TIMER0, 60000000);
	sysStartTimer(TIMER0, 50000, PERIODIC_MODE);	//100us
	spuOpen(eDRVSPU_FREQ_8000);
	RS485SendData("123456abcedf\n", strlen("123456abcedf\n") + 1);
	sysprintf("hello,world!");
	//RTP_Init();
	MP3Control_Init();
	//ADCKEY_Init();
	//CTP_Init();
	boardKey_Init();
	PWM_Open();
	BackLightAdj(50);
	GUI_Init();
//	WM_SetCreateFlags(WM_CF_MEMDEV);//示波器窗口避免闪烁
	//WM_EnableMemdev(WM_HBKWIN);//圆形菜单地图等窗口避免闪烁
//	WM_MULTIBUF_Enable(1);

	sysprintf("Task0 start!\r\n");
	CreatAllTask();
	sysprintf("Task0 endif!\r\n");
	//创造任务-END	
	//
	//OSTaskSuspend(APP_TASK_0);	//挂起起始任务.
	while(1)
	{  
	//	GUI_TOUCH_Exec();//调用触屏更新,在VOID DrvADC_PollingWT(VOID)函数有OSTimeDly更新
		//TOUCH_Scan();	
	//	sysprintf("Task011\n");		
		OSTimeDly(5000);//1ms更新一次
	//Test_LCDMENU(i++%9);
	}
}
int main()
{
	__use_two_region_memory();	//定义堆栈为双区，以免堆栈错误
	sys_Init();
	sysprintf("N32905 ucos2 base built @ %s %s\n", __DATE__, __TIME__);
	BSP_Init();
	OSInit();
	OSTaskCreate(Task0, (void *)0, &stk_Task_0[APP_TASK_0_STK_SIZE-1], APP_TASK_0); 
	OSStart();  
//	hmi_Init();
//	while(1)
//	{
////		if(ctp_flag)
////		{
////			ctp_flag = 0;
////			CPT_ReadPacket();
////		}
//		ADCKEY_Handle();
//		hmi(GetKey_Value());
//	}
//		
}




