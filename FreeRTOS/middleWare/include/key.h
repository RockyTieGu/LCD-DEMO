#ifndef _KEY_H_
#define _KEY_H_

//按键枚举，注释为PCB上按键标识符
typedef enum aa
{	
	BUTTON_IDLE,	
	BUTTON_DOWN,
	BUTTON_OK,	
	BUTTON_UP,	
	BUTTON_LOCK,		
	BUTTON_RETURN		
}key_enum;


//空闲状态
//=>IMPORT
//=>LOCK
//=>FINAL
//按键ADC值，注释为计算该值公式
#define AD_KEYDOWN_VALUE	(253)		//1024*(330R + 1K)/1.33K 
#define AD_KEYUP_VALUE		(743)		//1024*(2.65K)/3.65K  
#define AD_KEYOK_VALUE		(940)		//1024*(7.05K)/8.05K  
#define AD_KEYLOCK_VALUE	(853)		//1024*(5.05K)/6.05K  
#define AD_KEYRETURN_VALUE	(547)		//1024*(5.05K)/6.05K  
#define MAX_AD_ERROR_VALUE   (40)		//允许最大的误差值

extern void ADCKEY_Init(void);
extern void ADCKEY_Handle(void);
extern key_enum GetKey_Value(void);
extern void ClearKey_Value(void);
extern void boardKey_Init(void);
extern key_enum boardKeyMSG_handle(void);
#endif
