/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2010; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                         uC/OS-II example code
*                                  Application configuration header file
*
*                                   Energy Micro EFM32 (EFM32G890F128)
*                                              with the
*                            Energy Micro EFM32G890F128-DK Development Board
*
* Filename      : app_cfg.h
* Version       : V0.2
* Programmer(s) : Energy Micro AS
* (C) Copyright 2010 Energy Micro AS, http://www.energymicro.com
*
* (This source code is the property of Energy Micro AS.) The source and compiled code may only be used on 
* Energy Micro "EFM32" microcontrollers.
*
* This copyright notice may not be removed from the source code nor changed.
*
* DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Energy Micro AS has no obligation to support this 
* Software. Energy Micro AS is providing the Software "AS IS", with no express or implied warranties of 
* any kind, including, but not limited to, any implied warranties of merchantability or fitness for any
* particular purpose or warranties against infringement of any proprietary rights of a third party.
*
* Energy Micro AS will not be liable for any consequential, incidental, or special damages, or any other
* relief, or for any claim by any third party, arising from your use of this Software.
*
*********************************************************************************************************
*/
#ifndef  __APP_CFG_H
#define  __APP_CFG_H


/*
*********************************************************************************************************
*                                         TASKS PRIORITIES
*********************************************************************************************************
*/
#define  OS_TASK_TMR_PRIO       (OS_LOWEST_PRIO-2U)

#define  APP_CFG_TASK_START_PRIO                20U /* lowest priority task */
#define  APP_CFG_TASK_ONE_PRIO                   6U
#define  APP_CFG_TASK_TWO_PRIO                   4U /* highest priority task */
#define  APP_CFG_TASK_THREE_PRIO                 5U


/*
*********************************************************************************************************
*                                         TASK STACK SIZES
*                          Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_STK_SIZE           128U
#define  APP_CFG_TASK_ONE_STK_SIZE             128U
#define  APP_CFG_TASK_TWO_STK_SIZE             128U
#define  APP_CFG_TASK_THREE_STK_SIZE           128U


#endif /* end of __APP_CFG_MODULE_H */
