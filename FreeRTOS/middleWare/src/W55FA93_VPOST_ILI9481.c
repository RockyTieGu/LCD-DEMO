#include "stdio.h"
#include "stdlib.h"
#include "wb_include.h"
#include "w55fa93_vpost_driver.h"
#include "dx_lcdconfig.h"
#include "FreeRTOS.h"
#include "task.h"
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

/*

PA2 SPI_CS
PA3 SPI_CLK
PA4	SPI_SDI
PA5 SPI_SDO
PA6	REST
*/

#define SET_LCD_RST(x)		gpio_setportval(GPIO_PORTA, BIT6,	(x) ? BIT6 : 0)
#define SET_LCD_CS(x)		gpio_setportval(GPIO_PORTA, BIT2,	(x) ? BIT2 : 0)
#define SET_LCD_SCLK(x)		gpio_setportval(GPIO_PORTA, BIT3,	(x) ? BIT3 : 0)
#define SET_LCD_SD0(x)		gpio_setportval(GPIO_PORTA, BIT5,	(x) ? BIT5 : 0)
#define SET_LCD_SDI(x)		gpio_setportval(GPIO_PORTA, BIT4,	(x) ? BIT4 : 0);



static void SPI_SendData(unsigned char l)
{  
   unsigned char n;
   unsigned char i = l;
	
   for(n=0; n<8; n++)			
   {       
	SET_LCD_SCLK(0);
	LCDDelay(500);
	if(i&0x80)
		{SET_LCD_SDI(1);}
	else
		{SET_LCD_SDI(0);}
	LCDDelay(500);
	SET_LCD_SCLK(1);
	i<<=1;
	LCDDelay(500);
   }
}

static void SPI_WriteComm(unsigned char i)
{
	SET_LCD_CS(0);
	LCDDelay(500);
	SET_LCD_SCLK(0);
	LCDDelay(500);
	SET_LCD_SDI(0);
	LCDDelay(500);
	SET_LCD_SCLK(1);
	LCDDelay(500);
	SPI_SendData(i);
	LCDDelay(500);
	SET_LCD_CS(1);
	LCDDelay(500);
}

static void SPI_WriteData(unsigned char i)
{ 
	SET_LCD_CS(0);
	LCDDelay(500);
	SET_LCD_SCLK(0);
	LCDDelay(500);
	SET_LCD_SDI(1);
	LCDDelay(500);
	SET_LCD_SCLK(1);
		LCDDelay(500);
	SPI_SendData(i);
	LCDDelay(500);
	SET_LCD_CS(1);
	LCDDelay(500);
} 

