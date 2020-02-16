#include "task_lcd_test_app.h"
#include "task_play_music_app.h"
#include "task_play_video_app.h"
#include "task_main_menu_app.h"
#include "task_key_handle.h"
#include "task_comm_type.h"
#include "ctp.h"

//������
static void start_task(void *pvParameters);

//������Ϣ���е�����
#define KEYMSG_Q_NUM    (4)  							//������Ϣ���е�����  
volatile QueueHandle_t MenuKey_Queue;				//��ҳ������Ϣ���о��
volatile QueueHandle_t LcdTestMenu_Key_Queue;		//LCD����ҳ������Ϣ���о��
volatile QueueHandle_t PlayVideoMenu_Key_Queue;	//��Ƶ����ҳ������Ϣ���о��
volatile QueueHandle_t PlayMusicMenu_Key_Queue;	//��������ҳ������Ϣ���о��
volatile QueueHandle_t DisplayPhoto_Key_Queue;		//��ʾͼƬ������Ϣ���о��
volatile QueueHandle_t DemoMenu_Key_Queue;			//EMWINEҳ������Ϣ���о��
volatile QueueHandle_t PlayMusicName_Message;		//�������ִ����ļ�����ϢϢ���о��
volatile QueueHandle_t PlayVideoName_Message;		//������Ƶ�����ļ�����ϢϢ���о��

//����������
TaskHandle_t Task6KeyFun_Handler;
//������ʼ������
TaskHandle_t StartTask_Handler;
//��ҳ������
TaskHandle_t Task1MainMenu_Handler;
//LCD����������
TaskHandle_t Task2LCDTest_Handler;
//�������ֿ���������
TaskHandle_t Task3PlayMusicContrl_Handler;
//��������������
TaskHandle_t Task3PlayMusic_Handler;
//������Ƶ����������
TaskHandle_t Task4PlayVideoContrl_Handler;
//������Ƶ������
TaskHandle_t Task4PlayVideo_Handler;

//�����ջ��С	
#define START_STK_SIZE 						(1024)
#define APP_MAIN_MENU_STK_SIZE        		(1024*20)	
#define APP_LCD_TEST_STK_SIZE         		(1024)
#define APP_PLAY_VIDEO_CONTROL_STK_SIZE     (1024*10)
#define APP_PLAY_VIDEO_STK_SIZE          	(1024*10)
#define APP_PLAY_MUSIC_CONTROL_STK_SIZE     (1024*10)
#define APP_PLAY_MUSIC_STK_SIZE          	(1024*10)
#define	APP_DISPLAY_PHOTO_STK_SIZE          (1024)
#define	APP_DEMO_PROGRAM_STK_SIZE          	(1024)

//���������ջ��С	
#define APP_KEY_FUN_STK_SIZE          		(1024)

#define APP_MAIN_MENU_PRIORITY        		 (10)
#define APP_LCD_TEST_PRIORITY                (8)
#define APP_PLAY_VIDEO_CONTRL_PRIORITY       (9)
#define APP_PLAY_MUSIC_CONTRL_PRIORITY       (7)
#define APP_DEMO_PROGRAM_PRIORITY            (11)
#define APP_PLAY_VIDEO_PRIORITY           	 (12)
#define APP_PLAY_MUSIC_PRIORITY              (14)
#define APP_KEY_FUN_PRIORITY            	 (6)
#define START_TASK_PRIO						 (5)
//�������ȼ�


