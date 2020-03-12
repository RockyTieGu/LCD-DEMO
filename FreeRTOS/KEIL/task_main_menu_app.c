#include "task_main_menu_app.h"
#include "task_lcd_test_app.h"
#include "task_play_video_app.h"
#include "task_play_music_app.h"
#include "w55fa93_vpost.h"
#include "jpegSample.h"
#include "xbf_font.h"
#include "graph.h"
#include "GUI.h"

volatile char cur_item = 0;
void DrawMenu(char n)
{
	unsigned short start_x, start_y;
	const char *menuName[] = {"LCD", "VIDEO", "MP3", "eMWin"};
	char i;
	//char buff[128];
	start_x = _LCD_WIDTH/4;
	start_y = _LCD_HEIGHT/4;
	
	//GUI_SetColor(GUI_BLUE);
	GUI_SetBkColor(GUI_WHITE);
	//GUI_Clear();
	JpegDecTest(MAIN_PAGE_BACKGROUND_PICTURE_PATH, BUFF0);
	LCDUpdateScreen(BUFF0);
	
	for(i = 0; i < 4; i++)
	{	start_x =  _LCD_WIDTH/6 + _LCD_WIDTH/2 *(i%2);
		start_y = _LCD_HEIGHT/6 + _LCD_HEIGHT/2 *(i/2);
		if(i != n)
		{
			GUI_SetColor(GUI_LIGHTRED);
			GUI_FillRoundedRect( start_x, start_y, start_x + _LCD_WIDTH/6, start_y +  _LCD_HEIGHT/6, 10);
			GUI_SetColor(GUI_WHITE);
			GUI_FillRoundedRect( start_x+10, start_y+10, start_x + _LCD_WIDTH/6 - 10, start_y+ _LCD_HEIGHT/6 -10 , 10);
		
			GUI_SetColor(GUI_BLUE);
			GUI_SetFont(GUI_FONT_10S_ASCII); //GUI_FONT_20B_ASCII
			//GUI_DispStringAt(menuName[i],start_x+20,start_y + 40);
			GUI_DispStringHCenterAt(menuName[i], start_x + _LCD_WIDTH/12 , start_y + _LCD_HEIGHT/12);
		}else
		{
			GUI_SetColor(GUI_DARKRED);
			GUI_FillRoundedRect( start_x, start_y, start_x + _LCD_WIDTH/6, start_y +  _LCD_HEIGHT/6, 10);;
			GUI_SetColor(GUI_WHITE);
			GUI_FillRoundedRect( start_x+10, start_y+10, start_x + _LCD_WIDTH/6 - 10, start_y+ _LCD_HEIGHT/6 -10 , 10);
		
			GUI_SetColor(GUI_BLUE);
			GUI_SetFont(GUI_FONT_10S_ASCII); //GUI_FONT_24B_ASCII
		//	GUI_DispStringAt(menuName[i],start_x+20,start_y + 40);
			GUI_DispStringHCenterAt(menuName[i], start_x + _LCD_WIDTH/12 , start_y + _LCD_HEIGHT/12);
		}
	}
}

static void Menu_Dealkey(unsigned short input)
{
	switch(input)
	{
		case BUTTON_UP:
			if(cur_item> 0)cur_item--;
			else cur_item = 3;
			DrawMenu(cur_item);
			sysprintf("BUTTON_UP curphoto:0x%x\n", cur_item);
			break;
		case BUTTON_DOWN:
			if(cur_item<3)cur_item++;
			else cur_item = 0;
			DrawMenu(cur_item);
			sysprintf("BUTTON_DOWN curphoto:0x%x\n", cur_item);
			break;
		case BUTTON_LOCK:	
			break;
		case BUTTON_OK:
			sysprintf("BUTTON_OK curphoto:0x%x\n", cur_item);
			if(cur_item == 0)
			{
				mainMenuIndex = MENU_LCD_TEST;
				Test_LCDMENU(0);
				sysprintf("LCD TEST\r\n");
			}else if(cur_item == 1)
			{
				vTaskSuspend(Task2LCDTest_Handler);
				vTaskSuspend(Task3PlayMusic_Handler);
				vTaskSuspend(Task3PlayMusicContrl_Handler);
				mainMenuIndex = MENU_PLAY_VIDEO;
				loadPlayVideo_Menu();
				if(VIDEO_FILE_LIST.diskMode == NANDFLASH_MODE)
				{
					Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
				}
				else if(VIDEO_FILE_LIST.diskMode == SD_MODE)
				{
					Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
				}
				else 
				{
					VIDEO_FILE_LIST.diskMode = IDLE_MODE;
					VIDEO_FILE_LIST.nandflash_audioFile_curPos = VIDEO_FILE_LIST.sd_audioFile_curPos = 0;
					Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
				}

				sysprintf("play mp4\r\n");
			}
			else if(cur_item == 2)
			{
				vTaskSuspend(Task2LCDTest_Handler);
				vTaskSuspend(Task4PlayVideoContrl_Handler);
				vTaskSuspend(Task4PlayVideo_Handler);
				mainMenuIndex = MENU_PLAY_MUSIC;
				loadPlayMusic_Menu();
				if(AUDIO_FILE_LIST.diskMode == NANDFLASH_MODE)
				{
					Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
				}
				else if(AUDIO_FILE_LIST.diskMode == SD_MODE)
				{
					Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
				}
				else 
				{
					AUDIO_FILE_LIST.diskMode = IDLE_MODE;
					AUDIO_FILE_LIST.nandflash_audioFile_curPos = AUDIO_FILE_LIST.sd_audioFile_curPos = 0;
					Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
				}
			}
			else if(cur_item == 3)
			{
				/*¹ÒÆðMAIN_MENU,LCD_TEST, PLAY_MUSIC,PLAY_VIDEO*/
				vTaskSuspend(Task2LCDTest_Handler);
				vTaskSuspend(Task3PlayMusic_Handler);
				vTaskSuspend(Task3PlayMusicContrl_Handler);
				vTaskSuspend(Task4PlayVideoContrl_Handler);
				vTaskSuspend(Task4PlayVideo_Handler);
				mainMenuIndex = MENU_DEMO_PROGRAM;
				GUIDEMO_Main();
				vTaskResume(Task2LCDTest_Handler);
				vTaskResume(Task3PlayMusic_Handler);
				vTaskResume(Task3PlayMusicContrl_Handler);
				vTaskResume(Task4PlayVideoContrl_Handler);
				vTaskResume(Task4PlayVideo_Handler);
				mainMenuIndex = MENU_IDLE;
			//	DrawMenu(cur_item);
			}
			break;
		case BUTTON_RETURN:
				mainMenuIndex = MENU_IDLE;
				DrawMenu(cur_item);
			sysprintf("return curphoto:0x%x\n", cur_item);
			break;
		}
}


void Task1_Main_Menu(void *p_arg)
{
	static unsigned int key=0;
	_ShowXBF();
	DrawMenu(cur_item);
	
	sysprintf("Task1_Main_Menu task creation\r\n");
	while(1)
	{
		if(MenuKey_Queue != NULL)
		{
			if(xQueueReceive(MenuKey_Queue,&key,portMAX_DELAY))
			{
				Menu_Dealkey(key);
			}
		}
		vTaskDelay(50);
	}
}

