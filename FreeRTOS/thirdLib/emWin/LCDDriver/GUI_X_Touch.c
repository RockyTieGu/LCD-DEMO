#include "w55fa93_adc.h"
#include "ctp.h"
#include "rtp.h"
#include "GUI.h"

unsigned short x,y,c;	
extern _m_tp_dev tp_dev;


void GUI_TOUCH_X_ActivateX(void) {
}

void GUI_TOUCH_X_ActivateY(void) {
}

static  char touch_flag = 0;
int  GUI_TOUCH_X_MeasureX(void) {
	
	//sysprintf("GUI_TOUCH_X_MeasureX \r\n");
#if 0
//	c=adc_read(ADC_BLOCK, &x, &y);
	
	if(0)//	if(tp_dev.scan(0))
	{
		sysprintf("x:%d ",tp_dev.x[0]);
		touch_flag = 1;
		return tp_dev.x[0];
		
	}
	else 
		return -100;
#else
	unsigned short tt;
	c = 0;
	//TP_Scan(1);

	if( inpw(REG_ADC_CON)&0x100000)	
	{
		adc_normalread(5, &tt);
		if(tt)
			c=adc_read(ADC_BLOCK, &x, &y);
	}

  if(c)
	{
		sysprintf("x:%d ", x);
		return x;
		
	}
	else 
		return -100;	
#endif
}

int  GUI_TOUCH_X_MeasureY(void) 
{
#if 0
	unsigned short y1;
	y1=y;y=0;x=0;
	
	//sysprintf("GUI_TOUCH_X_MeasureY \r\n");
	if(touch_flag == 1)
	{	
		touch_flag = 0;
		sysprintf("y:%d \r\n",tp_dev.y[0]);
		return tp_dev.y[0];
	}
	else 
		return -100;
#else
		unsigned short y1;
  y1=y;y=0;x=0;
			
	if(c)
	{
		c=0;
		sysprintf("y:%d \r\n", y1);
		return y1;
	}
	else 
		return -100;

#endif
}





//GUI_PID_STATE State;
void TOUCH_Scan(void)	
{
	//CPT_ReadPacket();	
/*	
	c=adc_read(ADC_BLOCK, &x, &y);
	if(c==1)
	{
		      //OSSchedLock();//
					// 触摸按下事件 
					State.x =480-(x-44)/2;
					State.y = 62;//272-(y-80)/3;
					State.Pressed = 1;
					GUI_PID_StoreState(&State);
		     // OSSchedUnlock();
		
	}else   //触摸释放事件
	{
					State.Pressed = 0;
					GUI_PID_StoreState(&State);	
	}
*/			
}
