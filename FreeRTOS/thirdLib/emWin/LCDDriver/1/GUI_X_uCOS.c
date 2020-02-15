 /*
  2 *********************************************************************************************************
  3 *                                                uC/GUI
  4 *                        Universal graphic software for embedded applications
  5 *
  6 *                       (c) Copyright 2002, Micrium Inc., Weston, FL
  7 *                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
  8 *
  9 *              礐/GUI is protected by international copyright laws. Knowledge of the
10 *              source code may not be used to write a similar product. This file may
11 *              only be used in accordance with a license and should not be redistributed
12 *              in any way. We appreciate your understanding and fairness.
13 *
14 ---Author-Explanation
15 * 
16 * 1.00.00 020519 JJL    First release of uC/GUI to uC/OS-II interface
17 * 
18 *
19 * Known problems or limitations with current version
20 *
21 *    None.
22 *
23 *
24 * Open issues
25 *
26 *    None
27 *********************************************************************************************************
28 */
 //#include "ucos_ii.h"
 #include "GUI_Private.H"
 #include "stdio.H" 
 #include "wbtypes.h"
 /*
37 *********************************************************************************************************
38 *                                         GLOBAL VARIABLES
39 *********************************************************************************************************
40 */
 
// static  int        KeyPressed;
// static  char       KeyIsInited;
 
 static  OS_EVENT     *dispSem;
 static  OS_EVENT     *eventSem;
 //static  OS_SEM      keySem;
 
/*
50 *********************************************************************************************************
51 *                                        TIMING FUNCTIONS
52 *
53 * Notes: Some timing dependent routines of uC/GUI require a GetTime and delay funtion. 
54 *        Default time unit (tick), normally is 1 ms.
55 *********************************************************************************************************
56 */
 
 int GUI_X_GetTime(void)
 {
     
 return ((int)OSTimeGet());
 }
 
 void GUI_X_Delay(int period)
 {
    intU  ticks;


    ticks = (period * 1000) / OS_TICKS_PER_SEC;
    OSTimeDly(ticks);              
 }
 
 
/*
78 *********************************************************************************************************
79 *                                          GUI_X_ExecIdle()
80 *********************************************************************************************************
81 */
 /*WM空闲时调用*/
 void GUI_X_ExecIdle(void) 
 {
  
     OSTimeDly( 1 );   //改为1.
                 
 }
 
 
 
/*
94 *********************************************************************************************************
95 *                                    MULTITASKING intERFACE FUNCTIONS
96 *

97 * Note(1): 1) The following routines are required only if uC/GUI is used in a true multi task environment, 
98 *             which means you have more than one thread using the uC/GUI API.  In this case the #define 
99 *             GUI_OS 1   needs to be in GUIConf.h
100 *********************************************************************************************************
101 */
 
 void GUI_X_InitOS (void)//建立一个互斥型信号量
 { 
dispSem = OSSemCreate(1); 
eventSem  = OSSemCreate(1);   
 } 
 
 void GUI_X_Lock(void)
 { 
    charU  err;
    OSSemPend(dispSem, 0, &err);
 }
 
 
 void GUI_X_Unlock(void)
 { 
OSSemPost(dispSem);
 }
 
 
 U32 GUI_X_GetTaskId(void)
 { 
     return ((U32)(OSTCBCur->OSTCBPrio));
 }
 
 /*
148 *********************************************************************************************************
149 *                                        GUI_X_WaitEvent()
150 *                                        GUI_X_SignalEvent()
151 *********************************************************************************************************
152 */
//153 
//154 
 void GUI_X_WaitEvent(void)
 {
    charU  err;
    (void)OSMboxPend(eventSem, 0, &err);
 }
 
 
 void GUI_X_SignalEvent(void)
 {
	(void)OSMboxPost(eventSem,(void *)1);
 }
//176 
//177 /*
//178 *********************************************************************************************************
//179 *                                      KEYBOARD intERFACE FUNCTIONS
//180 *
//181 * Purpose: The keyboard routines are required only by some widgets.
//182 *          If widgets are not used, they may be eliminated.
//183 *
//184 * Note(s): If uC/OS-II is used, characters typed into the log window will be placed    in the keyboard buffer. 
//185 *          This is a neat feature which allows you to operate your target system without having to use or 
//186 *          even to have a keyboard connected to it. (useful for demos !)
//187 *********************************************************************************************************
//188 */
//189 
// static void CheckInit(void)
// {
//     if(KeyIsInited==DEF_FALSE)
//     {
//         KeyIsInited = DEF_TRUE;
//         GUI_X_Init();
//     }
// }
//198 
//199 
//200 /*被GUI_Init()调用,用来初始化一些GUI运行之前需要用的硬件,如键盘或者鼠标之类的.如果不需要的话,可以为空*/
 void GUI_X_Init(void)
 {
//     OS_ERR err;
//     
//     OSSemCreate(    (OS_SEM     *)&keySem,
//                     (CPU_CHAR   *)"keySem", 
//                     (OS_SEM_CTR )0, 
//                     (OS_ERR     *)&err   );

 }
////210 
////211 
// int GUI_X_GetKey(void)
// {
//     int r;
//     r = KeyPressed;
//     CheckInit();
//     KeyPressed = 0;
//    return (r);
// }
////220 
////221 
// int GUI_X_WaitKey(void)
// {
//     int  r;
//     OS_ERR err;
//     CPU_TS ts;
//     
//     CheckInit();
//     if(KeyPressed==0)
//     {
//         OSSemPend(  (OS_SEM     *)&keySem,      //等待信号量
//                     (OS_TICK    )0, 
//                     (OS_OPT     )OS_OPT_PEND_BLOCKING, 
//                     (CPU_TS     *)&ts, 
//                     (OS_ERR     *)&err);
//     }
//     r= KeyPressed;
//     KeyPressed = 0;
//     return (r);
// }
// 
// 
// void GUI_X_StoreKey(int k)
// {
//     OS_ERR err;
//          KeyPressed = k;
//     OSSemPost(  (OS_SEM     *)&keySem,      //释放信号量
//                 (OS_OPT     )OS_OPT_POST_1, 
//                 (OS_ERR     *)&err);
// }
//252 
 void GUI_X_Log(const char *s) 
 { 
     GUI_USE_PARA(s); 
 }
 
 void GUI_X_Warn(const char *s) 
 {
     GUI_USE_PARA(s); 
 }
 
 void GUI_X_ErrorOut(const char *s)
 { 
     GUI_USE_PARA(s); 
 }








