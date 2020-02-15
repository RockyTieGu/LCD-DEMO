/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (c) Nuvoton Technology Corp. All rights reserved.                                             */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/

#ifndef __DRVPWM_H__
#define __DRVPWM_H__


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include "wbio.h"
#include "wb_reg.h"


#ifdef  __cplusplus
extern "C"
{
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*  Define Version number								                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define	PWM_MAJOR_NUM 1
#define	PWM_MINOR_NUM 00
#define	PWM_BUILD_NUM 1

/*---------------------------------------------------------------------------------------------------------*/
/*  PWM Timer 								                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define	PWM_TIMER0     0x0
#define	PWM_TIMER1     0x1
#define	PWM_TIMER2     0x2
#define	PWM_TIMER3     0x3

/*---------------------------------------------------------------------------------------------------------*/
/*  PWM Capture								                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define	PWM_CAP0     0x10
#define	PWM_CAP1	 0x11
#define	PWM_CAP2     0x12
#define	PWM_CAP3     0x13

/*---------------------------------------------------------------------------------------------------------*/
/*  PWM Capture Interrupt Enable/Disable Flag                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_CAP_RISING_int	1
#define PWM_CAP_FALLING_int	2
#define PWM_CAP_ALL_int		0

/*---------------------------------------------------------------------------------------------------------*/
/*  PWM Capture Interrupt Flag				                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_CAP_RISING_FLAG	6
#define PWM_CAP_FALLING_FLAG	7

/*---------------------------------------------------------------------------------------------------------*/
/*  PWM Clcok Selector						                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_CLOCK_DIV_1    1
#define PWM_CLOCK_DIV_2    2
#define PWM_CLOCK_DIV_4    4
#define PWM_CLOCK_DIV_8    8
#define PWM_CLOCK_DIV_16   16

/*---------------------------------------------------------------------------------------------------------*/
/*  PWM Mode Selector						                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_TOGGLE_MODE		TRUE
#define PWM_ONE_SHOT_MODE	FALSE

/*---------------------------------------------------------------------------------------------------------*/
/* Define PWM Time Data Struct                                                                             */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
	unsigned char	u8Mode;
	FLOAT	fFrequency;
	unsigned char	u8HighPulseRatio;
	char	bInverter;
	unsigned char	u8ClockSelector;
	unsigned char	u8PreScale;
	unsigned int	u32Duty;
 
}PWM_TIME_DATA_T;

/*---------------------------------------------------------------------------------------------------------*/
/* Define PWM Call back funvtion Data Struct                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef void (*PFN_PWM_CALLBACK)(void);

/*---------------------------------------------------------------------------------------------------------*/
/* Define PWM Call back function Data Struct                                                               */
/*---------------------------------------------------------------------------------------------------------*/
typedef struct
{
    PFN_PWM_CALLBACK pfnPWM0CallBack;    
    PFN_PWM_CALLBACK pfnCAP0CallBack;
   
    PFN_PWM_CALLBACK pfnPWM1CallBack;    
    PFN_PWM_CALLBACK pfnCAP1CallBack;
    
    PFN_PWM_CALLBACK pfnPWM2CallBack;    
    PFN_PWM_CALLBACK pfnCAP2CallBack;
    
    PFN_PWM_CALLBACK pfnPWM3CallBack;    
    PFN_PWM_CALLBACK pfnCAP3CallBack;        
   
}PWM_CALLBACK_T;


/*---------------------------------------------------------------------------------------------------------*/
/* Define PWM functions prototype                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
extern void	PWM_Open(void);
extern void	PWM_Close(void);
extern void	PWM_SetTimerCounter(unsigned char u8Timer, unsigned short u16Counter);
extern void	PWM_EnableInt(unsigned char	u8Timer, unsigned char u8Int);
extern void	PWM_ClearInt(unsigned char u8Timer);
extern void	PWM_DisableInt(unsigned char u8Timer, unsigned char u8Int);
extern void	PWM_ClearCaptureIntStatus(unsigned char	u8Capture, unsigned char u8IntType);
extern void	PWM_EnableDeadZone(unsigned char u8Timer, unsigned char u8Length, char bEnableDeadZone);
extern void	PWM_SetTimerIO(unsigned char u8Timer, char bEnable);
extern void	PWM_Enable(unsigned char u8Timer, char bEnable);
extern char	PWM_IsTimerEnabled(unsigned char u8Timer);
extern char	PWM_GetCaptureIntStatus(unsigned char u8Capture, unsigned char u8IntType);
extern char	PWM_GetIntFlag(unsigned char u8Timer);
extern unsigned short	PWM_GetRisingCounter(unsigned char u8Capture);
extern unsigned short	PWM_GetFallingCounter(unsigned char u8Capture);
extern unsigned int	PWM_GetTimerCounter(unsigned char u8Timer);
extern FLOAT	PWM_SetTimerClk(unsigned char u8Timer, PWM_TIME_DATA_T *sPt);
extern void PWM_InstallCallBack(unsigned char u8Timer,	PFN_PWM_CALLBACK pfncallback,	PFN_PWM_CALLBACK *pfnOldcallback);
#ifdef  __cplusplus
}
#endif

#endif

















