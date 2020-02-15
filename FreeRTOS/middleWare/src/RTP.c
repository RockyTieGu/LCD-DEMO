#include "RTP.h"
//#include "GUI.h"
#include "W55fa93_adc.h"
#include "w55fa93_vpost.h"

Pen_Holder Pen_Point;

void RTP_Init(void)
{
	adc_init();//最大17M
	adc_open(ADC_TS_4WIRE, 480, 272);	
//	sysprintf("ADC_OK\n\r");		
//	// X.Y.校准
//	GUI_TOUCH_SetOrientation(GUI_MIRROR_X|GUI_MIRROR_Y);
//	GUI_TOUCH_Calibrate (GUI_COORD_X,0,480,44,955);
//	GUI_TOUCH_Calibrate (GUI_COORD_Y,0,272,80,882);
}

char CheckRTPStatus(void)
{
	if( inpw(REG_ADC_CON)&0x100000)
		return 1;
	else
		return 0;
}

unsigned short Read_RTP_X(void)
{
	unsigned short value;
	
	adc_normalread(5, &value);
	
	return value;
}

unsigned short coord[4][2];

INT GetPoint(UINT16 *x, UINT16* y)
{
	//UINT32 tx,ty;
#if 1
	coord[0][0] = 65;
	coord[0][1] = 867;
	coord[1][0] = 959;
	coord[1][1] = 109;
	*x = (INT16)(10 + (_LCD_WIDTH - 10)*(Pen_Point.X - coord[0][0])/(coord[1][0] - coord[0][0]));
	*y = (INT16)(10 + (_LCD_HEIGHT - 10)*(Pen_Point.Y - coord[0][1])/(coord[1][1] - coord[0][1]));
#else
//460 / 815~~ 0.564; 
//630 / 859~~0.733;
	*x = Pen_Point.X *460 / 815 - 42;
	*y = 703 - Pen_Point.Y *630/859;
#endif
	return 0;
}
#if 0
void CTPCalibrate()
{
	unsigned short x,y, temp;
	unsigned short n = 0, i;
	unsigned char flag = 0;
	
	LCD_Clear(GUI_BLUE, 0);

	for(i = 0; i < _LCD_WIDTH; i+= 10)
		 LCD_DrawLine(i, 0, i, _LCD_HEIGHT -1,GUI_WHITE);
		//Vpost_DrawVLine(i, 0, _LCD_HEIGHT -2, WHITE, BUFF0);
	
	for(i = 0; i < _LCD_HEIGHT; i+= 10)
			LCD_DrawLine(0, i, _LCD_WIDTH, i,GUI_WHITE);

	Fill_Circle(10, 10, 5, GUI_BLACK);
//	Fill_Circle(_LCD_WIDTH - 10, 10, 5, GUI_BLACK);
//	Fill_Circle(10,272-10, 5, GUI_BLACK);
	Fill_Circle(470,272-10, 5, GUI_BLACK);
	LCDQuickUpdateScreen(0);
//	StartTouchFlag = 0;
	while(n <2){
			adc_normalread(5, &temp);
			if(temp && adc_read(1, &x, &y)){
				sysprintf("x = %d, y = %d\n", x, y);				
				coord[n][0] = x;
				coord[n][1] = y;
				flag = 1;
			}else if((flag == 1)){
				flag = 0;
				if (n == 0){
					Fill_Circle(10, 10, 5, GUI_WHITE);
					LCDQuickUpdateScreen(0);	
				}else if(n == 1){
					Fill_Circle(470,272-10, 5, GUI_WHITE);
					LCDQuickUpdateScreen(0);
				}
				n++;
				sysprintf("n:%d\n", n);
			}
	}
//	StartTouchFlag = 1;


		for(i = 0; i < 2; i++)
			sysprintf("read draw point:coord[%d][0]:%d coord[%d][1]:%d \n", i, coord[i][0], i,  coord[i][1]);
		
}
#endif
