#include "task_lcd_test_app.h"
#include "task_play_music_app.h"
#include "task_play_video_app.h"
#include "task_main_menu_app.h"
#include "task_key_handle.h"
#include "task_comm_type.h"
#include "ctp.h"

//任务函数
static void start_task(void *pvParameters);

//按键消息队列的数量
#define KEYMSG_Q_NUM    (4)  							//按键消息队列的数量  
volatile QueueHandle_t MenuKey_Queue;				//主页按键消息队列句柄
volatile QueueHandle_t LcdTestMenu_Key_Queue;		//LCD测试页按键消息队列句柄
volatile QueueHandle_t PlayVideoMenu_Key_Queue;	//视频播放页按键消息队列句柄
volatile QueueHandle_t PlayMusicMenu_Key_Queue;	//播放音乐页按键消息队列句柄
volatile QueueHandle_t DisplayPhoto_Key_Queue;		//显示图片按键消息队列句柄
volatile QueueHandle_t DemoMenu_Key_Queue;			//EMWINE页按键消息队列句柄
volatile QueueHandle_t PlayMusicName_Message;		//播放音乐传递文件名消息息队列句柄
volatile QueueHandle_t PlayVideoName_Message;		//播放视频传递文件名消息息队列句柄

//按键任务句柄
TaskHandle_t Task6KeyFun_Handler;
//创建开始任务句柄
TaskHandle_t StartTask_Handler;
//主页任务句柄
TaskHandle_t Task1MainMenu_Handler;
//LCD测试任务句柄
TaskHandle_t Task2LCDTest_Handler;
//播放音乐控制任务句柄
TaskHandle_t Task3PlayMusicContrl_Handler;
//播放音乐任务句柄
TaskHandle_t Task3PlayMusic_Handler;
//播放视频控制任务句柄
TaskHandle_t Task4PlayVideoContrl_Handler;
//播放视频任务句柄
TaskHandle_t Task4PlayVideo_Handler;

//任务堆栈大小	
#define START_STK_SIZE 						(1024)
#define APP_MAIN_MENU_STK_SIZE        		(1024*20)	
#define APP_LCD_TEST_STK_SIZE         		(1024)
#define APP_PLAY_VIDEO_CONTROL_STK_SIZE     (1024*10)
#define APP_PLAY_VIDEO_STK_SIZE          	(1024*10)
#define APP_PLAY_MUSIC_CONTROL_STK_SIZE     (1024*10)
#define APP_PLAY_MUSIC_STK_SIZE          	(1024*10)
#define	APP_DISPLAY_PHOTO_STK_SIZE          (1024)
#define	APP_DEMO_PROGRAM_STK_SIZE          	(1024)

//按键任务堆栈大小	
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
//任务优先级


static void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();		//临界区
	
	MenuKey_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char));        	//创建主页按键消息队列
	LcdTestMenu_Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char));   //创建测试LCD按键消息队列
	PlayVideoMenu_Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char)); //创建播放视频按键消息队列
	//PlayMusic_Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char)); //创建图片显示按键消息队列
	PlayMusicMenu_Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char)); //创建播放音乐按键消息队列
	DemoMenu_Key_Queue=xQueueCreate(KEYMSG_Q_NUM,sizeof(unsigned char));  //创建Demo演示按键消息队列
	PlayMusicName_Message=xQueueCreate(KEYMSG_Q_NUM,80);  //创建Demo music演示按键消息队列
	PlayVideoName_Message=xQueueCreate(KEYMSG_Q_NUM,80);  //创建Demo video演示按键消息队列
   	//任务句柄
	sysprintf("start_task task creation!\r\n");
	
	//创建按键任务
    xTaskCreate((TaskFunction_t )Task6_Key_Fun,             
                (const char*    )"Task6_Key_Fun",           
                (uint16_t       )APP_KEY_FUN_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_KEY_FUN_PRIORITY,        
                (TaskHandle_t*  )&Task6KeyFun_Handler);  
				
	//创建主页显示任务
    xTaskCreate((TaskFunction_t )Task1_Main_Menu,             
                (const char*    )"Task1_Main_Menu",           
                (uint16_t       )APP_MAIN_MENU_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_MAIN_MENU_PRIORITY,        
                (TaskHandle_t*  )&Task1MainMenu_Handler);  
	
	//创建LCD测试任务
	xTaskCreate((TaskFunction_t )Task2_LCD_Test,             
			(const char*    )"Task2_LCD_Test",           
			(uint16_t       )APP_LCD_TEST_STK_SIZE,        
			(void*          )NULL,                  
			(UBaseType_t    )APP_LCD_TEST_PRIORITY,        
			(TaskHandle_t*  )&Task2LCDTest_Handler);  
				
	//创建音乐播放控制任务
    xTaskCreate((TaskFunction_t )Task3_Play_Music_Contrl,             
                (const char*    )"Task3_Play_Music_Contrl",           
                (uint16_t       )APP_PLAY_MUSIC_CONTROL_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_PLAY_MUSIC_CONTRL_PRIORITY,        
                (TaskHandle_t*  )&Task3PlayMusicContrl_Handler);  
				
	//创建音乐播放任务
    xTaskCreate((TaskFunction_t )Task_Play_Music,             
                (const char*    )"Task3_Play_Music",           
                (uint16_t       )APP_PLAY_MUSIC_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_PLAY_MUSIC_PRIORITY,        
                (TaskHandle_t*  )&Task3PlayMusic_Handler);  

		//创建视频播放控制任务
    xTaskCreate((TaskFunction_t )Task4_Play_Video_Contrl,             
                (const char*    )"Task4_Play_Video_Contrl",           
                (uint16_t       )APP_PLAY_VIDEO_CONTROL_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_PLAY_VIDEO_CONTRL_PRIORITY,        
                (TaskHandle_t*  )&Task4PlayVideoContrl_Handler);  
				
	//创建音乐播放任务
    xTaskCreate((TaskFunction_t )Task_Play_Video,             
                (const char*    )"Task_Play_Video",           
                (uint16_t       )APP_PLAY_VIDEO_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )APP_PLAY_VIDEO_PRIORITY,        
                (TaskHandle_t*  )&Task4PlayVideo_Handler);  
				
		taskEXIT_CRITICAL();            //退出临界区
	   vTaskDelete(StartTask_Handler); //删除开始任务
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
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄   v

	BackLightAdj(50);
	vTaskStartScheduler();	
}

