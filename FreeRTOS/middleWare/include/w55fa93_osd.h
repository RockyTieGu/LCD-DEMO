#ifndef _W55FA93_OSD_H_
#define _W55FA93_OSD_H_

// Frame buffer data selection
#define DRVVPOST_OSD_RGB555     0x8   // RGB555
#define DRVVPOST_OSD_RGB565     0x9   // RGB565
#define DRVVPOST_OSD_RGBx888    0xA   // xRGB888
#define DRVVPOST_OSD_RGB888x    0xB   // RGBx888
#define DRVVPOST_OSD_ARGB888    0xC   // ARGB888
#define DRVVPOST_OSD_CBYCRY     0x0   // Cb0  Y0  Cr0  Y1
#define DRVVPOST_OSD_YCBYCR     0x1   // Y0  Cb0  Y1  Cr0
#define DRVVPOST_OSD_CRYCBY     0x2   // Cr0  Y0  Cb0  Y1
#define DRVVPOST_OSD_YCRYCB     0x3   // Y0  Cr0  Y1  Cb0

typedef struct
{
    unsigned int ucOSDSrcFormat;         //User input Display source format
    unsigned int nOSDScreenWidth;          //Driver output,LCD width
    unsigned int nOSDScreenHeight;         //Driver output,LCD height
    unsigned int nOSDX1StartPixel;
    unsigned int nOSDY1StartPixel;
}OSDFORMATEX,*POSDFORMATEX;

typedef enum {
				eOSD_SHOW = 0x01, 
				eOSD_HIDE,
				eOSD_CLEAR,
				eOSD_SET_TRANSPARENT,
				eOSD_CLEAR_TRANSPARENT
} E_DRVVPOST_OSD_CTL;


void vpostOSDInit(POSDFORMATEX posdformatex, unsigned int *pOSDFramebuf);
void *vpostGetOSDFrameBuffer(void);
int vpostSpitOSD(unsigned int OSDX1End, unsigned int OSDY1End, unsigned int XBarOffset, unsigned int YBarOffset);
void vpostGetBarXY(unsigned int *OSDX1End, unsigned int *OSDY1End, unsigned int *XBarOffset, unsigned int *YBarOffset);
int vpostOSDControl(int cmd, int arg);
void vpostChangeOSDFrameBuffer(unsigned int *pOSDFramebuf);
#endif
