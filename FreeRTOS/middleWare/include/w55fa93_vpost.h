/***************************************************************************
 *                                                                         *
 * Copyright (c) 2007 - 2009 Nuvoton Technology Corp. All rights reserved.*
 *                                                                         *
 ***************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     w55fa93_vpost.h
 *
 * VERSION
 *     0.1 
 *
 * DESCRIPTION
 *
 *
 *
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *
 *
 *     
 * HISTORY
 *     2009.03.16		Created by Shu-Ming Fan
 *
 *
 * REMARK
 *     None
 *
 *
 **************************************************************************/
 
#ifndef _NUC930_VPOST_H_
#define _NUC930_VPOST_H_
#include "wb_include.h"
#include "midd_comm.h"
#include "dx_lcdconfig.h"

//#define	_LCD_WIDTH	800
//#define	_LCD_HEIGHT	480
#define _LCD_BPP	LCD_BITS_MODE	// 16BIT 18BIT 24BIT
#define VPOST_FRAME_BUFSZ			(_LCD_HEIGHT*_LCD_WIDTH*sizeof(DX_LCD_COLOR)) 
#define	FLASH_VIDEO_ADDR	0x500000
#define	FLASH_VIDEO_SIZE	0x200000
typedef __align(32) DX_LCD_COLOR  (*LCDBUFFSRC)[_LCD_WIDTH];
// Define one bit mask
#define BIT0		0x00000001
#define BIT1		0x00000002
#define BIT2		0x00000004
#define BIT3		0x00000008
#define BIT4		0x00000010
#define BIT5		0x00000020
#define BIT6		0x00000040
#define BIT7		0x00000080
#define BIT8		0x00000100
#define BIT9		0x00000200
#define BIT10	0x00000400
#define BIT11	0x00000800
#define BIT12	0x00001000
#define BIT13	0x00002000
#define BIT14	0x00004000
#define BIT15	0x00008000
#define BIT16	0x00010000
#define BIT17	0x00020000
#define BIT18	0x00040000
#define BIT19	0x00080000
#define BIT20	0x00100000
#define BIT21	0x00200000
#define BIT22	0x00400000
#define BIT23	0x00800000
#define BIT24	0x01000000
#define BIT25	0x02000000
#define BIT26	0x04000000
#define BIT27	0x08000000
#define BIT28	0x10000000
#define BIT29	0x20000000
#define BIT30	0x40000000
#define BIT31	0x80000000


#define LCM_ERR_ID				0xFFF06000	/* LCM library ID */

//#define	COLOR_INVERT
#define	LOADFROMFLASH
/* error code */
#define ERR_NULL_BUF 			(LCM_ERR_ID | 0x04)	//error memory location
#define ERR_NO_DEVICE			(LCM_ERR_ID | 0x05)	//error no device
#define ERR_BAD_PARAMETER       (LCM_ERR_ID | 0x06) //error for bad parameter
#define ERR_POWER_STATE			(LCM_ERR_ID | 0x07)	//error power state control

// Frame buffer data selection
#define DRVVPOST_FRAME_RGB555     0x0   // RGB555
#define DRVVPOST_FRAME_RGB565     0x1   // RGB565
#define DRVVPOST_FRAME_RGBx888    0x2   // xRGB888
#define DRVVPOST_FRAME_RGB888x    0x3   // RGBx888
#define DRVVPOST_FRAME_CBYCRY     0x4   // Cb0  Y0  Cr0  Y1
#define DRVVPOST_FRAME_YCBYCR     0x5   // Y0  Cb0  Y1  Cr0
#define DRVVPOST_FRAME_CRYCBY     0x6   // Cr0  Y0  Cb0  Y1
#define DRVVPOST_FRAME_YCRYCB     0x7   // Y0  Cr0  Y1  Cb0

