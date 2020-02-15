#ifndef _XBF_FONT_H_H_
#define _XBF_FONT_H_H_
#include "GUIDEMO.h"


int _cbGetData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer);
void _ShowXBF(void) ;
GUI_FONT * SetFont_Xbf(void);
char * zh(char * pText);
char * unicode_to_utf8(char *in) ;
unsigned short fsGBToUnicode(					//return the number of UCS2 code that have been converted.
						unsigned short *ucs2_ptr, 	//destination: string pointer to UCS2 string.
						const char *gb_ptr,//source: string pointer to GB string
						unsigned short len			//the length to convert from GB to UCS2.
						);
int UniToGB2312(unsigned char *unicode, unsigned short len, unsigned char *str);
#endif

