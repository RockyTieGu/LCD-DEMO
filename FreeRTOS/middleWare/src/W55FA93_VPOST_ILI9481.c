#include "stdio.h"
#include "stdlib.h"
#include "wb_include.h"
#include "w55fa93_vpost_driver.h"

extern void LCDDelay(unsigned int nCount);

//static unsigned int g_nScreenWidth;
//static unsigned int g_nScreenHeight;

typedef enum 
{
	eEXT 	= 0,
	eX32K 	= 1,
	eAPLL  	= 2,
	eUPLL  	= 3
}E_CLK;


static unsigned int GetPLLOutputKhz(
	E_CLK eSysPll,
	unsigned int u32FinKHz
	)
{
	unsigned int u32Freq, u32PllCntlReg;
	unsigned int NF, NR, NO;
	
	unsigned char au8Map[4] = {1, 2, 2, 4};
	if(eSysPll==eSYS_APLL)
		u32PllCntlReg = inp32(REG_APLLCON);
	else if(eSysPll==eSYS_UPLL)	
		u32PllCntlReg = inp32(REG_UPLLCON);		
	
	NF = (u32PllCntlReg&FB_DV)+2;
	NR = ((u32PllCntlReg & IN_DV)>>9)+2;
	NO = au8Map[((u32PllCntlReg&OUT_DV)>>14)];
	sysprintf("PLL regster = 0x%x\n", u32PllCntlReg);	
	sysprintf("NF = %d\n", NF);
	sysprintf("NR = %d\n", NR);
	sysprintf("NOr = %d\n", NO);
		
	u32Freq = u32FinKHz*NF/NR/NO;
	sysprintf("PLL Freq = %d\n", u32Freq);
	return u32Freq;
}

void BacklightControl(int OnOff)
{	
	// GPE[1] set OUTPUT mode  => control the backlight
	outpw(REG_GPIOE_OMD, (inpw(REG_GPIOE_OMD) & 0x0000FFFF)| 0x00000002);
	if(OnOff==TRUE) {
		outpw(REG_GPIOE_DOUT, (inpw(REG_GPIOE_DOUT) & 0x0000FFFF)| 0x00000002);
	} else {
		outpw(REG_GPIOE_DOUT, (inpw(REG_GPIOE_DOUT) & 0x0000FFFF) & 0xFFFFFFFD);
	}
}