static void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();		//�ٽ���
	
	MenuKey_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char));        	//������ҳ������Ϣ����
	LcdTestMenu_Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char));   //��������LCD������Ϣ����
	PlayVideoMenu_Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char)); //����������Ƶ������Ϣ����
	//PlayMusic_Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char)); //����ͼƬ��ʾ������Ϣ����
	PlayMusicMenu_Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char)); //�����������ְ�����Ϣ����
	DemoMenu_Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char));  //����Demo��ʾ������Ϣ����
	PlayMusicName_Message=xQueueCreate(KEYMSG_Q_NUM,80);  //����Demo music��ʾ������Ϣ����
	PlayVideoName_Message=xQueueCreate(KEYMSG_Q_NUM,80);  //����Demo video��ʾ������Ϣ����
   	//������
	sysprintf("start_task task creation!\r\n");
	
	//������������
    xTaskCreate((TaskFunction_t )Task6_Key_Fun,             
                (const char*    )"Task6_Key_Fun",           
                (uint16_t       )APP_KEY_FUN_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_KEY_FUN_PRIORITY,        
                (TaskHandle_t*  )&Task6KeyFun_Handler);  
				
	//������ҳ��ʾ����
    xTaskCreate((TaskFunction_t )Task1_Main_Menu,             
                (const char*    )"Task1_Main_Menu",           
                (uint16_t       )APP_MAIN_MENU_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_MAIN_MENU_PRIORITY,        
                (TaskHandle_t*  )&Task1MainMenu_Handler);  
	
	//����LCD��������
	xTaskCreate((TaskFunction_t )Task2_LCD_Test,             
			(const char*    )"Task2_LCD_Test",           
			(uint16_t       )APP_LCD_TEST_STK_SIZE,        
			(void*          )NULL,                  
			(UBaseType_t    )APP_LCD_TEST_PRIORITY,        
			(TaskHandle_t*  )&Task2LCDTest_Handler);  
				
	//�������ֲ��ſ�������
    xTaskCreate((TaskFunction_t )Task3_Play_Music_Contrl,             
                (const char*    )"Task3_Play_Music_Contrl",           
                (uint16_t       )APP_PLAY_MUSIC_CONTROL_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_PLAY_MUSIC_CONTRL_PRIORITY,        
                (TaskHandle_t*  )&Task3PlayMusicContrl_Handler);  
				
	//�������ֲ�������
    xTaskCreate((TaskFunction_t )Task_Play_Music,             
                (const char*    )"Task3_Play_Music",           
                (uint16_t       )APP_PLAY_MUSIC_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_PLAY_MUSIC_PRIORITY,        
                (TaskHandle_t*  )&Task3PlayMusic_Handler);  

		//������Ƶ���ſ�������
    xTaskCreate((TaskFunction_t )Task4_Play_Video_Contrl,             
                (const char*    )"Task4_Play_Video_Contrl",           
                (uint16_t       )APP_PLAY_VIDEO_CONTROL_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_PLAY_VIDEO_CONTRL_PRIORITY,        
                (TaskHandle_t*  )&Task4PlayVideoContrl_Handler);  
				
	//�������ֲ�������
    xTaskCreate((TaskFunction_t )Task_Play_Video,             
                (const char*    )"Task_Play_Video",           
                (uint16_t       )APP_PLAY_VIDEO_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_PLAY_VIDEO_PRIORITY,        
                (TaskHandle_t*  )&Task4PlayVideo_Handler);  
				
		taskEXIT_CRITICAL();            //�˳��ٽ���
	   vTaskDelete(StartTask_Handler); //ɾ����ʼ����
}


void Task_Create(void)
{
	char ListNameBuff[30];
	char *p = NULL;
	int i;
	
#if 0
	sysprintf("print music file!\r\n");
	for(i = 0; i < AUDIO_FILE_LIST.nandflash_audio_file_num;i++)
	{
		sysprintf("%d ,%s\r\n", i, AUDIO_FILE_LIST.nandflash_audio_name[i]);
		p = AUDIO_FILE_LIST.nandflash_audio_name[i];
		sysprintf("%d ,[0]:%x [1]:%x [2]:%x [3]:%x [4]:%x [5]:%x [6]:%x [7]:%x [8]:%x [9]:%x [10]:%x [11]:%x [12]:%x\r\n", i,\
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12]);
	}
	
	for(i = 0; i < AUDIO_FILE_LIST.sd_audio_file_num;i++)
	{
		sysprintf("%d ,%s\r\n", i, AUDIO_FILE_LIST.sd_audio_name[i]);
	}
	
	sysprintf("print video file!\r\n");
	for(i = 0; i < VIDEO_FILE_LIST.nandflash_audio_file_num;i++)
	{
		sysprintf("%d ,%s\r\n", i, VIDEO_FILE_LIST.nandflash_audio_name[i]);
		p = VIDEO_FILE_LIST.nandflash_audio_name[i];
		sysprintf("%d ,[0]:%x [1]:%x [2]:%x [3]:%x [4]:%x [5]:%x [6]:%x [7]:%x [8]:%x [9]:%x [10]:%x [11]:%x [12]:%x\r\n", i,\
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12]);
	}
	
	for(i = 0; i < VIDEO_FILE_LIST.sd_audio_file_num;i++)
	{
		sysprintf("%d ,%s\r\n", i, VIDEO_FILE_LIST.sd_audio_name[i]);
	}
#endif
	//������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������   v

	BackLightAdj(50);
	vTaskStartScheduler();	
}

