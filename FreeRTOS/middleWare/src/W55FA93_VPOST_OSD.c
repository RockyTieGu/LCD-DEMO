#include   "wb_include.h"
#include "w55fa93_vpost.h"
#include "w55fa93_osd.h"



void vpostOSDInit(POSDFORMATEX posdformatex, unsigned int *pOSDFramebuf)
{
	outp32(REG_LCM_OSD_CTL, inp32(REG_LCM_OSD_CTL) & ~OSD_CTL_OSD_FSEL | (posdformatex-> ucOSDSrcFormat << 24));
    outp32(REG_LCM_LINE_STRIPE, inp32(REG_LCM_LINE_STRIPE) & LINE_STRIPE_F1_LSL);           
    outp32(REG_LCM_OSD_SIZE, ((posdformatex->nOSDScreenWidth-1) & 0x3FF) | ((posdformatex->nOSDScreenHeight-1) & 0x3FF)<<16);   // Set OSD Size
    outp32(REG_LCM_OSD_SP, (posdformatex->nOSDX1StartPixel & 0x3FF) | ((posdformatex->nOSDX1StartPixel & 0xFF) << 16));
    
    outp32(REG_LCM_OSD_ADDR, (unsigned int) pOSDFramebuf);
    
    outp32(REG_LCM_OSD_BEP, 0x00010001);        
    outp32(REG_LCM_OSD_BO, 0x00010001);      
}

void *vpostGetOSDFrameBuffer(void)
{
    return (void *)inp32(REG_LCM_OSD_ADDR);
}

void vpostChangeOSDFrameBuffer(unsigned int *pOSDFramebuf)
{
    outp32(REG_LCM_OSD_ADDR, (unsigned int) pOSDFramebuf);
}

int vpostSpitOSD(unsigned int OSDX1End, unsigned int OSDY1End, unsigned int XBarOffset, unsigned int YBarOffset)
{
    
    if ((int)OSDX1End <= (int)(inp32(REG_LCM_OSD_SP) & 0x3FF))
        return -1;
        
    if ((int)OSDY1End <= (int)(inp32(REG_LCM_OSD_SP)>>16 & 0x3FF))
        return -1;
        
    if ((int)(OSDX1End +  XBarOffset) >= (int)(inp32(REG_LCM_OSD_SIZE) & 0x3FF))
        return -1;        
    
    if ((int)(OSDY1End +  YBarOffset) >= (int)(inp32(REG_LCM_OSD_SIZE)>>16 & 0x3FF))
        return -1;     
        
    outp32(REG_LCM_OSD_BEP, (OSDX1End & 0x3FF) | ((OSDY1End & 0x3FF) << 16) );        
    outp32(REG_LCM_OSD_BO, (XBarOffset & 0x3FF) | ((YBarOffset & 0x3FF) << 16) );     
     
    return 0;	    
}

void vpostGetBarXY(unsigned int *OSDX1End, unsigned int *OSDY1End, unsigned int *XBarOffset, unsigned int *YBarOffset)
{
    *OSDX1End = inp32(REG_LCM_OSD_BEP) & 0x3FF;
    *OSDY1End = (inp32(REG_LCM_OSD_BEP) >> 16) & 0x3FF;    
    *XBarOffset = inp32(REG_LCM_OSD_BO) & 0x3FF;
    *YBarOffset = (inp32(REG_LCM_OSD_BO) >> 16) & 0x3FF;     
}



int vpostOSDControl(int cmd, int arg)
{
    
    switch(cmd)
    {
        case eOSD_SHOW:
            outp32(REG_LCM_OSD_CTL, inp32(REG_LCM_OSD_CTL) | OSD_CTL_OSD_EN);        
            break;     
        case eOSD_HIDE:
            outp32(REG_LCM_OSD_CTL, inp32(REG_LCM_OSD_CTL) & ~OSD_CTL_OSD_EN);
            break;  
        case eOSD_CLEAR:
            break;   
        case eOSD_SET_TRANSPARENT:
            outp32(REG_LCM_OSD_CTL, inp32(REG_LCM_OSD_CTL) & ~OSD_CTL_OSD_TC | (arg & 0xFFFF));
            outp32(REG_LCM_OSD_CTL, inp32(REG_LCM_OSD_CTL) | (0x01 << 28));
            break;   
        case eOSD_CLEAR_TRANSPARENT:
            outp32(REG_LCM_OSD_CTL, inp32(REG_LCM_OSD_CTL) & ~BIT28);    
            break;  
    }
    return 0;
    
}
