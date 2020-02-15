/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUITime.C
Purpose     : Time related routines
---------------------------END-OF-HEADER------------------------------
*/

#include <stddef.h>           /* needed for definition of NULL */
#include "GUI_Private.h"
#include <GUI.H>
#include "wb_timer.h"
/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_GetTime
*/
int GUI_GetTime(void) {
  return GUI_X_GetTime();
}

/*********************************************************************
*
*       GUI_Delay
*/
void GUI_Delay(int Period) {
	int i =0;
#if 0
 int EndTime = GUI_GetTime()+Period;
  int tRem; /* remaining Time */
//  GUI_ASSERT_NO_LOCK();
  while (tRem = EndTime- GUI_GetTime(), tRem>0) {
    GUI_Exec();
    GUI_X_Delay((tRem >5) ? 5 : tRem);
  }
#else
  GUI_ASSERT_NO_LOCK(); 
//  sysprintf("dddd\n");
  while(Period--) 
  { 
	GUI_Exec(); 
//    sysprintf("11111\n");
	//sys5msDelay(1); 
//	for(i = 0; i < 10000; i++);
//	for(i = 0; i < 10000; i++);
//	for(i = 0; i < 10000; i++);
//	for(i = 0; i < 10000; i++);
//	for(i = 0; i < 10000; i++);
  }
//  sysprintf("ffff\n");  
#endif
}

/*************************** End of file ****************************/
