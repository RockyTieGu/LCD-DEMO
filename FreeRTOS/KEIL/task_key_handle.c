#include "task_key_handle.h"
#include "GUIDEMO.h"
#include "GUI.h"

void  Task6_Key_Fun (void *pvParameters)
{
	static int key = 0;
	mainMenuIndex = MENU_IDLE;
	WM_MESSAGE message;
	sysprintf("Task6_Key_Fun task has creation\r\n");
	while (1) 
	{                                             
		key = boardKeyMSG_handle();
		if(key)//����������
		{	
			sysprintf("Task6_Key_Fun key:%d mainMenuIndex:%d \n", key,mainMenuIndex);
			switch((char)mainMenuIndex) 
			{
				case MENU_IDLE:	
						if((xQueueSend(MenuKey_Queue,&key,10)) == errQUEUE_FULL)
						{
							sysprintf("MENU_IDLE Key_Queue is full, send message fail!\r\n");
						}
						break;
				case MENU_LCD_TEST:
						if((xQueueSend(LcdTestMenu_Key_Queue,&key,10)) == errQUEUE_FULL)
						{
							sysprintf("MENU_LCD_TEST Key_Queue is full, send message fail!\r\n");
						}
						break;
				case MENU_PLAY_VIDEO:	
						if((xQueueSend(PlayVideoMenu_Key_Queue,&key,10)) == errQUEUE_FULL)
						{
							sysprintf("MENU_PLAY_VIDEO Key_Queue is full, send message fail!\r\n");
						}
						break;
				case MENU_PLAY_MUSIC:
						if((xQueueSend(PlayMusicMenu_Key_Queue,&key,10)) == errQUEUE_FULL)
						{
							sysprintf("MENU_PLAY_MUSIC Key_Queue is full, send message fail!\r\n");
						}
						break;
				case MENU_DEMO_PROGRAM:
						if((xQueueSend(DemoMenu_Key_Queue,&key,10)) == errQUEUE_FULL)
						{
							sysprintf("DemoMenu_Key_Queue Key_Queue is full, send message fail!\r\n");
						}
						
						break;
			}
		}	
		vTaskDelay(50); //2
		//GT911_Scan(0);
		//TOUCH_Scan();
			GUI_TOUCH_Exec();//���ô�������,��VOID DrvADC_PollingWT(VOID)������OSTimeDly����
	}
}
