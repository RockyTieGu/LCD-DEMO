#ifndef _W55FA93_VPOST_DRIVER_H_
#define _W55FA93_VPOST_DRIVER_H_
#include "w55fa93_vpost.h"

void vpostVAStartTrigger(void);
void vpostVAStopTrigger(void);
void vpostVAStartTrigger_MPUContinue(void);
void vpostVAStartTrigger_MPUSingle(void);
void vpostVAStopTriggerMPU(void);
char vpostAllocVABuffer(PLCDFORMATEX plcdformatex,unsigned int nBytesPixel);
char vpostAllocVABufferFromAP(unsigned int *pFramebuf);
char vpostClearVABuffer(void);
char vpostFreeVABuffer(void);

void vpostSetSyncLCM_HTiming(S_DRVVPOST_SYNCLCM_HTIMING *psHTiming);
void vpostSetSyncLCM_VTiming(S_DRVVPOST_SYNCLCM_VTIMING *psVTiming);
void vpostSetLCDEnable(char bYUVBL, unsigned char ucVASrcType, char bLCDRun);
void vpostMPULCDWriteAddr16Bit(unsigned short u16AddrIndex);
void vpostMPULCDWriteData16Bit(unsigned short  u16WriteData);
unsigned short vpostMPULCDReadData16Bit(void);
void vpostEnableInt(E_DRVVPOST_int eInt);
void vpostDisableInt(E_DRVVPOST_int eInt);
void vpostClearInt(E_DRVVPOST_int eInt);
char vpostIsIntEnabled(E_DRVVPOST_int eInt);
//static void vpostISR(void);
//int vpostInstallCallBack(E_DRVVPOST_int eIntSource,PFN_DRVVPOST_int_CALLBACKpfnCallback,PFN_DRVVPOST_int_CALLBACK 	*pfnOldCallback);
void vpostSetMPULCM_ImageWindow(volatile S_DRVVPOST_MPULCM_WINDOW *psWindow);
void vpostSetMPULCM_TimingConfig(volatile S_DRVVPOST_MPULCM_TIMING *psTiming);
void vpostSetMPULCM_BusModeSelect(E_DRVVPOST_MPULCM_DATABUS eBusMode);
void vpostSetSyncLCM_ImageWindow(S_DRVVPOST_SYNCLCM_WINDOW *psWindow);
void vpostSetSyncLCM_SignalPolarity(S_DRVVPOST_SYNCLCM_POLARITY *psPolarity);


#endif
