#include <stdio.h>
#include <string.h>
#include "wb_include.h"
#include "w55fa93_i2c.h"
#include "DrvI2CH.h"
#include "ctp.h"

#define CLOSE 0
#define OPEN 1
#define WRITE_FIREMARE_CLOSE 0

unsigned char	CTP_Buf[50];
unsigned char	Point_num=0;
ST_TOUCH_POint	Point_info[5];
unsigned char	Gesture;
msg_t volatile msg_pool[MAX_MSG] = {0, 0, 0};
unsigned char err = 0;
char OpenMainFlag = FALSE;
extern unsigned int rx_tempeture;
//unsigned char	recbuff[8];
char SelfTestFlag = FALSE;
char MessageBoxFlag = CLOSE;
char COMMERRFlag = TRUE;
unsigned short errcnt = 0;
char readCtpEn;
unsigned char checkbuff[5];

extern hmi_t lmkm_data;
unsigned char const CTP_init_data[186]= {
								0x44,0x20,0x03,0xE0,0x01,0x0A,0x3D,0x00,0x01,0x0F,
								0x28,0x0A,0x64,0x50,0x03,0x05,0x00,0x00,0x00,0x00,
								0x00,0x00,0x0A,0x18,0x1A,0x1E,0x14,0x8C,0x28,0x0C,
								0x35,0x37,0xB5,0x06,0x00,0x00,0x00,0xD9,0x33,0x1D,
								0x00,0x01,0x00,0x0A,0x00,0x01,0xFF,0x7F,0x14,0x50,
								0x32,0x33,0x50,0x94,0xD5,0x02,0x07,0x00,0x00,0x04,
								0x80,0x35,0x00,0x76,0x3A,0x00,0x6D,0x3F,0x00,0x65,
								0x45,0x00,0x5D,0x4C,0x00,0x5D,0x00,0x00,0x00,0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,
								0x12,0x14,0x16,0x18,0xFF,0xFF,0x00,0x00,0x00,0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0F,
								0x10,0x12,0x13,0x14,0x16,0x18,0x1C,0x1D,0x1E,0x1F,
								0x20,0x21,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
								0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
								0x00,0x00,0x00,0x00,0x20,0x01
								};
unsigned char RdBuff[192];


void sendMsg(msg_t* msg)
{
	unsigned int i;
	for(i=0;i<MAX_MSG;i++){
		if(msg_pool[i].type==MSG_NULL){
			msg_pool[i].type = msg->type;
			msg_pool[i].data[0]=msg->data[0];
			msg_pool[i].data[1]=msg->data[1];
			//sysprintf("\n\r1: i = %d \n", i);
			break;
			}
		}
}

void ClearMsg(unsigned char msgtype)
{
	unsigned int i,j;

	for(i=0;i<MAX_MSG;i++){
		if(msg_pool[i].type==msgtype){
			msg_pool[i].type=MSG_NULL;
			for(j=i;j<MAX_MSG;j++){
				if(msg_pool[j+1].type==MSG_NULL)break;
				msg_pool[j].type=msg_pool[j+1].type;
				msg_pool[j].data[0]=msg_pool[j+1].data[0];
				msg_pool[j].data[1]=msg_pool[j+1].data[1];
				}
			}
	}
}

void ClearAllMsg(void)
{
	unsigned int i;

	for(i=0;i<MAX_MSG;i++){
		msg_pool[i].type=0;
		msg_pool[i].data[0]=0;
		msg_pool[i].data[1]=0;
	}
}

unsigned char	getmessage(msg_t* msg0)
{
	unsigned int i;
	
	if(msg_pool[0].type==MSG_NULL){
		return 0;
		}
	msg0->type=msg_pool[0].type;
	msg0->data[0]=msg_pool[0].data[0];
	msg0->data[1]=msg_pool[0].data[1];
	msg_pool[0].type=MSG_NULL;
	msg_pool[0].data[0]=0;
	msg_pool[0].data[1]=0;
	for(i = 1;i<MAX_MSG;i++){
		if(msg_pool[i].type == MSG_NULL) break;
			msg_pool[i-1].type=msg_pool[i].type;
			msg_pool[i-1].data[0]=msg_pool[i].data[0];
			msg_pool[i-1].data[1]=msg_pool[i].data[1];
			
			msg_pool[i].type=MSG_NULL;
			msg_pool[i].data[0]=0;
			msg_pool[i].data[1]=0;
		}
	return 1;
}