// LCD and TV source selection
#define	DRVVPOST_COLOR_LINEBUFFER       0x0 // Line Buffer
#define	DRVVPOST_COLOR_FRAMEBUFFER      0x1 // Frame Buffer
#define	DRVVPOST_COLOR_REGISTERSETTING  0x2 // Register Setting Color
#define	DRVVPOST_COLOR_COLORBAR         0x3 // Internal Color Bar

#define DRVVPOST_CLK_13_5MHZ	0x0       // TV Color Modulation Method (Using 13.5MHz)
#define DRVVPOST_CLK_27MHZ      0x1       // TV Color Modulation Method (Using 27MHz)
#define DRVVPOST_CLK_PLL        0x4       // TV Color Modulation Method (Using PLL output)

// GPU size setting
#define DRVVPOST_VGA_SIZE			0x1   
#define DRVVPOST_QVGA_SIZE			0x0

// Frame Buffer size setting
#define DRVVPOST_FRAME_BUFFER_D1_SIZE		0x3   
#define DRVVPOST_FRAME_BUFFER_VGA_SIZE		0x2   
#define DRVVPOST_FRAME_BUFFER_HVGA_SIZE		0x1   
#define DRVVPOST_FRAME_BUFFER_QVGA_SIZE		0x0   

// LCD output path selection
#define DRVVPOST_DATA_2_EBI					0x00   
#define DRVVPOST_DATA_2_LCD					0x01   
#define DRVVPOST_DATA_2_EBI_PLUS_LCD		0x02   

// MPU Command & Display Mode selection
#define DRVVPOST_MPU_CMD_MODE				0x1
#define DRVVPOST_MPU_NORMAL_MODE			0x0

// Data Format Transfer selection
#define DRVVPOST_DataFormatTransfer_RGB888	0x1
#define DRVVPOST_DataFormatTransfer_YCbCr	0x0

// VPOST interrupt Flag
#define DRVVPOST_Hint						0x1
#define DRVVPOST_Vint						0x2
#define DRVVPOST_TVFIELDint					0x4
#define DRVVPOST_MPUCPLint					0x10

// LCD data interface selection
#define DRVVPOST_LCDDATA_YUV422   0x0   // YUV422(CCIR601)
#define DRVVPOST_LCDDATA_RGB      0x1   // Dummy serial (R, G, B Dummy) (default)
#define DRVVPOST_LCDDATA_CCIR656  0x2   // CCIR656 interface
#define DRVVPOST_LCDDATA_RGBGBR   0x4   // Odd line data is RGB, even line is GBR
#define DRVVPOST_LCDDATA_BGRRBG   0x5   // Odd line data is BGR, even line is RBG
#define DRVVPOST_LCDDATA_GBRRGB   0x6   // Odd line data is GBR, even line is RGB
#define DRVVPOST_LCDDATA_RBGBGR   0x7   // Odd line data is RBG, even line is BGR

// LCD device type
#define DRVVPOST_LCD_HIGHRESOLUTION  0x0  // High Resolution mod
#define DRVVPOST_LCD_SYNCTYPE_TFT    0x1  // Sync-type TFT LCD
#define DRVVPOST_LCD_STNCTYPE_STN    0x2  // Sync-type Color STN LCD
#define DRVVPOST_LCD_MPUTYPE         0x3  // MPU-type LCD

// LCD information structure
typedef struct
{
    unsigned int ucVASrcFormat;         //User input Display source format
    unsigned int nScreenWidth;          //Driver output,LCD width
    unsigned int nScreenHeight;         //Driver output,LCD height
    unsigned int nFrameBufferSize;      //Driver output,Frame buffer size(malloc by driver)
    unsigned char ucROT90;                //Rotate 90 degree or not
}LCDFORMATEX,*PLCDFORMATEX;


typedef enum {
				eDRVVPOST_Hint = 0x01, 
				eDRVVPOST_Vint = 0x02,
				eDRVVPOST_TVFIELDint = 0x04,
				eDRVVPOST_MPUCPLint = 0x10
											} E_DRVVPOST_int;


