#ifndef _HMI_H_
#define _HMI_H_
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

typedef void ( *SampleMenu_Dealkey)(unsigned short input);
void Test_LCDMENU(char n);
void TestPhoto_Dealkey(unsigned short input);

void Menu_Dealkey(unsigned short input);
void CreatAllTask(void);
void Task_Create(void);
int ReadImageDirectory_Path(char * directName);
int  SearcheSpecifiedDirectory_filePath(char *suDirName);
#endif
