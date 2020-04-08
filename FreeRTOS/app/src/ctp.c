#include <stdio.h>
#include <string.h>
#include "wb_include.h"
#include "w55fa93_i2c.h"
#include "DrvI2CH.h"
#include "ctp.h"
#include "dx_lcdconfig.h"

#define CLOSE 0
#define OPEN 1
#define WRITE_FIREMARE_CLOSE 1

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
unsigned char  GT911_Scan(unsigned char mode);
unsigned char CTP_Init(void);
unsigned char CPT_ReadPacket(void);
_m_tp_dev tp_dev=
{
	CTP_Init,
	CPT_ReadPacket,
	0,
	0,
	0, 
	0,
	0,
	0,
	0,	  	 		
	0,
	0,	  	 		
};	


void CTP_Delay(unsigned int nCount)
{
	unsigned volatile int i;
		
	for(;nCount!=0;nCount--)
		for(i=0;i<10;i++);
}
void sendMsg(msg_t* msg)
{
	unsigned int i;
	for(i=0;i<MAX_MSG;i++)
	{
		if(msg_pool[i].type==MSG_NULL)
		{
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
	CTP_Delay(600000);
	CTP_Delay(600000);
	sysprintf("CTP_Buf:%d\n", CTP_Buf);
}

//从GT911读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度			  
void GT911_RD_Reg(unsigned short  reg,unsigned char *buf,unsigned char len)
{
	unsigned char real_len; 
	
	i2cOpen();
	i2cIoctl(I2C_IOC_SET_DEV_ADDRESS, (GT_CMD_WR>>1), 0);  
	i2cIoctl(I2C_IOC_SET_SPEED, 100, 0);	
	i2cIoctl(I2C_IOC_SET_SUB_ADDRESS, reg, 2);
	real_len = i2cRead(buf, len);
	
	if(len != real_len)
	{
		sysprintf("GT9147_RD_Reg 0x%x address read %d bytes fail!\r\n",reg, len);
	}
} 

//向GT911写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
unsigned char  GT911_WR_Reg(unsigned short reg,unsigned char *buf,unsigned char len)
{
	unsigned char  i;
	unsigned char  ret=0;
	
	i2cOpen();
	i2cIoctl(I2C_IOC_SET_DEV_ADDRESS, (GT_CMD_WR>>1), 0);  
	i2cIoctl(I2C_IOC_SET_SPEED, 100, 0);	
	i2cIoctl(I2C_IOC_SET_SUB_ADDRESS, reg, 2);
	if (i2cWrite(buf, len) <= 0)
	{
		sysprintf("GT911_WR_Reg 0x%x address write %d bytes fail!\r\n", reg, len);
	}
	return ret; 
}

/*
	PB2 RST
	PB3 INT
outpw(REG_GPBFUN, inpw(REG_GPBFUN) & 0xFFFFFF0F);
	gpio_setportdir(GPIO_PORTB,	BIT2|BIT3, BIT2|BIT3);
	gpio_setportpull(GPIO_PORTB, BIT2, BIT2);		//Set GPIOB-3 to pull high
	gpio_setportval(GPIO_PORTB, BIT2, 0);
	gpio_setportval(GPIO_PORTB, BIT3, 0);
	CTP_Delay(600000);
	CTP_Delay(600000);
	gpio_setportval(GPIO_PORTB, BIT2, BIT2);
	CTP_Delay(600000);

	i2cInit();
	rtval = i2cOpen();
	if(rtval < 0){
		sysprintf("fail open i2c interface!\n");
		return;
	}
	gpio_setportpull(GPIO_PORTB, BIT3, BIT3);		//Set GPIOB-3 to pull high	
	gpio_setportdir(GPIO_PORTB, BIT3, 0x0000);		//Correct	Set GPIOB-3 as input port
*/

unsigned char CTP_Init(void)
{	
	int rtval;
	unsigned char touchChip_id[10];
	unsigned char u8DeviceID = 0xBA;
	outpw(REG_GPBFUN, inpw(REG_GPBFUN) & 0xFFFFFF0F);
	gpio_setportdir(GPIO_PORTB,	BIT2|BIT3, BIT2|BIT3);
	gpio_setportpull(GPIO_PORTB, BIT2, BIT2);		/*Set GPIOB-3 to pull high 		*/	

	i2cInit();
	rtval = i2cOpen();
	if(rtval < 0){
		sysprintf("fail open i2c interface!\n");
		return 0xff;
	}
	gpio_setportval(GPIO_PORTB, BIT2, 0);
	//gpio_setportval(GPIO_PORTB, BIT3, 0);
	CTP_Delay(600000);
	//CTP_Delay(600000);
	gpio_setportval(GPIO_PORTB, BIT2, BIT2);
	CTP_Delay(600000);
//	CTP_Delay(600000);
	gpio_setportpull(GPIO_PORTB, BIT3, BIT3);		/*Set GPIOB-3 to pull high 		*/	
	gpio_setportdir(GPIO_PORTB, BIT3, 0x00);		/*Correct	Set GPIOB-3 as input port*/	
	CTP_Delay(600000);
	//CTP_Delay(600000);
	
	GT911_RD_Reg(0X8140, touchChip_id, 4);
	sysprintf("red touch chip id:%s\r\n",touchChip_id);
	
#if WRITE_FIREMARE_CLOSE
	memset(RdBuff, 0, sizeof(RdBuff));
	i2cOpen();
	i2cIoctl(I2C_IOC_SET_DEV_ADDRESS, (u8DeviceID>>1), 0);  
	i2cIoctl(I2C_IOC_SET_SPEED, 100, 0);
	i2cIoctl(I2C_IOC_SET_SUB_ADDRESS, 0x8047, 2);	
	i2cWrite((unsigned char *)CTP_init_data, sizeof(CTP_init_data));
	i2cOpen();
	sysDelay(100);
	i2cIoctl(I2C_IOC_SET_DEV_ADDRESS, (u8DeviceID>>1), 0);  
	i2cIoctl(I2C_IOC_SET_SPEED, 100, 0);
	i2cIoctl(I2C_IOC_SET_SUB_ADDRESS, 0x8047, 2);
	i2cRead(RdBuff, sizeof(RdBuff));
#endif
	sysprintf("CTP_Init finished!\n");
		
	return 0;
}

unsigned char CPT_ReadPacket(void)
{
	unsigned char  u8DeviceID,data = 0;
	unsigned int len;
	volatile unsigned short value = 0;
//	unsigned short low_byte   = 0;
//	unsigned short high_byte  = 0;
		u8DeviceID = 0XBA;
	
	
	
#if 0
	GT911_RD_Reg(GT_GSTID_REG, CTP_Buf, 6);
	sysDelay(10);
	GT911_WR_Reg(GT_GSTID_REG, &data, 1);
#else
	gpio_readport(GPIO_PORTB, &value);
//	gpio_setportval(GPIO_PORTB, BIT2, BIT2);
	//sysprintf("value:0x%x\r\n",value);
	if((value&BIT3) == 0)
	{sysprintf("value:0x%x\r\n",value);
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
	}
#endif
	if(CTP_Buf[0] &0x80)
	{
		tp_dev.x[0]=((short)CTP_Buf[3]<<8)+CTP_Buf[2];
		tp_dev.y[0]=((short)CTP_Buf[5]<<8)+CTP_Buf[4];
		return 1;
	}
	else
	{
		tp_dev.x[0]=tp_dev.y[0]=0;
		return 0;
	}
//	sysprintf("0x%x:0x%x:0x%x:0x%x:0x%x:0x%x:\n",CTP_Buf[0], CTP_Buf[1],CTP_Buf[2],\
//									CTP_Buf[3], CTP_Buf[4],CTP_Buf[5]);
	
}

const unsigned short GT911_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};

//扫描触摸屏(采用查询方式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
unsigned char  GT911_Scan(unsigned char mode1)
{
	unsigned char buf[4];
	unsigned char i=0;
	unsigned char res=0;
	unsigned char temp;
	unsigned char tempsta;
	unsigned char mode;
 	static unsigned char t=0;//控制查询间隔,从而降低CPU占用率   
	#if 1
	t++;
	if(1)//if((t%10)==0||t<10)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
	{				
		GT911_RD_Reg(GT_GSTID_REG,&mode,1);	//读取触摸点的状态ACK
		//sysprintf("read mode:%d\r\n",mode);
 		if(mode&0X80&&((mode&0XF)<6))
		{
			temp=0;
			GT911_WR_Reg(GT_GSTID_REG,&temp,1);//清标志 		
		}		
		if((mode&0XF)&&((mode&0XF)<6))
		{
			temp=0XFF<<(mode&0XF);		//将点的个数转换为1的位数,匹配tp_dev.sta定义 
			tempsta=tp_dev.sta;			//保存当前的tp_dev.sta值
			tp_dev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES; 
			tp_dev.x[4]=tp_dev.x[0];	//保存触点0的数据
			tp_dev.y[4]=tp_dev.y[0];
			for(i=0;i<5;i++)
			{
				if(tp_dev.sta&(1<<i))	//触摸有效?
				{
					GT911_RD_Reg(GT911_TPX_TBL[i],buf,4);	//读取XY坐标值
					if(tp_dev.touchtype&0X01)//横屏
					{
						tp_dev.y[i]=((short)buf[1]<<8)+buf[0];
						tp_dev.x[i]=_LCD_WIDTH-(((short)buf[3]<<8)+buf[2]);
					}else
					{
						tp_dev.x[i]=((short)buf[1]<<8)+buf[0];
						tp_dev.y[i]=((short)buf[3]<<8)+buf[2];
					}  
					sysprintf("x[%d]:%d,y[%d]:%d\r\n",i,tp_dev.x[i],i,tp_dev.y[i]);
				}			
			} 
			res=1;
			if(tp_dev.x[0]>_LCD_WIDTH || tp_dev.y[0]>_LCD_HEIGHT)//非法数据(坐标超出了)
			{ 
				if((mode&0XF)>1)		//有其他点有数据,则复第二个触点的数据到第一个触点.
				{
					tp_dev.x[0]=tp_dev.x[1];
					tp_dev.y[0]=tp_dev.y[1];
					t=0;				//触发一次,则会最少连续监测10次,从而提高命中率
				}else					//非法数据,则忽略此次数据(还原原来的)  
				{
					tp_dev.x[0]=tp_dev.x[4];
					tp_dev.y[0]=tp_dev.y[4];
					mode=0X80;		
					tp_dev.sta=tempsta;	//恢复tp_dev.sta
				}
			}else t=0;					//触发一次,则会最少连续监测10次,从而提高命中率
		}
	}
	if((mode&0X8F)==0X80)//无触摸点按下
	{ 
		if(tp_dev.sta&TP_PRES_DOWN)	//之前是被按下的
		{
			tp_dev.sta&=~(1<<7);	//标记按键松开
		}else						//之前就没有被按下
		{ 
			tp_dev.x[0]=0xffff;
			tp_dev.y[0]=0xffff;
			tp_dev.sta&=0XE0;	//清除点有效标记	
		}	 
	} 	
	if(t>240)t=10;//重新从10开始计数
	
	#endif
	return res;
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
		memset(CTP_Buf,0 ,sizeof(CTP_Buf));
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