void LCD_CELL_INIT()
{
	SET_LCD_CS(0);
	sysDelay(1000);//100ms
	SET_LCD_RST(1);
	sysDelay(1000);//100ms
	SET_LCD_RST(0);
	sysDelay(500);//50ms
	SET_LCD_RST(1);
	sysDelay(1000);//100ms


	//CMI3.47									
	SPI_WriteComm(0x11); //Sleep Out				
	sysDelay(300);						
	SPI_WriteComm(0xB9); //SET password			
	SPI_WriteData(0xFF);					
	SPI_WriteData(0x83);					
	SPI_WriteData(0x57);					
	sysDelay(500);//50ms					
	SPI_WriteComm(0xB1); //SETPower				
	SPI_WriteData(0x00); //STB		
	SPI_WriteData(0x16); //				
	SPI_WriteData(0x1C); //VSPR = 4.41V			
	SPI_WriteData(0x1C); //VSNR = -4.41V			
	SPI_WriteData(0xC3); //AP 0xc3				
	SPI_WriteData(0x5C); //FS 0x44					
	sysDelay(500);//50ms	

	SPI_WriteComm(0xB3);
	SPI_WriteData(0x43);
	SPI_WriteData(0x00);
	SPI_WriteData(0x06);
	SPI_WriteData(0x06);
	sysDelay(500);//50ms		//VPL[5:0]
						
	SPI_WriteComm(0xB4); //SETCYC				
	SPI_WriteData(0x32); //2-dot				
	SPI_WriteData(0x40); //RTN					
	SPI_WriteData(0x00); //DIV					
	SPI_WriteData(0x2A); //N_DUM				
	SPI_WriteData(0x2A); //I_DUM				
	SPI_WriteData(0x0D); //GDON				
	SPI_WriteData(0x78); //GDOFF				
	sysDelay(500);//50ms				
	SPI_WriteComm(0xB6); //VCOMDC				
	SPI_WriteData(0x3c);				
	sysDelay(500);//50ms	
	SPI_WriteComm(0xB5);
	SPI_WriteData(0x0B);//08
	SPI_WriteData(0x0B);//08
	sysDelay(500);//50ms					
						
	SPI_WriteComm(0xC0); //SETSTBA				
	SPI_WriteData(0x70); //N_OPON				
	SPI_WriteData(0x50); //I_OPON				
	SPI_WriteData(0x01); //STBA				
	SPI_WriteData(0x3C); //STBA				
	SPI_WriteData(0xC8); //STBA				
	SPI_WriteData(0x08); //GENON				
	sysDelay(500);//50ms		
	SPI_WriteComm(0xCC); //Set Panel		
	SPI_WriteData(0x0B); 		
	sysDelay(500);//50ms		
	SPI_WriteComm(0xB6); //VCOMDC				
	SPI_WriteData(0x40);  //0x40

	SPI_WriteComm(0xE0); //Set Gamma		
	SPI_WriteData(0x02);		
	SPI_WriteData(0x0A);		
	SPI_WriteData(0x10);		
	SPI_WriteData(0x1A);		
	SPI_WriteData(0x22);		
	SPI_WriteData(0x34);		
	SPI_WriteData(0x41);		
	SPI_WriteData(0x4A);		
	SPI_WriteData(0x4D);		
	SPI_WriteData(0x44);		
	SPI_WriteData(0x3A);		
	SPI_WriteData(0x23);		
	SPI_WriteData(0x19);		
	SPI_WriteData(0x08);		
	SPI_WriteData(0x09);		
	SPI_WriteData(0x03);		
	SPI_WriteData(0x02);		
	SPI_WriteData(0x0A);		
	SPI_WriteData(0x10);		
	SPI_WriteData(0x1A);		
	SPI_WriteData(0x22);		
	SPI_WriteData(0x34);		
	SPI_WriteData(0x41);		
	SPI_WriteData(0x4A);		
	SPI_WriteData(0x4D);		
	SPI_WriteData(0x44);		
	SPI_WriteData(0x3A);		
	SPI_WriteData(0x23);		
	SPI_WriteData(0x19);		
	SPI_WriteData(0x08);		
	SPI_WriteData(0x09);		
	SPI_WriteData(0x03);		
	SPI_WriteData(0x00);		
	SPI_WriteData(0x01);		
	sysDelay(50);//50ms		
	SPI_WriteComm(0x3A); //COLMOD		
	SPI_WriteData(0x66); //RGB888		
	SPI_WriteComm(0xE9); //SETIMAGE		
	SPI_WriteData(0x00); //DBbus24_EN=1		
	sysDelay(500);//50ms		
	SPI_WriteComm(0x29); //Display On		
	sysDelay(500);//50ms		
	SPI_WriteComm(0x2C); //Write SRAM Data		

}

/*

PA2 SPI_CS
PA3 SPI_CLK
PA4	SPI_SDI
PA5 SPI_SDO
PA6	REST
*/

void SPI_INIT_LCD(void)
{
	outpw(REG_GPAFUN, inpw(REG_GPAFUN) & 0xFFFF000F);
	gpio_setportdir(GPIO_PORTA, BIT2|BIT3|BIT4|BIT6, BIT2|BIT3|BIT4|BIT6);
	gpio_setportdir(GPIO_PORTA, BIT5, BIT5);
	gpio_setportpull(GPIO_PORTA, BIT2|BIT3|BIT4|BIT6, BIT2|BIT3|BIT4|BIT6);
	gpio_setportpull(GPIO_PORTA, BIT5, BIT5);
	
	LCD_CELL_INIT();
}

