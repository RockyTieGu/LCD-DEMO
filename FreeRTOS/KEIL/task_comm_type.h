#ifndef _TASK_COMM_TYPE_H_
#define _TASK_COMM_TYPE_H_

#include "wb_include.h"
#include "avilib.h"
#include "avifile.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "key.h"

#define MESSAGE_FILE_NAME_LEN	(80)

#define MAX_AUDIO_FILE_NUM		(100)
#define MAX_AUDIO_NAME_LEN		(80)

#define MAIN_PAGE_BACKGROUND_PICTURE_PATH				"C:\\1.jpg"
#define AUDIO_FILE_NANDFLASH_PATH						"D:\\music\\"
#define AUDIO_FILE_SD_PATH								"X:\\music\\"

#define VIDEO_FILE_NANDFLASH_PATH						"D:\\video\\"
#define VIDEO_FILE_SD_PATH								"X:\\video\\"



typedef enum
{
	MENU_IDLE,
	MENU_LCD_TEST,
	MENU_PLAY_VIDEO,
//	MENU_DISPLAY_PHOTO,
	MENU_PLAY_MUSIC,
	MENU_DEMO_PROGRAM,
	MENU_MAX
}HMI_MENU;

typedef enum enumSELECT_DISK
{
	IDLE_MODE,
	NANDFLASH_MODE,
	SD_MODE
}SELECT_DISK;

typedef struct file_list
{
	char nandflash_audio_name[MAX_AUDIO_FILE_NUM][MAX_AUDIO_NAME_LEN];	//nandflash 中音频文件名
	char sd_audio_name[MAX_AUDIO_FILE_NUM][MAX_AUDIO_NAME_LEN];			//sd卡中音频文件名
	short nandflash_audio_file_num;
	short sd_audio_file_num;
	short nandflash_audioFile_curPos;	//Nandflash中音频文件当前位置
	short sd_audioFile_curPos;			//SD卡中音频文件当前位置
	SELECT_DISK diskMode;				//选择从哪个磁盘中播放音乐
	
}FILE_LIST;

extern volatile FILE_LIST AUDIO_FILE_LIST;
extern volatile FILE_LIST VIDEO_FILE_LIST;

extern volatile QueueHandle_t MenuKey_Queue;				//主页按键消息队列句柄
extern volatile QueueHandle_t LcdTestMenu_Key_Queue;		//LCD测试页按键消息队列句柄
extern volatile QueueHandle_t PlayVideoMenu_Key_Queue;	//视频播放页按键消息队列句柄
extern volatile QueueHandle_t PlayMusicMenu_Key_Queue;	//播放音乐页按键消息队列句柄
extern volatile QueueHandle_t DisplayPhoto_Key_Queue;		//显示图片按键消息队列句柄
extern volatile QueueHandle_t DemoMenu_Key_Queue;			//EMWINE页按键消息队列句柄
extern volatile QueueHandle_t PlayMusicName_Message;		//播放音乐传递文件名消息息队列句柄
extern volatile QueueHandle_t PlayVideoName_Message;		//播放视频传递文件名消息息队列句柄

extern volatile char cur_item;		//当前应用程序	LCD , MUSIC ,VIDEO ,DEMO
extern volatile PLAY_CTRL_E musicPlayStatus;	//播放音视频状态
extern volatile HMI_MENU  mainMenuIndex;

//按键任务句柄
extern TaskHandle_t Task6KeyFun_Handler;
//创建开始任务句柄
extern TaskHandle_t StartTask_Handler;
//主页任务句柄
extern TaskHandle_t Task1MainMenu_Handler;
//LCD测试任务句柄
extern TaskHandle_t Task2LCDTest_Handler;
//播放音乐控制任务句柄
extern TaskHandle_t Task3PlayMusicContrl_Handler;
//播放音乐任务句柄
extern TaskHandle_t Task3PlayMusic_Handler;
//播放视频控制任务句柄
extern TaskHandle_t Task4PlayVideoContrl_Handler;
//播放视频任务句柄
extern TaskHandle_t Task4PlayVideo_Handler;

void MP3Control_Init(void);
void MP3Control_En(void);
void MP3Control_Dis(void);

void Select_File(FILE_LIST *typeFileList );
void DrawMenu(char n);

#endif
