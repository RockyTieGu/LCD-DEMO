#ifndef _DX_LCDConfig_H_
#define _DX_LCDConfig_H_
#include "wbtypes.h"

#define	_LCD_WIDTH	800
#define	_LCD_HEIGHT	480

//System  hardware version
#define CENTRY_LCD_HW_VERSION	"V2.0"

//system  software version
#define CENTRY_LCD_SW_VERSION	"V0.1"

//Current LCD type name
#define CENTRY_LCD_TYPE			"C043WVH02NE"

// lcd clock frequency[1~120]MHZ
#define LCD_CLOCK_FREQUENCY	20

//Specify the number of pixel clock in each line
#define LCD_PIXEL_PER_LINE	_LCD_WIDTH

//Specify the number of active lines per screen
#define LCD_LINE_PER_PANEL	_LCD_HEIGHT

//Specify the number of pixel in each line or row of screen
#define LCD_PIXEL_PER_LINE	_LCD_WIDTH

// CLOCK  (TRUE  or FALSE)
#define LCD_RGB_CLOCK_POL	FALSE

// Horizontal sync pulse width value: [0~255]
#define LCD_RGB_HPW 	2

// Horizontal back porch value: [0~255]
#define LCD_RGB_HBP 	0x2c

// Horizontal front porch value: [0~255]
#define LCD_RGB_HFP 	0x32

// Vertical sync pulse width value: [0~255]
#define LCD_RGB_VPW 	3

// Vertical back porch value: [0~255]
#define LCD_RGB_VBW 	0x14

// Vertical front porch width value: [0~255]
#define LCD_RGB_VFW 	0x22

// Vsync polarity (TRUE  or FALSE)
#define LCD_RGB_VSYNC_POL	 TRUE

// Hsync polarity (TRUE  or FALSE)
#define LCD_RGB_HSYNC_POL	 TRUE

// VDEN polarity  (TRUE  or FALSE)
#define LCD_RGB_DE_POL		FALSE

// CLOCK polarity (TRUE  or FALSE)
#define LCD_RGB_CLOCK_POL	FALSE



#endif
