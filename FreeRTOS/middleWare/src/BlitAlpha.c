#include <stdio.h>
#include "wb_reg.h"
#include "blt.h"
#include "DrvBLT.h"
//#include "DrvVPOST.h"
#include "FI.h"
#include "w55fa93_vpost.h"

void BlitOpAlpha(unsigned short u16Multiplier, unsigned short u16Offset, unsigned int u32TransformFlag)
{
    S_FI_BLITOP sBlitOpPara;
    S_FI_BLITTRANSFORMATION sTran;
    
 //   CopyDestImageToFB();
    
    sBlitOpPara.sFISrcImage.psARGB8 = default_pallete;
    sBlitOpPara.sFISrcImage.sDrvSrcImage.u32SrcImageAddr = (unsigned int)OSD_FrameRGB565;
//    sBlitOpPara.sFISrcImage.sDrvSrcImage.i32Stride = VPOST_WIDTH;  
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i32XOffset = 0;  
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i32YOffset = 0;      
#if 0
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Width = VPOST_WIDTH;  
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Height = VPOST_HEIGHT;  
#else
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Width = 96;  
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i16Height = 129;  
#endif    
    
    sBlitOpPara.sDestFB.u32FrameBufAddr = (unsigned int)OSD_FrameRGB565;
    sBlitOpPara.sDestFB.i32XOffset = 0;  
    sBlitOpPara.sDestFB.i32YOffset = 0;   
//    sBlitOpPara.sDestFB.i32Stride = VPOST_WIDTH;
#if 0         
    sBlitOpPara.sDestFB.i16Width = VPOST_WIDTH;  
    sBlitOpPara.sDestFB.i16Height = VPOST_HEIGHT;       
#else
    sBlitOpPara.sDestFB.i16Width = 96;  
    sBlitOpPara.sDestFB.i16Height = 129;
#endif    
          
    sBlitOpPara.psTransform = &sTran;     
    
    sBlitOpPara.psTransform->sMatrix.a =  (1<<16);        
    sBlitOpPara.psTransform->sMatrix.b =  0;                
    sBlitOpPara.psTransform->sMatrix.c =  0;      
    sBlitOpPara.psTransform->sMatrix.d =  (1<<16);  
	// Keep original setting            
#if 0	
    sBlitOpPara.psTransform->eSrcFmt = inp32(SFMT);
#else    
	sBlitOpPara.psTransform->eSrcFmt = eDRVBLT_SRC_ARGB8888;    
#endif	
    sBlitOpPara.psTransform->eDestFmt = inp32(DFMT);
    
#if 0    
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i32Stride = GetSrcRowByte(sBlitOpPara.psTransform->eSrcFmt, VPOST_WIDTH);     
#else    
    sBlitOpPara.sFISrcImage.sDrvSrcImage.i32Stride = GetSrcRowByte(sBlitOpPara.psTransform->eSrcFmt, 96);       
#endif    
    sBlitOpPara.sDestFB.i32Stride = GetDestRowByte(sBlitOpPara.psTransform->eDestFmt, VPOST_WIDTH);     
    
    sBlitOpPara.psTransform->i32Flag = u32TransformFlag;
    

    sBlitOpPara.psTransform->sColorMultiplier.i16Blue   = u16Multiplier;   
    sBlitOpPara.psTransform->sColorMultiplier.i16Green  = u16Multiplier;  
    sBlitOpPara.psTransform->sColorMultiplier.i16Red    = u16Multiplier; 
    sBlitOpPara.psTransform->sColorMultiplier.i16Alpha  = u16Multiplier;       
    
    sBlitOpPara.psTransform->sColorOffset.i16Blue   = u16Offset;   
    sBlitOpPara.psTransform->sColorOffset.i16Green  = u16Offset;  
    sBlitOpPara.psTransform->sColorOffset.i16Red    = u16Offset; 
    sBlitOpPara.psTransform->sColorOffset.i16Alpha  = u16Offset;      
         
    sBlitOpPara.psTransform->eFilStyle = (E_DRVBLT_FILL_STYLE)0;    
    
    SI_2DBlit(sBlitOpPara);		
		
 	 
}





