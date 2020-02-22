#include "task_lcd_test_app.h"
#include "w55fa93_vpost.h"
#include "jpegSample.h"
#include "graph.h"
#include "GUI.h"

#define MAX_PHOTO_NUM	(14)

void Test_LCDMENU(char n)
{
	switch(n)
	{
		case 0: LCD_Clear(LCD_GUI_BLACK, BUFF0);
				LCDUpdateScreen(BUFF0);
				break;
		case 1: Vpost_PictureFrame(0, 0, _LCD_WIDTH-1, _LCD_HEIGHT-1, LCD_GUI_WHITE, BUFF0);
				LCDUpdateScreen(BUFF0);
				break;
		case 2: LCD_Clear(LCD_GUI_WHITE, BUFF0);
				LCDUpdateScreen(BUFF0);
				break;
		case 3:	LCD_Clear(LCD_GUI_RED, BUFF0);
				LCDUpdateScreen(BUFF0);
				break;
		case 4:	LCD_Clear(LCD_GUI_GREEN, BUFF0);
				LCDUpdateScreen(BUFF0);
				break;
		case 5: LCD_Clear(LCD_GUI_BLUE, BUFF0);
				LCDUpdateScreen(BUFF0);
				break;
		case 6: lcd_gay();
				LCDUpdateScreen(BUFF0);
				break;
		case 7:JpegDecTest("C:\\photo\\1.jpg", VPOST);
				break;
		case 8:JpegDecTest("C:\\photo\\2.jpg", VPOST);
				break;
		case 9:JpegDecTest("C:\\photo\\3.jpg", VPOST);
				break;	
		case 10:JpegDecTest("C:\\photo\\4.jpg", VPOST);
				break;
		case 11:JpegDecTest("C:\\photo\\5.jpg", VPOST);
				break;
		case 12:JpegDecTest("C:\\photo\\6.jpg", VPOST);
				break;	
		case 13:JpegDecTest("C:\\photo\\7.jpg", VPOST);
				break;
		case 14:JpegDecTest("C:\\photo\\8.jpg", VPOST);	
				break;
		default:break;

	}
}

void TestPhoto_Dealkey(unsigned short input)
{
	static unsigned short curphoto = 1;
	
	switch(input)
	{
		case BUTTON_UP:
			if(curphoto)curphoto--;
			else curphoto=MAX_PHOTO_NUM-1;
			Test_LCDMENU(curphoto);
			sysprintf("up curphoto:0x%x\n", curphoto);
			break;
		case BUTTON_DOWN:
			if(curphoto<MAX_PHOTO_NUM-1)curphoto++;
			else curphoto=0;
			Test_LCDMENU(curphoto);
			sysprintf("down curphoto:0x%x\n", curphoto);
			break;
		case BUTTON_LOCK:	
			sysprintf("lock curphoto:0x%x\n", curphoto);		
			break;
		case BUTTON_OK:
			sysprintf("OK curphoto:0x%x\n", curphoto);
			break;
		case BUTTON_RETURN:
			//	OSTaskSuspend(APP_PLAY_MUSIC_PRIORITY);//挂起音乐播放
			//	OSTaskSuspend(APP_LCD_TEST_PRIORITY);//挂起LCD测试播放
			//	OSTaskSuspend(APP_PLAY_VIDEO_PRIORITY);//挂起视频播放
			//	OSTaskSuspend(APP_DEMO_PROGRAM_PRIORITY);//挂起DEMO程序播放
				DrawMenu(cur_item);
				mainMenuIndex = MENU_IDLE;
			//	OSTaskResume(APP_MAIN_MENU_PRIORITY);//唤醒主界面显示
			//	OSTaskSuspend(APP_LCD_TEST_PRIORITY);//挂起LCD测试播放
			sysprintf("return curphoto:0x%x\n", curphoto);
			break;
		}
}

void Task2_LCD_Test(void *pvParameters)
{
	static unsigned char key=0;
	BaseType_t err;
	
	sysprintf("Task2_LCD_Test task has creation\r\n");
	while(1)
	{
		if(LcdTestMenu_Key_Queue != NULL)
		{
			if(xQueueReceive(LcdTestMenu_Key_Queue,&key,portMAX_DELAY))
			{
				TestPhoto_Dealkey(key);
			}
		}
		vTaskDelay(50);
	}
}