void CPT_Reset(void)
{
	unsigned char  u8DeviceID;

	u8DeviceID = 0xb8;
	i2cOpen();
	i2cIoctl(I2C_IOC_SET_DEV_ADDRESS, (u8DeviceID>>1), 0);  
	i2cIoctl(I2C_IOC_SET_SPEED, 50, 0);	

	i2cIoctl(I2C_IOC_SET_SUB_ADDRESS, 0xba, 1);	
	CTP_Buf[0]=0x03;
	i2cWrite(CTP_Buf, 1);
	sysDelay(1000);
	sysprintf("CTP_Buf:%d\n", CTP_Buf);
}



void CTP_Init(void)
{	
	//unsigned char  u8DeviceID, i;
	int rtval;
	//int ret = 0;
	//u8DeviceID = 0xBA;
	
	outpw(REG_GPBFUN, inpw(REG_GPBFUN) & 0xFFFFFF0F);
	gpio_setportdir(GPIO_PORTB,	BIT2|BIT3, BIT2|BIT3);
	gpio_setportpull(GPIO_PORTB, BIT2, BIT2);		/*Set GPIOB-3 to pull high 		*/	
	gpio_setportval(GPIO_PORTB, BIT2, 0);
	gpio_setportval(GPIO_PORTB, BIT3, 0);
	sysDelay(2000);
	gpio_setportval(GPIO_PORTB, BIT2, BIT2);
	sysDelay(5000);	

	i2cInit();
	rtval = i2cOpen();
	if(rtval < 0){
		sysprintf("fail open i2c interface!\n");
		return;
	}
	gpio_setportpull(GPIO_PORTB, BIT3, BIT3);		/*Set GPIOB-3 to pull high 		*/	
	gpio_setportdir(GPIO_PORTB, BIT3, 0x0000);		/*Correct	Set GPIOB-3 as input port*/			
#if 1
	gpio_setsrcgrp(GPIO_PORTB, BIT3, 0x03);		/*Group GPIOB-3 to EXT_IRQ3	*/
	gpio_setintmode(GPIO_PORTB, BIT3, BIT3, 0);	/*Rising/Falling				 	*/
	sysInstallISR(IRQ_LEVEL_7, (INT_SOURCE_E)(0x03+2), (void *)ctp_isr);
	sysSetInterruptType((INT_SOURCE_E)(0x03+2), POSITIVE_EDGE_TRIGGER);
	sysEnableInterrupt((INT_SOURCE_E)(0x03+2));
	sysSetLocalInterrupt(ENABLE_IRQ);	
	ClearAllMsg();
#endif
//#if WRITE_FIREMARE_CLOSE
//	memset(RdBuff, 0, sizeof(RdBuff));
//	i2cOpen();
//	i2cIoctl(I2C_IOC_SET_DEV_ADDRESS, (u8DeviceID>>1), 0);  
//	i2cIoctl(I2C_IOC_SET_SPEED, 100, 0);
//	i2cIoctl(I2C_IOC_SET_SUB_ADDRESS, 0x8047, 2);	
//	i2cWrite((unsigned char *)CTP_init_data, sizeof(CTP_init_data));
//	i2cOpen();
//	sysDelay(100);
//	i2cIoctl(I2C_IOC_SET_DEV_ADDRESS, (u8DeviceID>>1), 0);  
//	i2cIoctl(I2C_IOC_SET_SPEED, 100, 0);
//	i2cIoctl(I2C_IOC_SET_SUB_ADDRESS, 0x8047, 2);
//	i2cRead(RdBuff, sizeof(RdBuff));
//#endif	
}

