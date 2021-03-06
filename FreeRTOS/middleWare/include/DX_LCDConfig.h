#ifndef _DX_LCDConfig_H_
#define _DX_LCDConfig_H_
#include "wbtypes.h"

#define	_LCD_WIDTH				(1024)
#define	_LCD_HEIGHT				(600)

//System  hardware version
#define CENTRY_LCD_HW_VERSION	"V2.0"

//system  software version
#define CENTRY_LCD_SW_VERSION	"V0.1"

//Current LCD type name
#define CENTRY_LCD_TYPE			"C070WSH01"

//Set lcd color bits ,16BIT 18BITS .defautlt lcd color is 16 bits
#define LCD_BITS_MODE			(16)

//select LCD buffer data formate
#if	(LCD_BITS_MODE==16)
#define	DX_LCD_COLOR	unsigned short
#elif (LCD_BITS_MODE==18)
#define	DX_LCD_COLOR	unsigned int
#endif

/*To drive the C035QC05N lcd ,the DE pin must be forced to GND */
// lcd clock frequency[1~120]MHZ
#define LCD_CLOCK_FREQUENCY			(30)

//Specify the number of pixel clock in each line
#define LCD_PIXEL_PER_LINE			(_LCD_WIDTH)

//Specify the number of active lines per screen
#define LCD_LINE_PER_PANEL			(_LCD_HEIGHT)

//Specify the number of pixel in each line or row of screen
#define LCD_PIXEL_PER_LINE			(_LCD_WIDTH)


#if 1
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
#else
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
#endif

// Vsync polarity (TRUE  or FALSE)
#define LCD_RGB_VSYNC_POL	 		(FALSE)

// Hsync polarity (TRUE  or FALSE)
#define LCD_RGB_HSYNC_POL	 		(FALSE)

// VDEN polarity  (TRUE  or FALSE)
#define LCD_RGB_DE_POL				(TRUE)

// CLOCK polarity (TRUE  or FALSE)
#define LCD_RGB_CLOCK_POL			(FALSE)


#endif
