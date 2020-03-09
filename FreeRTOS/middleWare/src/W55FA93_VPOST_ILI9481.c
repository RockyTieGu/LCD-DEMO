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
	LCDDelay(10000);//100ms
	SET_LCD_RST(1);
	LCDDelay(10000);//100ms
	SET_LCD_RST(0);
	LCDDelay(10000);//50ms
	SET_LCD_RST(1);
	LCDDelay(10000);//100ms

	 SPI_WriteComm(0x0028);SPI_WriteData(0x00);SPI_WriteData(0x06); // set SS and SM bit
	SPI_WriteComm(0x0000);SPI_WriteData(0x00);SPI_WriteData(0x01);// SPI_Write_Data(0x0001); // set 1 line inversion
	SPI_WriteComm(0x0010);SPI_WriteData(0x00);SPI_WriteData(0x00); // set GRAM write direction and BGR=1.
	SPI_WriteComm(0x0007);SPI_WriteData(0x00);SPI_WriteData(0x33); // Resize register
	SPI_WriteComm(0x0011);SPI_WriteData(0x4e);SPI_WriteData(0x70); // set the back porch and front porch
	SPI_WriteComm(0x0002);SPI_WriteData(0x06);SPI_WriteData(0x00); // RGB interface setting
	SPI_WriteComm(0x0003);SPI_WriteData(0x6e);SPI_WriteData(0x68); // Frame marker Position  0x686a

	SPI_WriteComm(0x0001);SPI_WriteData(0x72);SPI_WriteData(0xef); //72 RGB interface polarity      
	SPI_WriteComm(0x0012);SPI_WriteData(0x09);SPI_WriteData(0x99);
	SPI_WriteComm(0x0026);SPI_WriteData(0x38);SPI_WriteData(0x00);


	//*************Power On sequence ****************
	SPI_WriteComm(0x000c);SPI_WriteData(0x00);SPI_WriteData(0x05); // VDV[4:0] for VCOM amplitude
	SPI_WriteComm(0x002a);SPI_WriteData(0x09);SPI_WriteData(0xd5);
	SPI_WriteComm(0x000d);SPI_WriteData(0x00);SPI_WriteData(0x0a); // SAP);SPI_WriteData(BT[3:0]);SPI_WriteData(AP);SPI_WriteData(DSTB);SPI_WriteData(SLP);SPI_WriteData(STB
	SPI_WriteComm(0x000e);SPI_WriteData(0x25);SPI_WriteData(0x00); // DC1[2:0]);SPI_WriteData(DC0[2:0]);SPI_WriteData(VC[2:0]    0X2100
	SPI_WriteComm(0x001e);SPI_WriteData(0x00);SPI_WriteData(0xaf); // Internal reference voltage= Vci;
	SPI_WriteComm(0x0015);SPI_WriteData(0x00);SPI_WriteData(0x58);



	//--------------- Gamma control---------------//
	SPI_WriteComm(0x0030);SPI_WriteData(0x00);SPI_WriteData(0x00); // GRAM horizontal Address
	SPI_WriteComm(0x0031);SPI_WriteData(0x01);SPI_WriteData(0x01); // GRAM Vertical Address
	SPI_WriteComm(0x0032);SPI_WriteData(0x01);SPI_WriteData(0x00);
	SPI_WriteComm(0x0033);SPI_WriteData(0x03);SPI_WriteData(0x05);
	SPI_WriteComm(0x0034);SPI_WriteData(0x07);SPI_WriteData(0x07);
	SPI_WriteComm(0x0035);SPI_WriteData(0x03);SPI_WriteData(0x05);
	SPI_WriteComm(0x0036);SPI_WriteData(0x07);SPI_WriteData(0x07);
	SPI_WriteComm(0x0037);SPI_WriteData(0x02);SPI_WriteData(0x01);
	SPI_WriteComm(0x003a);SPI_WriteData(0x12);SPI_WriteData(0x00);
	SPI_WriteComm(0x003b);SPI_WriteData(0x09);SPI_WriteData(0x00);

	 SPI_WriteComm(0x0044);SPI_WriteData(0xef);SPI_WriteData(0x00);	   //Vertical RAM address position	
	SPI_WriteComm(0x0045);SPI_WriteData(0x00);SPI_WriteData(0x00);	   //Horizontal RAM address position
	SPI_WriteComm(0x0046);SPI_WriteData(0x01);SPI_WriteData(0x3f);	   //VHorizontal RAM address position
	SPI_WriteComm(0x004e);SPI_WriteData(0x00);SPI_WriteData(0x00);	   //Horizontal start position
	SPI_WriteComm(0x004f);SPI_WriteData(0x00);SPI_WriteData(0x00);	   //VHorizontal start position
	SPI_WriteComm(0x0022);	

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
    
    // set frambuffer base address
	if(pFramebuf != NULL) {
		vpostAllocVABufferFromAP(pFramebuf);
	} else {
    	if( vpostAllocVABuffer(plcdformatex, nBytesPixel)==FALSE)
    		return ERR_NULL_BUF;
	} 
	// set frame buffer data format
	vpostSetFrameBuffer_DataType((E_DRVVPOST_FRAME_DATA_TYPE)(plcdformatex->ucVASrcFormat));

	vpostSetYUVEndianSelect(eDRVVPOST_YUV_LITTLE_ENDIAN);

	// enable MPU LCD controller
	vpostVAStartTrigger();
	return 0;
}


int vpostLCMDeinit_CENTRY(VOID)
{
	vpostVAStopTrigger();
	vpostFreeVABuffer();
	outpw(REG_AHBCLK, inpw(REG_AHBCLK) & ~VPOST_CKE);	
	return 0;
}