typedef enum {
				eDRVVPOST_SYNC_TV = 0, 
				eDRVVPOST_ASYNC_TV
											} E_DRVVPOST_TIMING_TYPE;
											
typedef enum {
				eDRVVPOST_RESERVED = 0, 
				eDRVVPOST_FRAME_BUFFER,
				eDRVVPOST_REGISTER_SETTING,				
				eDRVVPOST_COLOR_BAR
											} E_DRVVPOST_IMAGE_SOURCE;
											
											
typedef enum {
				eDRVVPOST_DUPLICATED = 0, 
				eDRVVPOST_intERPOLATION
											} E_DRVVPOST_IMAGE_SCALING;
											
typedef enum {
				eDRVVPOST_HIGH_RESOLUTINO_SYNC = 0,	// 16/18/24-bit sync type LCM
				eDRVVPOST_SYNC = 1, 				// 8-bit sync type LCM				
				eDRVVPOST_MPU = 3					// 8/9/16/18/24-bit MPU type LCM

											} E_DRVVPOST_LCM_TYPE;

typedef enum {
				eDRVVPOST_I80 = 0,	
				eDRVVPOST_M68
											} E_DRVVPOST_MPU_TYPE;
											
typedef enum {
				eDRVVPOST_SRGB_YUV422 = 0,	// CCIR601
				eDRVVPOST_SRGB_RGBDUMMY, 	// RGB Dummy Mode
				eDRVVPOST_SRGB_CCIR656,		// CCIR656
				eDRVVPOST_SRGB_RGBTHROUGH	// RGB ThroughMode				
											} E_DRVVPOST_8BIT_SYNCLCM_intERFACE;

typedef enum {
				eDRVVPOST_CCIR656_360 = 0,	// CCIR656 320 mode
				eDRVVPOST_CCIR656_320 		// CCIR656 320 mode
											} E_DRVVPOST_CCIR656_MODE;

typedef enum {
				eDRVVPOST_YUV_BIG_ENDIAN = 0,	// Bigh endian
				eDRVVPOST_YUV_LITTLE_ENDIAN 	// Little endian
											} E_DRVVPOST_ENDIAN;

typedef enum {
				eDRVVPOST_SRGB_RGB = 0,		
				eDRVVPOST_SRGB_BGR, 		
				eDRVVPOST_SRGB_GBR,			
				eDRVVPOST_SRGB_RBG		
											} E_DRVVPOST_SERAIL_SYNCLCM_COLOR_ORDER;

typedef enum {
				eDRVVPOST_PRGB_16BITS = 0,		
				eDRVVPOST_PRGB_18BITS, 		
				eDRVVPOST_PRGB_24BITS			
											} E_DRVVPOST_PARALLEL_SYNCLCM_intERFACE;

typedef enum {
				eDRVVPOST_SYNC_8BITS = 0,		
				eDRVVPOST_SYNC_9BITS,
				eDRVVPOST_SYNC_16BITS,				
				eDRVVPOST_SYNC_18BITS,		
				eDRVVPOST_SYNC_24BITS						
											} E_DRVVPOST_SYNCLCM_DATABUS;

typedef enum {
				eDRVVPOST_MPU_8_8 = 0,
				eDRVVPOST_MPU_2_8_8,
				eDRVVPOST_MPU_6_6_6,
				eDRVVPOST_MPU_8_8_8,
				eDRVVPOST_MPU_9_9,
				eDRVVPOST_MPU_16,
				eDRVVPOST_MPU_16_2,
				eDRVVPOST_MPU_2_16,
				eDRVVPOST_MPU_16_8,
				eDRVVPOST_MPU_18,
				eDRVVPOST_MPU_18_6,
				eDRVVPOST_MPU_24
											} E_DRVVPOST_MPULCM_DATABUS;


