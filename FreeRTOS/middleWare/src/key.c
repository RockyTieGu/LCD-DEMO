
#include "wb_include.h"
#include "W55FA93_adc.h"
#include "key.h"
#define AIN_AD_CHANNEL	3		//读取模拟量通道3
#define KEY_SCANTIME	200//(10)	//20ms
static unsigned int key_time;
key_enum cur_key_status = BUTTON_IDLE;
//GPD4 down
//GPD3	IMPORT
//GPA7	UP
//GPB0	OK
//GPB5	LOCK


void ADCKEY_Init()
{
	key_time = sysGetTicks(TIMER0);
}

key_enum KeyValue_handle(unsigned short keyValue)
{
	static unsigned char temp_key = 0;
	unsigned char key_status = BUTTON_IDLE;
	
	if(temp_key && (keyValue == 1023))	//按键释放
	{
		switch(temp_key)
		{
			case 0x01:	key_status = BUTTON_DOWN;	sysprintf("AD_KEYDOWN_VALUE 1\n");break;
			case 0x02:	key_status = BUTTON_UP;	break;
			case 0x03:	key_status = BUTTON_OK;	break;
			case 0x04:	key_status = BUTTON_LOCK;break;
			case 0x05:	key_status = BUTTON_RETURN;break;
			default:	key_status = BUTTON_IDLE;	break;
		}
		temp_key = 0;
		sysprintf("AD_KEYDOWN_VALUE 1.0\n");
	}else if((!temp_key) && (keyValue < (AD_KEYDOWN_VALUE + MAX_AD_ERROR_VALUE)) && \
		(keyValue > (AD_KEYDOWN_VALUE - MAX_AD_ERROR_VALUE))
		)//向下键按下
	{
		temp_key = 0x01;
		sysprintf("AD_KEYDOWN_VALUE 0\n");
	}else if((!temp_key) && (keyValue < (AD_KEYUP_VALUE + MAX_AD_ERROR_VALUE)) && \
		(keyValue > (AD_KEYUP_VALUE - MAX_AD_ERROR_VALUE))
		)//向上键按下
	{
		temp_key = 0x02;
	}else if((!temp_key) && (keyValue < (AD_KEYOK_VALUE + MAX_AD_ERROR_VALUE)) && \
		(keyValue > (AD_KEYOK_VALUE - MAX_AD_ERROR_VALUE))
		)//确认键按下
	{
		temp_key = 0x03;
	}else if((!temp_key) && (keyValue < (AD_KEYLOCK_VALUE + MAX_AD_ERROR_VALUE)) && \
		(keyValue > (AD_KEYLOCK_VALUE - MAX_AD_ERROR_VALUE))
		)//锁存键按下
	{
		temp_key = 0x04;
	}else if((!temp_key) && (keyValue < (AD_KEYRETURN_VALUE + MAX_AD_ERROR_VALUE)) && \
		(keyValue > (AD_KEYRETURN_VALUE - MAX_AD_ERROR_VALUE))
		)//返回键按下
	{
		temp_key = 0x05;
	}
	return (key_enum)key_status;
}

void ADCKEY_Handle(void)
{
	unsigned short key_value;
	
	if(sysGetTicks(TIMER0) > (key_time + KEY_SCANTIME))
	{
		adc_normalread(AIN_AD_CHANNEL, &key_value);
		cur_key_status = KeyValue_handle(key_value);
		key_time = sysGetTicks(TIMER0);
	}else if(sysGetTicks(TIMER0) < key_time)
	{
		key_time = sysGetTicks(TIMER0);
	}
	//sysGetTicks(TIMER0);
}

key_enum GetKey_Value(void)
{
	return cur_key_status;
}

void ClearKey_Value(void)
{
	cur_key_status = 0;
	return ;
}

