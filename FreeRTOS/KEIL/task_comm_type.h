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
	char nandflash_audio_name[MAX_AUDIO_FILE_NUM][MAX_AUDIO_NAME_LEN];	//nandflash ����Ƶ�ļ���
	char sd_audio_name[MAX_AUDIO_FILE_NUM][MAX_AUDIO_NAME_LEN];			//sd������Ƶ�ļ���
	short nandflash_audio_file_num;
	short sd_audio_file_num;
	short nandflash_audioFile_curPos;	//Nandflash����Ƶ�ļ���ǰλ��
	short sd_audioFile_curPos;			//SD������Ƶ�ļ���ǰλ��
	SELECT_DISK diskMode;				//ѡ����ĸ������в�������
	
}FILE_LIST;

extern volatile FILE_LIST AUDIO_FILE_LIST;
extern volatile FILE_LIST VIDEO_FILE_LIST;

extern volatile QueueHandle_t MenuKey_Queue;				//��ҳ������Ϣ���о��
extern volatile QueueHandle_t LcdTestMenu_Key_Queue;		//LCD����ҳ������Ϣ���о��
extern volatile QueueHandle_t PlayVideoMenu_Key_Queue;	//��Ƶ����ҳ������Ϣ���о��
extern volatile QueueHandle_t PlayMusicMenu_Key_Queue;	//��������ҳ������Ϣ���о��
extern volatile QueueHandle_t DisplayPhoto_Key_Queue;		//��ʾͼƬ������Ϣ���о��
extern volatile QueueHandle_t DemoMenu_Key_Queue;			//EMWINEҳ������Ϣ���о��
extern volatile QueueHandle_t PlayMusicName_Message;		//�������ִ����ļ�����ϢϢ���о��
extern volatile QueueHandle_t PlayVideoName_Message;		//������Ƶ�����ļ�����ϢϢ���о��

extern volatile char cur_item;		//��ǰӦ�ó���	LCD , MUSIC ,VIDEO ,DEMO
extern volatile PLAY_CTRL_E musicPlayStatus;	//��������Ƶ״̬
extern volatile HMI_MENU  mainMenuIndex;

//����������
extern TaskHandle_t Task6KeyFun_Handler;
//������ʼ������
extern TaskHandle_t StartTask_Handler;
//��ҳ������
extern TaskHandle_t Task1MainMenu_Handler;
//LCD����������
extern TaskHandle_t Task2LCDTest_Handler;
//�������ֿ���������
extern TaskHandle_t Task3PlayMusicContrl_Handler;
//��������������
extern TaskHandle_t Task3PlayMusic_Handler;
//������Ƶ����������
extern TaskHandle_t Task4PlayVideoContrl_Handler;
//������Ƶ������
extern TaskHandle_t Task4PlayVideo_Handler;

void MP3Control_Init(void);
void MP3Control_En(void);
void MP3Control_Dis(void);

void Select_File(FILE_LIST *typeFileList );
void DrawMenu(char n);

#endif
