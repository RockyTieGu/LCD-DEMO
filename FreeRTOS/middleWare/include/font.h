#ifndef	_FONT_H_
#define	_FONT_H_

#include "midd_comm.h"

#define	FONT_L8

enum{
	FONT_18 =0,
	FONT_22,
	FONT_28,
	FONT_36,
	FONT_18B,
	FONT_28B
};

enum{
	TA_H_LEFT=0,
	TA_H_RIGHT,
	TA_H_CENTER
};



#ifndef	FONT_L8
#define	ASCII_12_ADDR	FLASH_FONT_ADDR
#define	ASCII_16_ADDR	FLASH_FONT_ADDR+0x1000
#define	ASCII_24_ADDR	FLASH_FONT_ADDR+0x2400
#define	HZ_12_ADDR	FLASH_FONT_ADDR+0x4800
#define	HZ_16_ADDR	FLASH_FONT_ADDR+0x38460//0x34800
#define	HZ_24_ADDR	FLASH_FONT_ADDR+0x7D4E0//0x74800
#endif

/*void FontInit(void);
void SetFont(UINT8 fontsize);
void SetFontSpace(UINT32 h,UINT32 v);
void GetASCIIFontSize(UINT8 c,UINT16 *w,UINT16* h);
void GetHZFontSize(UINT16 *w,UINT16* h);
UINT16 GetStringLength(const char *str);
UINT16 GetAlphaData(UINT16 c,UINT8 a,UINT16 x,UINT16 y);
void LCD_ShowChar(UINT16 x,UINT16 y,char c);
void LCD_ShowHz(UINT16 x,UINT16 y,UINT16 hz);
//void LCD_ShowString(UINT16 x,UINT16 y,const char* str,UINT32 mode,UINT32 buff);
UINT16 LCD_ShowStringInTxtBox(VPOST_RECT* rect,const char* str,UINT16 len,UINT32 buff);
void LCD_ShowStringInRect(const char*str, VPOST_RECT* rect,UINT32 buff,UINT32 mode);
*/
#endif