int CPT_ReadPacket(void)
{
	unsigned char  u8DeviceID,data;
	unsigned int len;
//	unsigned short low_byte   = 0;
//	unsigned short high_byte  = 0;
	u8DeviceID = 0XBA;
	
	
	i2cOpen();
	i2cIoctl(I2C_IOC_SET_DEV_ADDRESS, (u8DeviceID>>1), 0);  
	i2cIoctl(I2C_IOC_SET_SPEED, 100, 0);	
	i2cIoctl(I2C_IOC_SET_SUB_ADDRESS, 0x814e, 2);
	len = i2cRead(CTP_Buf, 6);
	
	i2cIoctl(I2C_IOC_SET_DEV_ADDRESS, (u8DeviceID>>1), 0);  
	i2cIoctl(I2C_IOC_SET_SPEED, 100, 0);	
	i2cIoctl(I2C_IOC_SET_SUB_ADDRESS, 0x814e, 2);
	data = 0x00;
	i2cWrite(&data, 1);
//	if(len != 0 )
//	{
//		sysprintf("len:%d 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:\n",len,CTP_Buf[0], CTP_Buf[1],CTP_Buf[2],\
//									CTP_Buf[3], CTP_Buf[4],CTP_Buf[5]);
//	}
	return len;
}


unsigned char bt_parser_pixcir(void)
{
	unsigned short low_byte   = 0;
	unsigned short high_byte  = 0;
	unsigned char i          = 0;
	unsigned int temp;

	Point_num = CTP_Buf[0] & 0x07;
//	sysprintf("ctp_buf[0]:0x%x\n", CTP_Buf[0]);
	if(Point_num > CTPM_26BYTES_POintS_MAX )
		return CTPM_ERR_PROTOCOL;
	         
	/*remove the touch point information into pst_touch_info.*/
//	Point_num = 1;				//Point_num
	for(i = 0; i < Point_num; i++){   
		high_byte = CTP_Buf[3+8*i];
		high_byte <<= 8;
		low_byte = CTP_Buf[2+8*i];
		temp = (high_byte | low_byte)&0x3ff;
		Point_info[i].w_tp_x = 	temp;

		high_byte = CTP_Buf[5+8*i];
		high_byte <<= 8;
		low_byte = CTP_Buf[4+8*i];
		temp = (high_byte | low_byte)&0x1ff;
		Point_info[i].w_tp_y = temp;
		
		sysprintf("LOx = %d,y=%d\n",Point_info[i].w_tp_x,Point_info[i].w_tp_y );
	}
	return CTPM_NOERROR;
}


unsigned short pre_x = 0,pre_y = 0;
void ctp_isr(void)
{
	msg_t ctpmsg;
	int rctp;

	rctp=CPT_ReadPacket();
	if(rctp==6){
		if(bt_parser_pixcir()!=CTPM_NOERROR)
			return;
		}else{
		readCtpEn=FALSE;
		gpio_setintmode(GPIO_PORTB, (1 << 3), (1 << 3), 0);	/*Rising/Falling				 	*/
		gpio_cleartriggersrc(GPIO_PORTB);
		sysEnableInterrupt((INT_SOURCE_E)(0x03 + 2));
		return;
		}
	pre_x = Point_info[0].w_tp_x;
	pre_y = Point_info[0].w_tp_y;
	sysprintf("press 1s  x = %d,y=%d\n",pre_x,pre_y);
	gpio_cleartriggersrc(GPIO_PORTB);
	sysDisableInterrupt((INT_SOURCE_E)(0x03 + 2));
	gpio_setintmode(GPIO_PORTB, (1 << 3), 0, 0);	/*Rising/Falling*/
	readCtpEn=TRUE;
	ctpmsg.type = MSG_TP_PRESS;
	ctpmsg.data[0]=pre_x;
	ctpmsg.data[1]=pre_y;                                                   
	sendMsg(&ctpmsg);
}
