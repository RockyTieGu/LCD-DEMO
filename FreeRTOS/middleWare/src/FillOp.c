#include <stdio.h>
#include "wb_reg.h"
#include "blt.h"
#include "DrvBLT.h"
#include "FI.h"
#include "w55fa93_vpost.h"

#define RECT_TEST_STEP_SIZE 1
extern volatile DX_LCD_COLOR   *_VpostFrameBuffer;

void ClearFrameBuffer(void)
{
	// Fill Frame Buffer with Black color
	 FillFrameBuffer((unsigned int)_VpostFrameBuffer, VPOST_WIDTH, VPOST_HEIGHT, gDisplayFormat, 0xFF, 0xFF, 0x00, 0xff);
}

void fillRect(unsigned int u32BufAddr, unsigned int s_x, unsigned int s_y,unsigned int Width, unsigned int Height, E_DRVBLT_DISPLAY_FORMAT eDestFmt,unsigned char u8R, unsigned char u8G, unsigned char u8B, unsigned char u8A)
{
	   S_FI_FILLOP s_ClearOP;
    DrvBLT_SetRevealAlpha(eDRVBLT_NO_EFFECTIVE);
       
    s_ClearOP.sRect.i16Xmin = s_x;
    s_ClearOP.sRect.i16Ymin = s_y;  
    s_ClearOP.sRect.i16Xmax = Width;
    s_ClearOP.sRect.i16Ymax = Height;      
    
    s_ClearOP.sARGB8.u8Blue=u8B;
    s_ClearOP.sARGB8.u8Green=u8G;
    s_ClearOP.sARGB8.u8Red=u8R;   
    s_ClearOP.sARGB8.u8Alpha=u8A;            
    
    s_ClearOP.u32FBAddr = u32BufAddr;
    s_ClearOP.i32Stride = Width;
   // s_ClearOP.i32Stride = GetDestRowByte(eDestFmt,Width);
    s_ClearOP.eDisplayFmt = eDestFmt;
    s_ClearOP.i32Blend = 0;
    
    SI_Fill(s_ClearOP);
}

void FillFrameBuffer(unsigned int u32BufAddr, unsigned int Width, unsigned int Height, E_DRVBLT_DISPLAY_FORMAT eDestFmt,unsigned char u8R, unsigned char u8G, unsigned char u8B, unsigned char u8A)
{
    S_FI_FILLOP s_ClearOP;
    
    DrvBLT_SetRevealAlpha(eDRVBLT_NO_EFFECTIVE);
       
    s_ClearOP.sRect.i16Xmin = 0;
    s_ClearOP.sRect.i16Ymin = 0;  
    s_ClearOP.sRect.i16Xmax = Width;
    s_ClearOP.sRect.i16Ymax = Height;      
    
    s_ClearOP.sARGB8.u8Blue=u8B;
    s_ClearOP.sARGB8.u8Green=u8G;
    s_ClearOP.sARGB8.u8Red=u8R;   
    s_ClearOP.sARGB8.u8Alpha=u8A;            
    
    s_ClearOP.u32FBAddr = u32BufAddr;
    //s_ClearOP.i32Stride = Width;					//Width	  VPOST_WIDTH
    s_ClearOP.i32Stride = GetDestRowByte(eDestFmt,Width);
    s_ClearOP.eDisplayFmt = eDestFmt;
    s_ClearOP.i32Blend = 0;
    
    SI_Fill(s_ClearOP);
     
}














