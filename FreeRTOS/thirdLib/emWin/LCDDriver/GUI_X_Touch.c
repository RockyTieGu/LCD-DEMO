
#include "w55fa93_adc.h"
#include "GUI.h"

unsigned short x,y,c;	

void GUI_TOUCH_X_ActivateX(void) {
}

void GUI_TOUCH_X_ActivateY(void) {
}


int  GUI_TOUCH_X_MeasureX(void) {
	
	c=adc_read(ADC_BLOCK, &x, &y);
	if(c)
	{
		return x;
		
	}
	else 
		return -100;
}

int  GUI_TOUCH_X_MeasureY(void) {
	unsigned short y1;
	y1=y;y=0;x=0;
			
	if(c)
	{
		c=0;
		return y1;
	}
	else 
		return -100;
}





//GUI_PID_STATE State;
//void TOUCH_Scan(void)	
//{
//	
//c=adc_read(ADC_BLOCK, &x, &y);
//	if(c==1)
//	{
//		      //OSSchedLock();//
//					/* 触摸按下事件 */
//					State.x =480-(x-44)/2;
//					State.y = 62;//272-(y-80)/3;
//					State.Pressed = 1;
//					GUI_PID_StoreState(&State);
//		     // OSSchedUnlock();
//		
//	}else   /* 触摸释放事件 */
//	{
//					State.Pressed = 0;
//					GUI_PID_StoreState(&State);	
//	}
//			
//}