typedef enum {
				eDRVVPOST_FRAME_RGB555 = 0,		
				eDRVVPOST_FRAME_RGB565, 		
				eDRVVPOST_FRAME_RGBx888,
				eDRVVPOST_FRAME_RGB888x,
				eDRVVPOST_FRAME_CBYCRY,
				eDRVVPOST_FRAME_YCBYCR,
				eDRVVPOST_FRAME_CRYCBY,
				eDRVVPOST_FRAME_YCRYCB
											} E_DRVVPOST_FRAME_DATA_TYPE;

typedef struct {
				unsigned char u8PulseWidth;  	 	// Horizontal sync pulse width
				unsigned char u8BackPorch;    		// Horizontal back porch
				unsigned char u8FrontPorch;    		// Horizontal front porch
											} S_DRVVPOST_SYNCLCM_HTIMING;

typedef struct {
				unsigned char u8PulseWidth;   		// Vertical sync pulse width
				unsigned char u8BackPorch;    		// Vertical back porch
				unsigned char u8FrontPorch;    		// Vertical front porch
											} S_DRVVPOST_SYNCLCM_VTIMING;

typedef struct {
				unsigned short u16ClockPerLine;		// Specify the number of pixel clock in each line or row of screen
				unsigned short u16LinePerPanel;    	// Specify the number of active lines per screen
				unsigned short u16PixelPerLine;   	// Specify the number of pixel in each line or row of screen
											} S_DRVVPOST_SYNCLCM_WINDOW;

typedef enum {
				eDRVVPOST_DATA_8BITS = 0,		
				eDRVVPOST_DATA_9BITS,
				eDRVVPOST_DATA_16BITS,			
				eDRVVPOST_DATA_18BITS,					
				eDRVVPOST_DATA_24BITS		
											} E_DRVVPOST_DATABUS;

typedef enum{
	VPOST=0,		//直接刷入到显存
	BUFF0,
	BUFF1,
	BUFF2,
}LCD_BUFFER;


typedef struct {
				char bIsVsyncActiveLow;		// Vsync polarity
				char bIsHsyncActiveLow;		// Hsync polarity
				char bIsVDenActiveLow;		// VDEN polarity				
				char bIsDClockRisingEdge;		// VDEN polarity								
											} S_DRVVPOST_SYNCLCM_POLARITY;

// for MVsync & FrameMark signal
typedef struct {
				unsigned short u16LinePerPanel;    	// Specify the number of active lines per screen
				unsigned short u16PixelPerLine;   	// Specify the number of pixel in each line or row of screen
											} S_DRVVPOST_MPULCM_WINDOW; 	

typedef struct {
				unsigned char  u8CSnF2DCt;			// CSn fall edge to Data change clock counter
				unsigned char  u8WRnR2CSnRt;		// WRn rising edge to CSn rising  clock counter
				unsigned char  u8WRnLWt;			// WR Low pulse clock counter
				unsigned char  u8CSnF2WRnFt;		// Csn fall edge To WR falling edge clock counter
											} S_DRVVPOST_MPULCM_TIMING;


typedef struct {
				char  	bIsSyncWithTV;				
				char  	bIsVsyncSignalOut;
				char  	bIsFrameMarkSignalIn;				
				E_DRVVPOST_IMAGE_SOURCE		eSource;
				E_DRVVPOST_LCM_TYPE			eType;
				E_DRVVPOST_MPU_TYPE			eMPUType;				
				E_DRVVPOST_MPULCM_DATABUS	eBus;
				S_DRVVPOST_MPULCM_WINDOW*	psWindow;			
				S_DRVVPOST_MPULCM_TIMING*	psTiming;
} S_DRVVPOST_MPULCM_CTRL;


typedef int (*PFN_DRVVPOST_int_CALLBACK)(unsigned char*, unsigned int);

#if _LCD_BPP == 16
#define		LCD_GUI_BLACK 			(0x0000)
#define		LCD_GUI_BLUE 			(0x001F)
#define		LCD_GUI_RED				(0xF800)
#define		LCD_GUI_GREEN			(0x07E0)
#define 	LCD_GUI_CYAN			(0x07FF)
#define 	LCD_GUI_MAGENTA 		(0xF81F)
#define 	LCD_GUI_YELLOW			(0xFFE0)
#define 	LCD_GUI_WHITE 			(0xFFFF)

