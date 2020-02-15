#include "wb_reg.h"
#include "blt.h"
#include "DrvBLT.h"
#include "FI.h"
#include "palette.h"
#include "w55fa93_vpost.h"
#include "Font.h"

#define	 NULL	0

void CopySrcImageToFB(void)
{
	BlitCopyImageToFB((unsigned int)LCDOSDBuffer2, 0, 0, VPOST_WIDTH, VPOST_HEIGHT, gDisplayFormat);
}

void CopyDestImageToFB(void)
{
	//DrvBLT_SetDisplayFormat(gDisplayFormat);
	DrvBLT_SetSrcFormat(eDRVBLT_SRC_RGB565);
	BlitCopyImageToFB((unsigned int)LCDOSDBuffer1, 0, 0, VPOST_WIDTH, VPOST_HEIGHT, gDisplayFormat);
}

//void BlitCopySrcImageToFB(unsigned int u32Xoffset, unsigned int u32Yoffset, unsigned int Width, unsigned int Height, E_DRVBLT_DISPLAY_FORMAT eDestFmt)
void BlitCopyImageToFB(unsigned int u32SrcAddr, unsigned int u32Xoffset, unsigned int u32Yoffset, unsigned int Width, unsigned int Height, E_DRVBLT_DISPLAY_FORMAT eDestFmt)
{
    S_FI_BLITOP sBlitOpPara;
    
    DrvBLT_SetRevealAlpha(eDRVBLT_NO_EFFECTIVE);
    
    sBlitOpPara.sFISrcImage.psARGB8 = default_pallete;
    sBlitOpPara.sFISrcImage.sDrvSrcImage.u32SrcImageAddr = u32SrcAddr;
    //sBlitOpPara.sFISrcImage.sDrvSrcImage.i32Stride = VPOST_WIDTH; 
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i32Stride = GetSrcRowByte(inp32(SFMT), VPOST_WIDTH);  
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i32XOffset = u32Xoffset;  
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i32YOffset = u32Yoffset;       
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Width = Width;  
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Height = Height;  
    
    sBlitOpPara.sDestFB.u32FrameBufAddr = (unsigned int)OSD_FrameRGB565;
    sBlitOpPara.sDestFB.i32XOffset = u32Xoffset;  
    sBlitOpPara.sDestFB.i32YOffset = u32Yoffset;   
    //sBlitOpPara.sDestFB.i32Stride = VPOST_WIDTH;   
    sBlitOpPara.sDestFB.i32Stride = GetDestRowByte(inp32(DFMT), VPOST_WIDTH);            
    sBlitOpPara.sDestFB.i16Width = Width;  
    sBlitOpPara.sDestFB.i16Height = Height;       
           
    sBlitOpPara.psTransform = NULL;           

    
    SI_2DBlit(sBlitOpPara);
     
}

void BlitOpCopy(unsigned int srcaddr,S_FI_BLITTRANSFORMATION* psTran, unsigned int step)
{
    S_FI_BLITOP sBlitOpPara;    
    int i,j,k,evenline=1,startX;
    int  i32Blend=0;
    unsigned int u32SrcFmt, u32DestFmt;
   
    DrvBLT_SetSrcFormat(eDRVBLT_SRC_RGB565);
    
    sBlitOpPara.sFISrcImage.psARGB8 = default_pallete;
    sBlitOpPara.sFISrcImage.sDrvSrcImage.u32SrcImageAddr = srcaddr;
    sBlitOpPara.sDestFB.u32FrameBufAddr = (unsigned int)OSD_FrameRGB565;    
    sBlitOpPara.psTransform = psTran;     
    
    if (psTran != 0)
        i32Blend=1;
        
    if (psTran != 0)
    {   // Change for new Format
        u32SrcFmt = psTran->eSrcFmt;
        u32DestFmt = psTran->eDestFmt;    
    }
    else
    {   // Keep Origianl Format
        u32SrcFmt = inp32(SFMT);
        u32DestFmt = inp32(DFMT);
    }
            
#if 0                    
    for(i=MIN_RECT_TEST_SIZE;i<=MAX_RECT_TEST_SIZE;i=i+step)
#else
	i = 32;
#endif    
    {
        if (i32Blend)
            CopyDestImageToFB();
        else
            ClearFrameBuffer();
        for(j=0;(j+i)<=VPOST_HEIGHT;j=j+i)
        {
            if (evenline)
            {
                startX=0;
                evenline =0;
            }                
            else
            {
                startX=i;     
                evenline =1;                
            }                
            
            { 
                sBlitOpPara.sFISrcImage.sDrvSrcImage.i32Stride = GetSrcRowByte(u32SrcFmt,VPOST_WIDTH);  
                sBlitOpPara.sFISrcImage.sDrvSrcImage.i32XOffset = 0;  
                sBlitOpPara.sFISrcImage.sDrvSrcImage.i32YOffset = 0;       
                sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Width = VPOST_WIDTH;  
                sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Height = VPOST_HEIGHT;               
                
                sBlitOpPara.sDestFB.i32XOffset = 0;  
                sBlitOpPara.sDestFB.i32YOffset = 0;   
                //sBlitOpPara.sDestFB.i32Stride = VPOST_WIDTH;         
                sBlitOpPara.sDestFB.i32Stride = GetDestRowByte(u32DestFmt,VPOST_WIDTH);
                sBlitOpPara.sDestFB.i16Width = VPOST_WIDTH/2;  
                sBlitOpPara.sDestFB.i16Height = VPOST_HEIGHT;                 
                
                SI_2DBlit(sBlitOpPara); 
            }
        }    
    }
}


