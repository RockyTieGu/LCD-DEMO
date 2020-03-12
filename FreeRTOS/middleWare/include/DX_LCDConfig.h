#ifndef _DX_LCDConfig_H_
#define _DX_LCDConfig_H_
#include "wbtypes.h"

/* LCD type defined  
S035QL01H :  CENTRY LCD TYPE NAME.
    LVDS/MCU/RGB:  interface type
    SPINIT:  Whether LCD needs to be initialized,
    320_240: LCD resolution size
    KEY/TP/CTP: HMI interaction mode
   
*/
enum CENTRY_LCD_TYPE1
{
  C035QL01N_RGB_SPINIT_320_240_KEY = 0,
  C035QC05N_RGB_NOINIT_320_240_KEY = 1,
  C043WVH02NE_RGB_NOINIT_800_480_KEY =2 ,
  C050WQ01_RGB_NOINIT_480_272_KEY = 3,
  C070WSH01HH_RGB_NOINIT_1024_600_KEY =4	
};

#define SELECT_LCD_TYPE 			(0)

#if (SELECT_LCD_TYPE == 0)	
	#define	_LCD_WIDTH				(320)
	#define	_LCD_HEIGHT				(240)

	// lcd clock frequency[1~120]MHZ
	#define LCD_CLOCK_FREQUENCY			(7)

	//Current LCD type name
	#define CENTRY_LCD_TYPE			"S035QL01N" 				
	//system  software version
	#define CENTRY_LCD_SW_VERSION	"V0.1"
	// Horizontal sync pulse width value: [0~255]
	#define LCD_RGB_HPW 				(50)
	// Horizontal back porch value: [0~255]
	#define LCD_RGB_HBP 				(18) 
	// Horizontal front porch value: [0~255]
	#define LCD_RGB_HFP 				(10)
	// Vertical sync pulse width value: [0~255]
	#define LCD_RGB_VPW 				(3)
	// Vertical back porch value: [0~255]
	#define LCD_RGB_VBW 				(17)
	// Vertical front porch width value: [0~255]
	#define LCD_RGB_VFW 				(16) 
	// Vsync polarity (TRUE  or FALSE)
	#define LCD_RGB_VSYNC_POL	 		(TRUE)
	// Hsync polarity (TRUE  or FALSE)
	#define LCD_RGB_HSYNC_POL	 		(TRUE)
	// VDEN polarity  (TRUE  or FALSE)
	#define LCD_RGB_DE_POL				(TRUE)
	// CLOCK polarity (TRUE  or FALSE)
	#define LCD_RGB_CLOCK_POL			(TRUE)

#elif (SELECT_LCD_TYPE == 1)
	#define	_LCD_WIDTH				(320)
	#define	_LCD_HEIGHT				(240)
	
	// lcd clock frequency[1~120]MHZ
	#define LCD_CLOCK_FREQUENCY			(7)
	
	//Current LCD type name
	#define CENTRY_LCD_TYPE			"C035QC05N" 				
	//system  software version
	#define CENTRY_LCD_SW_VERSION	"V0.1"
	// Horizontal sync pulse width value: [0~255]
	#define LCD_RGB_HPW 				(50)
	// Horizontal back porch value: [0~255]
	#define LCD_RGB_HBP 				(18) 
	// Horizontal front porch value: [0~255]
	#define LCD_RGB_HFP 				(10)
	// Vertical sync pulse width value: [0~255]
	#define LCD_RGB_VPW 				(3)
	// Vertical back porch value: [0~255]
	#define LCD_RGB_VBW 				(17)
	// Vertical front porch width value: [0~255]
	#define LCD_RGB_VFW 				(16) 
	// Vsync polarity (TRUE  or FALSE)
	#define LCD_RGB_VSYNC_POL	 		(TRUE)
	// Hsync polarity (TRUE  or FALSE)
	#define LCD_RGB_HSYNC_POL	 		(TRUE)
	// VDEN polarity  (TRUE  or FALSE)
	#define LCD_RGB_DE_POL				(TRUE)
	// CLOCK polarity (TRUE  or FALSE)
	#define LCD_RGB_CLOCK_POL			(TRUE)