#define 	LCD_GUI_LIGHTGREEN     	(0x1db8)//浅绿色
//以上三色为PANEL的颜色 
#elif _LCD_BPP == 18
#define		LCD_GUI_BLACK 			(0x000000)
#define		LCD_GUI_BLUE 			(0x0000FF)
#define		LCD_GUI_RED				(0xFF0000)
#define		LCD_GUI_GREEN			(0x00FF00)
#define 	LCD_GUI_CYAN			(0x00FFFF)
#define 	LCD_GUI_MAGENTA 		(0xFF00FF)
#define 	LCD_GUI_YELLOW			(0xFFFF00)
#define 	LCD_GUI_WHITE 			(0xFFFFFF)
#define 	LCD_GUI_LIGHTGREEN     	(0x032bc0)//浅绿色
#endif

#define	FLASH_SECTOR_SIZE		(64*1024)

#define	FLASH_IMAGE_ADDR		0x10000
#define	FLASH_IMAGE_SIZE		0x70000

#define	FLASH_FONT_ADDR			0x80000
#define	FLASH_FONT_SIZE			0x00000

#define	FLASH_RES_ADDR			0x80000
#define	FLASH_RES_SIZE			0x780000

#define	FLASH_USERLIST_ADDR		0x780000
#define	FLASH_USERLIST_SIZE		0x80000
#define	RES_ADDR				0x380000

/***********************************
* Function Prototype List  *
***********************************/

#define	SetColor(color)		POint_COLOR=color
#define	SetBackColor(color)	BACK_COLOR=color


extern void* g_VAFrameBuf;
extern void* g_VAOrigFrameBuf;
extern unsigned short POint_COLOR,BACK_COLOR;
extern DX_LCD_COLOR __align(32) LCDOSDBuffer1[_LCD_HEIGHT][_LCD_WIDTH];
extern DX_LCD_COLOR __align(32) LCDOSDBuffer2[_LCD_HEIGHT][_LCD_WIDTH];
extern __align(32) DX_LCD_COLOR OSD_FrameRGB565[_LCD_HEIGHT*_LCD_WIDTH];
void systemInit(void);
/*********************************************************/
void vpostVAStartTrigger(void);
void vpostVAStopTrigger(void);
void vpostVAStartTrigger_MPUContinue(void);
void vpostVAStartTrigger_MPUSingle(void);
void vpostVAStopTriggerMPU(void);
char vpostAllocVABuffer(PLCDFORMATEX plcdformatex,unsigned int nBytesPixel);
char vpostAllocVABufferFromAP(unsigned int *pFramebuf);
char vpostClearVABuffer(void);
char vpostFreeVABuffer(void);
void vpostSetLCDEnable(char bYUVBL, unsigned char ucVASrcType, char bLCDRun);
void vpostSetLCDConfig(char bLCDSynTv, unsigned char u8LCDDataSel, unsigned char u8LCDTYPE);
void vpostsetLCM_TimingType(E_DRVVPOST_TIMING_TYPE eTimingTpye);
void vpostSetLCM_TypeSelect(E_DRVVPOST_LCM_TYPE eType);
void vpostSetSerialSyncLCM_Interface(E_DRVVPOST_8BIT_SYNCLCM_intERFACE eInterface);
void vpostSetParalelSyncLCM_Interface(E_DRVVPOST_PARALLEL_SYNCLCM_intERFACE eInterface);
void vpostSetFrameBuffer_DataType(E_DRVVPOST_FRAME_DATA_TYPE eType);
void vpostSetFrameBuffer_BaseAddress(unsigned int u32BufferAddress);
void vpostMPULCDWriteAddr16Bit(unsigned short u16AddrIndex);
void vpostMPULCDWriteData16Bit(unsigned short  u16WriteData);
unsigned short vpostMPULCDReadData16Bit(void);
void vpostSetMPULCM_BusModeSelect(E_DRVVPOST_MPULCM_DATABUS eBusMode);
void vpostSetDataBusPin(E_DRVVPOST_DATABUS eDataBus);
void vpostSetLCM_ImageSource(E_DRVVPOST_IMAGE_SOURCE eSource);
void vpostSetMPULCM_TimingConfig(volatile S_DRVVPOST_MPULCM_TIMING *psTiming);
void vpostSetMPULCM_ImageWindow(volatile S_DRVVPOST_MPULCM_WINDOW *psWindow);
void vpostSetYUVEndianSelect(E_DRVVPOST_ENDIAN eEndian);
/*********************************************************/

