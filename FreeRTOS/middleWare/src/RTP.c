#include "RTP.h"
#include "GUI.h"
#include "W55fa93_adc.h"
#include "w55fa93_vpost.h"
#include "ctp.h"
#include "key.h"
Pen_Holder Pen_Point;
extern _m_tp_dev tp_dev;
void RTP_Init(void)
{
	adc_init();//���17M
	adc_open(ADC_TS_4WIRE, 480, 272);	
	sysprintf("ADC_OK\n\r");		
	// X.Y.У׼
	GUI_TOUCH_SetOrientation(GUI_MIRROR_X|GUI_MIRROR_Y);
	GUI_TOUCH_Calibrate (GUI_COORD_X,0,480,85,940);
	GUI_TOUCH_Calibrate (GUI_COORD_Y,0,272,875,153);
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

/*
void TP_Adj_Info_Show(unsigned short x0,unsigned short  y0,unsigned short  x1,unsigned short  y1,unsigned short  x2,unsigned short  y2,unsigned short  x3,unsigned short  y3,unsigned short  fac)
{	  
	POINT_COLOR=RED;
	LCD_ShowString(40,160,lcddev.width,lcddev.height,16,"x1:");
 	LCD_ShowString(40+80,160,lcddev.width,lcddev.height,16,"y1:");
 	LCD_ShowString(40,180,lcddev.width,lcddev.height,16,"x2:");
 	LCD_ShowString(40+80,180,lcddev.width,lcddev.height,16,"y2:");
	LCD_ShowString(40,200,lcddev.width,lcddev.height,16,"x3:");
 	LCD_ShowString(40+80,200,lcddev.width,lcddev.height,16,"y3:");
	LCD_ShowString(40,220,lcddev.width,lcddev.height,16,"x4:");
 	LCD_ShowString(40+80,220,lcddev.width,lcddev.height,16,"y4:");  
 	LCD_ShowString(40,240,lcddev.width,lcddev.height,16,"fac is:");     
	LCD_ShowNum(40+24,160,x0,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,160,y0,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,180,x1,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,180,y1,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,200,x2,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,200,y2,4,16);	//��ʾ��ֵ
	LCD_ShowNum(40+24,220,x3,4,16);		//��ʾ��ֵ
	LCD_ShowNum(40+24+80,220,y3,4,16);	//��ʾ��ֵ
 	LCD_ShowNum(40+56,240,fac,3,16); 	//��ʾ��ֵ,����ֵ������95~105��Χ֮��.

}

*/
//////////////////////////////////////////////////////////////////////////////////		  
//��LCD�����йصĺ���  
//��һ��������
//����У׼�õ�
//x,y:����
//color:��ɫ
void TP_Drow_Touch_Point(unsigned short x,unsigned short y,unsigned int color)
{
	LCD_DrawLine(x-12,y,x+13,y,color);//����
	LCD_DrawLine(x,y-12,x,y+13,color);//����
	//LCD_DrawPoint(x+1,y+1);
	//LCD_DrawPoint(x-1,y+1);
	//LCD_DrawPoint(x+1,y-1);
	//LCD_DrawPoint(x-1,y-1);
	Draw_Circle(x,y,6,color);//������Ȧ
}


//////////////////////////////////////////////////////////////////////////////////		  
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
unsigned char TP_Scan(unsigned char  tp)
{	
	unsigned short tt = 0;
	unsigned short x,y;
	static unsigned char touch_flag = 0;
	int ret ;
	
	if(inpw(REG_ADC_CON)&0x100000)
	{
		adc_normalread(5, &tt);
		ret = adc_read(ADC_BLOCK, &x,&y);
		if(tt && ret) //��TP ����
		{
		
			if(tp)
			{
				tp_dev.x[0] = x;
				tp_dev.y[0] = y;
				//adc_read(ADC_BLOCK, &tp_dev.x[0],&tp_dev.y[0]);//��ȡ��������
			}
			else if(ret)//��ȡ��Ļ����
			{
				tp_dev.x[0]=tp_dev.xfac*tp_dev.x[0]+tp_dev.xoff;//�����ת��Ϊ��Ļ����
				tp_dev.y[0]=tp_dev.yfac*tp_dev.y[0]+tp_dev.yoff;  
			} 
			if((tp_dev.sta&TP_PRES_DOWN)==0)//֮ǰû�б�����
			{		 
				tp_dev.sta=TP_PRES_DOWN|TP_CATH_PRES;//��������  
				tp_dev.x[4]=x;//��¼��һ�ΰ���ʱ������
				tp_dev.y[4]=y;  	   			 
			}
			touch_flag = 1;
			//sysprintf("press x:%d y:%d\r\n",x,y);
		}
		
	}else if(touch_flag == 1)
	{
		touch_flag = 0;
		if(tp_dev.sta&TP_PRES_DOWN)//֮ǰ�Ǳ����µ�
		{
			tp_dev.sta&=~(1<<7);//��ǰ����ɿ�	
		}else//֮ǰ��û�б�����
		{
			tp_dev.x[4]=0;
			tp_dev.y[4]=0;
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
		}	
		sysprintf("release x:%d y:%d\r\n",tp_dev.x[0],tp_dev.y[0]);			
	}	
	
	return tp_dev.sta&TP_PRES_DOWN;//���ص�ǰ�Ĵ���״̬
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

unsigned char * const TP_REMIND_MSG_TBL="Please use the stylus click the cross on the screen.The cross will always move until the screen adjustment is completed.";
 
void TPDelay(unsigned int nCount)
{
	unsigned volatile int i;
		
	for(;nCount!=0;nCount--)
		for(i=0;i<10;i++);
}


//������У׼����
//�õ��ĸ�У׼����
void TP_Adjust(void)
{								 
	unsigned short pos_temp[4][2];//���껺��ֵ
	unsigned char  cnt=0;	
	long d1,d2;
	unsigned int tem1,tem2;
	double fac; 
	int fac_temp;
	long x1,x2;
	unsigned short outtime=0;
	
	GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();
	GUI_DispStringHCenterAt(TP_REMIND_MSG_TBL, _LCD_WIDTH/2 ,  _LCD_HEIGHT/2);

 	cnt=0;				
	/*POINT_COLOR=BLUE;
	BACK_COLOR =WHITE;
	LCD_Clear(WHITE);//����   
	POINT_COLOR=LCD_GUI_RED;//��ɫ 
	LCD_Clear(WHITE);//���� 	   
	POINT_COLOR=BLACK;
	LCD_ShowString(40,40,160,100,16,(u8*)TP_REMIND_MSG_TBL);//��ʾ��ʾ��Ϣ
	*/
	TP_Drow_Touch_Point(20,20,LCD_GUI_RED);//����1 
	tp_dev.sta=0;//���������ź� 
	tp_dev.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	
	
	while(1)//�������10����û�а���,���Զ��˳�
	{
		TP_Scan(1);
		TPDelay(300);
		//tp_dev.scan(1);//ɨ����������
		if((tp_dev.sta&0xc0)==TP_CATH_PRES)//����������һ��(��ʱ�����ɿ���.)
		{	
			outtime=0;		
			tp_dev.sta&=~(1<<6);//��ǰ����Ѿ����������.
						   			   
			pos_temp[cnt][0]=tp_dev.x[0];
			pos_temp[cnt][1]=tp_dev.y[0];
			sysprintf("cnt:%d x:%d y:%d\r\n", cnt,tp_dev.x[0],tp_dev.y[0]);
			cnt++;	  
			switch(cnt)
			{			   
				case 1:						 
					TP_Drow_Touch_Point(20,20,LCD_GUI_WHITE);				//�����1 
					TP_Drow_Touch_Point(_LCD_WIDTH-20,20,LCD_GUI_RED);	//����2
					break;
				case 2:
 					TP_Drow_Touch_Point(_LCD_WIDTH-20,20,LCD_GUI_WHITE);	//�����2
					TP_Drow_Touch_Point(20,_LCD_HEIGHT-20,LCD_GUI_RED);	//����3
					break;
				case 3:
 					TP_Drow_Touch_Point(20,_LCD_HEIGHT-20,LCD_GUI_WHITE);			//�����3
 					TP_Drow_Touch_Point(_LCD_WIDTH-20,_LCD_HEIGHT-20,LCD_GUI_RED);	//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    //�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=tem1+tem2;//sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=tem1+tem2; //sqrt(tem1+tem2);//�õ�3,4�ľ���
					//fac=(float)d1/d2;
					//fac_temp = fac*100;
					x1 = d1;
					x2 = d2*100;
				//sysprintf("1x:%d y:%d 2x:%d y:%d 3x:%d y:%d 4x:%d y:%d:%d\r\n",);
				sysprintf("1 d1:%d d2:%d x1:%d x2:%d\r\n",d1,d2,x1,x2);
					if(((x1 - x2) > 95 || (x1 - x2) > 105) || \
						((x2 - x1) > 95 || (x2 - x1) > 105) ||d1==0||d2==0
					)
					//if(fac_temp<95||fac_temp>105||d1==0||d2==0)//���ϸ�
					{
						sysprintf("clear continue 0\r\n");
						cnt=0;
 				    	TP_Drow_Touch_Point(_LCD_WIDTH-20,_LCD_HEIGHT-20,LCD_GUI_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,LCD_GUI_RED);								//����1
 					//	TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
 						continue;
					}
					sysprintf("clear continue 0k\r\n");
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					fac_temp = fac*100;
					sysprintf("2fac %d d1:%d d2",fac_temp,d1,d2);
					if(fac_temp<95||fac_temp>105)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(_LCD_WIDTH-20,_LCD_HEIGHT-20,LCD_GUI_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,LCD_GUI_RED);								//����1
 						//TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					fac_temp = fac*100;
					sysprintf("3fac %d d1:%d d2",fac_temp,d1,d2);
					if(fac_temp<95||fac_temp>105)//���ϸ�
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(_LCD_WIDTH-20,_LCD_HEIGHT-20,LCD_GUI_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,LCD_GUI_RED);								//����1
 					//	TP_Adj_Info_Show(pos_temp[0][0],pos_temp[0][1],pos_temp[1][0],pos_temp[1][1],pos_temp[2][0],pos_temp[2][1],pos_temp[3][0],pos_temp[3][1],fac*100);//��ʾ����   
						continue;
					}//��ȷ��
					//������
					tp_dev.xfac=(float)(_LCD_WIDTH-40)/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					tp_dev.xoff=(_LCD_WIDTH-tp_dev.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					tp_dev.yfac=(float)(_LCD_HEIGHT-40)/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					tp_dev.yoff=(_LCD_HEIGHT-tp_dev.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
					if(abs(tp_dev.xfac)>2||abs(tp_dev.yfac)>2)//������Ԥ����෴��.
					{
						cnt=0;
 				    	TP_Drow_Touch_Point(_LCD_WIDTH-20,_LCD_HEIGHT-20,LCD_GUI_WHITE);	//�����4
   	 					TP_Drow_Touch_Point(20,20,LCD_GUI_RED);								//����1
						//LCD_ShowString(40,26,_LCD_WIDTH,lcddev.height,16,"TP Need readjust!");
						GUI_DispStringHCenterAt("TP Need readjust!", 100 ,  100);
						tp_dev.touchtype=!tp_dev.touchtype;//�޸Ĵ�������.
						if(tp_dev.touchtype)//X,Y��������Ļ�෴
						{
							;//CMD_RDX=0X90;
							//CMD_RDY=0XD0;	 
						}else				   //X,Y��������Ļ��ͬ
						{
							;//CMD_RDX=0XD0;
							//CMD_RDY=0X90;	 
						}			    
						continue;
					}		
				
/*					POINT_COLOR=BLUE;
					LCD_Clear(WHITE);//����
					LCD_ShowString(35,110,lcddev.width,lcddev.height,16,"Touch Screen Adjust OK!");//У�����
					GUI_DispStringHCenterAt("TP Need readjust!", _LCD_WIDTH/2 ,  _LCD_HEIGHT/4);
					delay_ms(1000);
					TP_Save_Adjdata();  
 					LCD_Clear(WHITE);//����   
*/
					GUI_Clear();
					GUI_DispStringHCenterAt("TP Need readjust!", _LCD_WIDTH/2 ,  _LCD_HEIGHT/4);
					GUI_Clear();
					return;//У�����				 
			}
		}
	/*	delay_ms(10);
		outtime++;
		if(outtime>1000)
		{
			TP_Get_Adjdata();
			break;
	 	} 
		*/
 	}
}	 




void startupCalibrateTP(void)
{
	unsigned char readValue = 0;
	unsigned char macroValue = LOCK_GPIO_BIT|IMPORT_GPIO_BIT;
	
	readValue = readGPIOKey_Value();
	sysprintf("readValue:%x macroValue:%x\r\n", readValue,macroValue);
	if(1)  //readValue == macroValue
	{
		BackLightAdj(50);
		TP_Adjust();
	}
}