void LCD_Control_En()
{
	outpw(REG_GPBFUN, inpw(REG_GPBFUN) & 0xFFFFCFF3);
	gpio_setportdir(GPIO_PORTB, BIT1|BIT6, BIT1|BIT6);		//GPIOB1,6 output mode
	gpio_setportval(GPIO_PORTB, BIT1, 0);      //GPIOB1,6 high to enable Amplifier 
	gpio_setportval(GPIO_PORTB, BIT6, BIT6);      //GPIOB1,6 high to enable Amplifier 
	
	gpio_setportpull(GPIO_PORTB, BIT1|BIT6, BIT1|BIT6);		//GPIOB1,6 pull high
	
}
int vpostLCMInit_S050WQ01(PLCDFORMATEX plcdformatex, unsigned int *pFramebuf)
{
	S_DRVVPOST_SYNCLCM_WINDOW sWindow = {_LCD_WIDTH,_LCD_HEIGHT,_LCD_WIDTH};	
	S_DRVVPOST_SYNCLCM_HTIMING sHTiming = {LCD_RGB_HPW ,LCD_RGB_HBP ,LCD_RGB_HFP };
	S_DRVVPOST_SYNCLCM_VTIMING sVTiming = {LCD_RGB_VPW ,LCD_RGB_VBW ,LCD_RGB_VFW };
	S_DRVVPOST_SYNCLCM_POLARITY sPolarity = {LCD_RGB_VSYNC_POL,LCD_RGB_HSYNC_POL,LCD_RGB_DE_POL,LCD_RGB_CLOCK_POL};
	UINT32 nBytesPixel, u32PLLclk, u32ClockDivider;
//	UINT32 TT;

	LCD_Control_En();
	
	// VPOST clock control
	outpw(REG_AHBCLK, inpw(REG_AHBCLK) | VPOST_CKE | HCLK4_CKE);
	outpw(REG_AHBIPRST, inpw(REG_AHBIPRST) | VPOSTRST);
	LCDDelay(10);
	outpw(REG_AHBIPRST, inpw(REG_AHBIPRST) & ~VPOSTRST);	
		
	u32PLLclk = GetPLLOutputKhz(eUPLL, 12000);			// CLK_IN = 12 MHz
	u32ClockDivider = u32PLLclk / 1000;
	u32ClockDivider /= 2;
	u32ClockDivider =  u32ClockDivider/LCD_CLOCK_FREQUENCY;
	sysprintf("u32ClockDivider:%d\n", u32ClockDivider);
	outpw(REG_CLKDIV1, (inpw(REG_CLKDIV1) & ~VPOST_N0) | 1);						
	outpw(REG_CLKDIV1, (inpw(REG_CLKDIV1) & ~VPOST_N1) | (((u32ClockDivider-1) & 0xFF) << 8));						
	outpw(REG_CLKDIV1, inpw(REG_CLKDIV1) & ~VPOST_S);
	outpw(REG_CLKDIV1, inpw(REG_CLKDIV1) | (3<<3));		// VPOST clock from UPLL	

//	TT = inpw(REG_CLKDIV4);
//	sysprintf("read clkdiv:0x%x ==%d\n", TT, TT);
	vpostVAStopTrigger();	

	// Enable VPOST function pins
#ifdef	OPT_24BIT_MODE		
	vpostSetDataBusPin(eDRVVPOST_DATA_24BITS);
#else
//	vpostSetDataBusPin(eDRVVPOST_DATA_8BITS);	
	vpostSetDataBusPin(eDRVVPOST_DATA_16BITS);	
#endif	  
	// LCD image source select
	vpostSetLCM_ImageSource(eDRVVPOST_FRAME_BUFFER);
	
	// configure LCD interface
	vpostSetLCM_TypeSelect(eDRVVPOST_HIGH_RESOLUTINO_SYNC);

	// configure LCD timing sync or async with TV timing	
	vpostsetLCM_TimingType(eDRVVPOST_ASYNC_TV);
	
#ifdef	OPT_24BIT_MODE		
    vpostSetParalelSyncLCM_Interface(eDRVVPOST_PRGB_24BITS);
#else    
//    vpostSetParalelSyncLCM_Interface(eDRVVPOST_PRGB_16BITS);
    vpostSetParalelSyncLCM_Interface(eDRVVPOST_PRGB_16BITS);    
#endif    
	// MPU LCM initial 	
	
    // set MPU timing 
    vpostSetSyncLCM_HTiming(&sHTiming);

    vpostSetSyncLCM_VTiming(&sVTiming);
    // set MPU LCM window 
	vpostSetSyncLCM_ImageWindow(&sWindow);
	vpostSetSyncLCM_SignalPolarity(&sPolarity);  	
    
#if 0
	vpostSetFrameBuffer_BaseAddress(pFramebuf);
#else    
    // set frambuffer base address
	if(pFramebuf != NULL) {
		vpostAllocVABufferFromAP(pFramebuf);
	} else {
    	if( vpostAllocVABuffer(plcdformatex, nBytesPixel)==FALSE)
    		return ERR_NULL_BUF;
	}
#endif    
	
	// set frame buffer data format
	vpostSetFrameBuffer_DataType((E_DRVVPOST_FRAME_DATA_TYPE)(plcdformatex->ucVASrcFormat));
	
	vpostSetYUVEndianSelect(eDRVVPOST_YUV_LITTLE_ENDIAN);
	
	// enable MPU LCD controller
	vpostVAStartTrigger();
	return 0;
}


int vpostLCMDeinit_S050WQ01(VOID)
{
	vpostVAStopTrigger();
	vpostFreeVABuffer();
	outpw(REG_AHBCLK, inpw(REG_AHBCLK) & ~VPOST_CKE);	
	return 0;
}