void boardKey_Init(void)
{
	outpw(REG_GPDFUN, inpw(REG_GPDFUN) & 0xFFFFFC3F);							//set GPIOD-3 GPIOD-4 为GPIO口
	gpio_setportdir(DOWN_GPIO_PORT,	DOWN_GPIO_BIT|IMPORT_GPIO_BIT, 0);			//set INPUT
	gpio_setportpull(DOWN_GPIO_PORT, DOWN_GPIO_BIT|IMPORT_GPIO_BIT, DOWN_GPIO_BIT|IMPORT_GPIO_BIT);		//Set GPIOD-3 GPIOD-4 to pull high
	gpio_setportval(DOWN_GPIO_PORT, DOWN_GPIO_BIT|IMPORT_GPIO_BIT, 0);			//set GPIOD-3 GPIOD-4  0
	
	outpw(REG_GPBFUN, inpw(REG_GPBFUN) & 0xFFFFF3FC);
	gpio_setportdir(OK_GPIO_PORT,	OK_GPIO_BIT|LOCK_GPIO_BIT, 0);
	gpio_setportpull(OK_GPIO_PORT,OK_GPIO_BIT|LOCK_GPIO_BIT, OK_GPIO_BIT|LOCK_GPIO_BIT);
	gpio_setportval(OK_GPIO_PORT, OK_GPIO_BIT|LOCK_GPIO_BIT, 0);	
	
	outpw(REG_GPAFUN, inpw(REG_GPAFUN) & 0xFFFF3FFF);
	gpio_setportdir(UP_GPIO_PORT,	UP_GPIO_BIT, 0);
	gpio_setportpull(UP_GPIO_PORT, UP_GPIO_BIT, UP_GPIO_BIT);
	gpio_setportval(UP_GPIO_PORT, UP_GPIO_PORT, 0);
	
}

unsigned char readGPIOKey_Value(void)
{	
	static unsigned short keyValue_DOWN_IMPORT = 0;
	static unsigned short keyValue_OK_LOCK = 0;
	static unsigned short keyValue_UP = 0;
	volatile unsigned char totalKeyValue = 0;
	
	gpio_readport(GPIO_PORTD, &keyValue_DOWN_IMPORT);
	gpio_readport(GPIO_PORTB, &keyValue_OK_LOCK);
	gpio_readport(GPIO_PORTA, &keyValue_UP);
	
	if((keyValue_DOWN_IMPORT&DOWN_GPIO_BIT) == 0)
		totalKeyValue |= DOWN_GPIO_BIT; 
	if((keyValue_DOWN_IMPORT&IMPORT_GPIO_BIT) == 0)
		totalKeyValue |= IMPORT_GPIO_BIT; 
	if((keyValue_OK_LOCK&OK_GPIO_BIT) == 0)
		totalKeyValue |= OK_GPIO_BIT; 
	if((keyValue_OK_LOCK&LOCK_GPIO_BIT) == 0)
		totalKeyValue |= LOCK_GPIO_BIT; 
	if((keyValue_UP&UP_GPIO_BIT) == 0)
		totalKeyValue |= UP_GPIO_BIT; 
	
	return totalKeyValue;
}


key_enum boardKeyMSG_handle(void)
{
	static key_enum temp_keyStatus = BUTTON_IDLE;
	volatile unsigned char keyValue;
	key_enum key_status = BUTTON_IDLE;
	
	
	keyValue = readGPIOKey_Value();
	//printf("keyValue:%x\r\n", keyValue);
	if(temp_keyStatus  && (keyValue == 0))	//key release
	{
		switch(temp_keyStatus)
		{
			case 0x01:	key_status = BUTTON_DOWN;	break;
			case 0x02:	key_status = BUTTON_UP;	break;
			case 0x03:	key_status = BUTTON_OK;	break;
			case 0x04:	key_status = BUTTON_LOCK;	break;
			case 0x05:	key_status = BUTTON_RETURN;break;
			default:	key_status = BUTTON_IDLE;	break;
		}
		temp_keyStatus = BUTTON_IDLE;
	//	sysprintf("buttons reslease!\r\n");
	}else if((temp_keyStatus == BUTTON_IDLE) && (keyValue == DOWN_GPIO_BIT))//press down key
	{//	sysprintf("press down key\r\n");
		temp_keyStatus = BUTTON_DOWN;
	}
	else if((temp_keyStatus == BUTTON_IDLE) && (keyValue == UP_GPIO_BIT))//press up key
	{	
	//	sysprintf("press up key\r\n");
		temp_keyStatus = BUTTON_UP;
	}
	else if((temp_keyStatus == BUTTON_IDLE) && (keyValue == OK_GPIO_BIT))//press ok key
	{
	//	sysprintf("press ok key\r\n");
		temp_keyStatus = BUTTON_OK;
	}
	else if((temp_keyStatus == BUTTON_IDLE) && (keyValue == LOCK_GPIO_BIT))//press lock key
	{
	//	sysprintf("press lock key\r\n");
		temp_keyStatus = BUTTON_LOCK;
	}
	else if((temp_keyStatus == BUTTON_IDLE) && (keyValue == IMPORT_GPIO_BIT))//press import key 
	{
	//	sysprintf("press lock key\r\n");
		temp_keyStatus = BUTTON_RETURN;
	}
	return key_status;
}