#elif (SELECT_LCD_TYPE == 2)
	#define	_LCD_WIDTH				(800)
	#define	_LCD_HEIGHT				(480)
	
	// lcd clock frequency[1~120]MHZ
	#define LCD_CLOCK_FREQUENCY			(15)
	
	//Current LCD type name
	#define CENTRY_LCD_TYPE			"C043WVH02NE" 				
	//system  software version
	#define CENTRY_LCD_SW_VERSION	"V0.1"
	// Horizontal sync pulse width value: [0~255]
	#define LCD_RGB_HPW 				(2)
	// Horizontal back porch value: [0~255]
	#define LCD_RGB_HBP 				(0x2c) 
	// Horizontal front porch value: [0~255]
	#define LCD_RGB_HFP 				(0x32)
	// Vertical sync pulse width value: [0~255]
	#define LCD_RGB_VPW 				(3)
	// Vertical back porch value: [0~255]
	#define LCD_RGB_VBW 				(0x14)
	// Vertical front porch width value: [0~255]
	#define LCD_RGB_VFW 				(0x22) 
	// Vsync polarity (TRUE  or FALSE)
	#define LCD_RGB_VSYNC_POL	 		(FALSE)
	// Hsync polarity (TRUE  or FALSE)
	#define LCD_RGB_HSYNC_POL	 		(FALSE)
	// VDEN polarity  (TRUE  or FALSE)
	#define LCD_RGB_DE_POL				(TRUE)
	// CLOCK polarity (TRUE  or FALSE)
	#define LCD_RGB_CLOCK_POL			(FALSE)


#elif (SELECT_LCD_TYPE == 3)
	#define	_LCD_WIDTH				(480)
	#define	_LCD_HEIGHT				(272)
	
	// lcd clock frequency[1~120]MHZ
	#define LCD_CLOCK_FREQUENCY			(15)
	
	// Horizontal sync pulse width value: [0~255]
	#define LCD_RGB_HPW 				(50)
	// Horizontal back porch value: [0~255]
	#define LCD_RGB_HBP 				(18) 
	// Horizontal front porch value: [0~255]
	#define LCD_RGB_HFP 				(10)
	// Vertical sync pulse width value: [0~255]
	#define LCD_RGB_VPW 				(13)
	// Vertical back porch value: [0~255]
	#define LCD_RGB_VBW 				(17)
	// Vertical front porch width value: [0~255]
	#define LCD_RGB_VFW 				(16) 
	// Vsync polarity (TRUE  or FALSE)
	#define LCD_RGB_VSYNC_POL	 		(TRUE)
	// Hsync polarity (TRUE  or FALSE)
	#define LCD_RGB_HSYNC_POL	 		(TRUE)
	// VDEN polarity  (TRUE  or FALSE)
	#define LCD_RGB_DE_POL				(TRUE)
	// CLOCK polarity (TRUE  or FALSE)
	#define LCD_RGB_CLOCK_POL			(FALSE)
	//Current LCD type name
	#define CENTRY_LCD_TYPE			"C050WQ01" 				
	//system  software version
	#define CENTRY_LCD_SW_VERSION	"V0.1"
	
	
#elif (SELECT_LCD_TYPE == 4)
	#define	_LCD_WIDTH				(1024)
	#define	_LCD_HEIGHT				(600)
	
	// lcd clock frequency[1~120]MHZ
	#define LCD_CLOCK_FREQUENCY			(30)
	//Current LCD type name
	#define CENTRY_LCD_TYPE			"C070WSH01HH" 				
	//system  software version
	#define CENTRY_LCD_SW_VERSION	"V0.1"
	
	// Horizontal sync pulse width value: [0~255]
	#define LCD_RGB_HPW 				(2)
	// Horizontal back porch value: [0~255]
	#define LCD_RGB_HBP 				(0xa0) 
	// Horizontal front porch value: [0~255]
	#define LCD_RGB_HFP 				(0xa2)
	// Vertical sync pulse width value: [0~255]
	#define LCD_RGB_VPW 				(2)
	// Vertical back porch value: [0~255]
	#define LCD_RGB_VBW 				(0x18)
	// Vertical front porch width value: [0~255]
	#define LCD_RGB_VFW 				(0x16) 
	// Vsync polarity (TRUE  or FALSE)
	#define LCD_RGB_VSYNC_POL	 		(FALSE)
	// Hsync polarity (TRUE  or FALSE)
	#define LCD_RGB_HSYNC_POL	 		(FALSE)
	// VDEN polarity  (TRUE  or FALSE)
	#define LCD_RGB_DE_POL				(TRUE)
	// CLOCK polarity (TRUE  or FALSE)
	#define LCD_RGB_CLOCK_POL			(FALSE)


#endif

/*To drive the C035QC05N lcd ,the DE pin must be forced to GND */

//Specify the number of pixel clock in each line
#define LCD_PIXEL_PER_LINE			(_LCD_WIDTH)

//Specify the number of active lines per screen
#define LCD_LINE_PER_PANEL			(_LCD_HEIGHT)

//Specify the number of pixel in each line or row of screen
#define LCD_PIXEL_PER_LINE			(_LCD_WIDTH)

//System  hardware version
#define CENTRY_LCD_HW_VERSION	"V2.0"

//Set lcd color bits ,16BIT 18BITS .defautlt lcd color is 16 bits
#define LCD_BITS_MODE			(16)

//select LCD buffer data formate
#if	(LCD_BITS_MODE==16)
#define	DX_LCD_COLOR	unsigned short
#elif (LCD_BITS_MODE==18)
#define	DX_LCD_COLOR	unsigned int
#endif

#endif