int vpostLCMInit_CENTRY(PLCDFORMATEX plcdformatex, unsigned int *pFramebuf);
int vpostLCMDeinit_CENTRY(void);
void *vpostGetFrameBuffer(void);
int vpostLCMDeinit(void);
int vpostLCMInit(PLCDFORMATEX plcdformatex, unsigned int *pFramebuf);
void BacklightControl(int OnOff);							
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
extern void LCDUpdateScreen(unsigned short buff);
void LCDQuickUpdateScreen(unsigned short	buff);
extern void LCDUpdateRect(VPOST_RECT* rect,unsigned short buff);
extern void LCDBufferMove(unsigned int src_addr,unsigned int dest_addr,unsigned int size);
void Vpost_FillRect(unsigned short sx,unsigned short sy,unsigned short ex,unsigned short ey,unsigned short color,unsigned short buff);
void LCDBUffMove221(VPOST_RECT* rect);
void LCDBUffMove321(VPOST_RECT* rect);
void LCDBUffMove320(VPOST_RECT* rect);
void LCDBUffMove112(VPOST_RECT* rect);
void LCD_Clear(unsigned int color,unsigned short buff);
void showIcon(unsigned int index,unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned short alpha);
void showIcon1bitAlpha(unsigned int index,unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned short alpha);
//void showIconbuff3(unsigned int index,unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned short alpha);
void ShowIconBuff2(unsigned int index,unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned short alpha);
void Rotate90showIcon(unsigned int index,unsigned short x,unsigned short y,unsigned short w,unsigned short h,unsigned short alpha);
void BKAlphaSet(unsigned short alpha);
void BKAlphaSet20(void);
void BKAlphaSet50(void);
void DrawBigBackGround(unsigned short x,unsigned short y);
void DrawMidBackGround(unsigned short x,unsigned short y);
void DrawSmallBackGround(unsigned short x,unsigned short y);
void DrawArcRect(unsigned short x,unsigned short y,unsigned short width,unsigned short height,unsigned short color);
void Vpost_DrawHLine_DrawHLine(unsigned short sx,unsigned short ex,unsigned short y,unsigned short color,unsigned short buff);
void Vpost_DrawHLine_DrawVLine(unsigned short x,unsigned short sy,unsigned short ey,unsigned short color,unsigned short buff);
void Fill_Circle(unsigned short x0,unsigned short y0, unsigned short r,unsigned short dcolor);
void Draw_Circle(unsigned short x0,unsigned short y0,unsigned short r,unsigned short dcolor);
void LCD_DrawLine(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,unsigned short dcolor);
void Vpost_DrawCircle(unsigned int x0, unsigned int y0, unsigned int r,unsigned short color);
void UpdateDrawRect(unsigned short x,unsigned short y,unsigned short width,unsigned short height,unsigned short color);
void Vpost_PictureFrame(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2,unsigned short color,unsigned short buff);
#ifndef	LOADFROMFLASH
void LoadRes(void);
#endif
void openRes(void);
void LCD_ShowFixString(unsigned short x,unsigned short y,const char* str);
void lcd_gay(void);
/* function prototype */
						
												
#endif   /* _NUC930_VPOST_H_  */