void BlitBufferCopy(unsigned int srcaddr,unsigned int destaddr)
{
	S_FI_BLITOP sBlitOpPara;    
	unsigned int u32SrcFmt, u32DestFmt;
   
	DrvBLT_SetSrcFormat(eDRVBLT_SRC_RGB565);
    	sys_flush_and_clean_dcache();
	sBlitOpPara.sFISrcImage.psARGB8 = default_pallete;
	sBlitOpPara.sFISrcImage.sDrvSrcImage.u32SrcImageAddr = srcaddr;
	sBlitOpPara.sDestFB.u32FrameBufAddr = (unsigned int)destaddr;    
	sBlitOpPara.psTransform = 0;     
    
	u32SrcFmt = inp32(SFMT);
	u32DestFmt = inp32(DFMT);
            
	sBlitOpPara.sFISrcImage.sDrvSrcImage.i32Stride = GetSrcRowByte(u32SrcFmt,VPOST_WIDTH);  
	sBlitOpPara.sFISrcImage.sDrvSrcImage.i32XOffset = 0;  
	sBlitOpPara.sFISrcImage.sDrvSrcImage.i32YOffset = 0;       
	sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Width = VPOST_WIDTH;  
	sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Height = VPOST_HEIGHT;
                
	sBlitOpPara.sDestFB.i32XOffset = 0;  
	sBlitOpPara.sDestFB.i32YOffset = 0;   
	//sBlitOpPara.sDestFB.i32Stride = VPOST_WIDTH;         
	sBlitOpPara.sDestFB.i32Stride = GetDestRowByte(u32DestFmt,VPOST_WIDTH);
	sBlitOpPara.sDestFB.i16Width = VPOST_WIDTH;  
	sBlitOpPara.sDestFB.i16Height = VPOST_HEIGHT;                 
                
	SI_2DBlit(sBlitOpPara); 
}

void BlitRectCopy(unsigned int srcaddr,unsigned int destaddr,VPOST_RECT* arect)
{
	S_FI_BLITOP sBlitOpPara;    
	unsigned int u32SrcFmt, u32DestFmt;
   
	DrvBLT_SetSrcFormat(eDRVBLT_SRC_RGB565);
    	sys_flush_and_clean_dcache();
	sBlitOpPara.sFISrcImage.psARGB8 = default_pallete;
	sBlitOpPara.sFISrcImage.sDrvSrcImage.u32SrcImageAddr = srcaddr;
	sBlitOpPara.sDestFB.u32FrameBufAddr = destaddr;    
	sBlitOpPara.psTransform = 0;     
    
	u32SrcFmt = inp32(SFMT);
	u32DestFmt = inp32(DFMT);
            

	sBlitOpPara.sFISrcImage.sDrvSrcImage.i32XOffset = arect->x0;  
	sBlitOpPara.sFISrcImage.sDrvSrcImage.i32YOffset = arect->y0;       
	sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Width = arect->x1-arect->x0;  
	sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Height = arect->y1-arect->y0; 
      sBlitOpPara.sFISrcImage.sDrvSrcImage.i32Stride = GetSrcRowByte(u32SrcFmt,VPOST_WIDTH);   

	
	sBlitOpPara.sDestFB.i32XOffset = arect->x0;  
	sBlitOpPara.sDestFB.i32YOffset = arect->y0;   
	sBlitOpPara.sDestFB.i32Stride = GetDestRowByte(u32DestFmt,VPOST_WIDTH);
	sBlitOpPara.sDestFB.i16Width = arect->x1-arect->x0;  
	sBlitOpPara.sDestFB.i16Height = arect->y1-arect->y0;                 

	SI_2DBlit(sBlitOpPara); 
}





