int vpostLCMInit_CENTRY(PLCDFORMATEX plcdformatex, unsigned int *pFramebuf)
{
	S_DRVVPOST_SYNCLCM_WINDOW sWindow = {_LCD_WIDTH,_LCD_HEIGHT,_LCD_WIDTH};	
	S_DRVVPOST_SYNCLCM_HTIMING sHTiming = {LCD_RGB_HPW ,LCD_RGB_HBP ,LCD_RGB_HFP };
	S_DRVVPOST_SYNCLCM_VTIMING sVTiming = {LCD_RGB_VPW ,LCD_RGB_VBW ,LCD_RGB_VFW };
	S_DRVVPOST_SYNCLCM_POLARITY sPolarity = {LCD_RGB_VSYNC_POL,LCD_RGB_HSYNC_POL,LCD_RGB_DE_POL,LCD_RGB_CLOCK_POL};
	UINT32 nBytesPixel, u32PLLclk, u32ClockDivider;

	LCD_Control_En();
	LCDDelay(500);
	//SPI_INIT_LCD();
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
#if (LCD_BITS_MODE == 18)			
	vpostSetDataBusPin(eDRVVPOST_DATA_18BITS);
#elif (LCD_BITS_MODE == 16)  
	vpostSetDataBusPin(eDRVVPOST_DATA_16BITS);	
#else	
	vpostSetDataBusPin(eDRVVPOST_DATA_16BITS);	
#endif	  
	// LCD image source select
	vpostSetLCM_ImageSource(eDRVVPOST_FRAME_BUFFER);
	
	// configure LCD interface
	vpostSetLCM_TypeSelect(eDRVVPOST_HIGH_RESOLUTINO_SYNC);

	// configure LCD timing sync or async with TV timing	
	vpostsetLCM_TimingType(eDRVVPOST_ASYNC_TV);
	
#if(LCD_BITS_MODE == 18)		
    vpostSetParalelSyncLCM_Interface(eDRVVPOST_PRGB_18BITS);
#elif (LCD_BITS_MODE == 16)   
    vpostSetParalelSyncLCM_Interface(eDRVVPOST_PRGB_16BITS);    
#else
	 vpostSetParalelSyncLCM_Interface(eDRVVPOST_PRGB_16BITS);   
#endif    
	// MPU LCM initial 	
	
    // set MPU timing 
    vpostSetSyncLCM_HTiming(&sHTiming);

    vpostSetSyncLCM_VTiming(&sVTiming);
    // set MPU LCM window 
	vpostSetSyncLCM_ImageWindow(&sWindow);
	vpostSetSyncLCM_SignalPolarity(&sPolarity);  	
    
  	sysprintf("Lcd init step 1.0\n");
    // set frambuffer base address
	if(pFramebuf != NULL) {
		vpostAllocVABufferFromAP(pFramebuf);
	} else {
    	if( vpostAllocVABuffer(plcdformatex, nBytesPixel)==FALSE)
    		return ERR_NULL_BUF;
	} 
		sysprintf("Lcd init step 1.1\n");
	// set frame buffer data format
	vpostSetFrameBuffer_DataType((E_DRVVPOST_FRAME_DATA_TYPE)(plcdformatex->ucVASrcFormat));
	sysprintf("Lcd init step 1.2\n");
	vpostSetYUVEndianSelect(eDRVVPOST_YUV_LITTLE_ENDIAN);
	sysprintf("Lcd init step 1.3\n");
	// enable MPU LCD controller
	vpostVAStartTrigger();
	sysprintf("Lcd init step 1.4\n");
	return 0;
}


int vpostLCMDeinit_CENTRY(VOID)
{
	vpostVAStopTrigger();
	vpostFreeVABuffer();
	outpw(REG_AHBCLK, inpw(REG_AHBCLK) & ~VPOST_CKE);	
	return 0;
}

