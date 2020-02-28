#include "task_play_video_app.h"
#include "task_play_music_app.h"
#include "w55fa93_vpost.h"
#include "xbf_font.h"
#include "nvtfat.h"
#include "stdio.h"
#include "string.h"
#include "graph.h"
#include "GUI.h"

static volatile PLAY_CTRL_E videoPlayStatus = PLAY_CTRL_STOP;
extern volatile char nAudioPlayVolume;

void videoPlay_BackGround(void)
{
	
	GUI_SetBkColor(GUI_BLUE);
	GUI_Clear();
	GUI_SetFont(GUI_FONT_20B_ASCII); //GUI_Font32B_1  
	GUI_SetColor(GUI_WHITE);
	GUI_DrawLine(0,_LCD_HEIGHT/5, _LCD_WIDTH, _LCD_HEIGHT/5);
	GUI_DrawLine(0,_LCD_HEIGHT*4/5, _LCD_WIDTH, _LCD_HEIGHT*4/5);
	GUI_DrawLine(_LCD_WIDTH/2,0, _LCD_WIDTH/2, _LCD_HEIGHT*4/5);
//GUI_FillRect(0,_LCD_HEIGHT/5, _LCD_WIDTH, _LCD_HEIGHT/5);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringHCenterAt("NAND FLASH D:", _LCD_WIDTH/4 , _LCD_HEIGHT/5/3);
	GUI_DispStringHCenterAt("MINI SD X:", _LCD_WIDTH*3/4 , _LCD_HEIGHT/5/3);
	GUI_SetFont(GUI_FONT_24B_ASCII);
	GUI_DispStringHCenterAt("Video play", _LCD_WIDTH/2 , _LCD_HEIGHT*4/5 + 1);
	GUI_UC_SetEncodeUTF8(); 
	SetFont_Xbf();
}

void loadPlayVideo_Menu()
{
	videoPlay_BackGround();
	Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
	Audio_AdjustVolume();
}

void  avi_play_control(AVI_INFO_T *aviInfo)
{
	static INT	last_time;
	int    frame_rate;
	
	if (aviInfo->uPlayCurTimePos != 0)
		frame_rate = ((aviInfo->uVidFramesPlayed - aviInfo->uVidFramesSkipped) * 100) / aviInfo->uPlayCurTimePos;
	
	if (aviInfo->uPlayCurTimePos - last_time > 100)
	{
		sysprintf("%02d:%02d / %02d:%02d  Vid fps: %d / %d\n", 
			aviInfo->uPlayCurTimePos / 6000, (aviInfo->uPlayCurTimePos / 100) % 60,
			aviInfo->uMovieLength / 6000, (aviInfo->uMovieLength / 100) % 60,
			frame_rate, aviInfo->uVideoFrameRate);
		last_time = aviInfo->uPlayCurTimePos;

		vTaskDelay(5);
	}
}

static void startPlayMP4(char * audioFileName)
{
	int			nStatus;
	char		suFileName[256] ;
	//char 		assicFilName[128];
	
	MP3Control_En();
	GUI_SetBkColor(GUI_BLUE);								//…Ë÷√±≥æ∞—’…´ GUI_LIGHTBLUE
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_16_ASCII);
	memset(suFileName,0,sizeof(suFileName));
	if(VIDEO_FILE_LIST.diskMode == NANDFLASH_MODE)
	{	
		fsAsciiToUnicode(VIDEO_FILE_NANDFLASH_PATH,suFileName, TRUE);
		fsUnicodeStrCat(suFileName, audioFileName);
	}
	else if(VIDEO_FILE_LIST.diskMode == SD_MODE)
	{
		fsAsciiToUnicode(VIDEO_FILE_SD_PATH,suFileName,TRUE);
		fsUnicodeStrCat(suFileName, audioFileName);
	}
	else
	{
		sysprintf("AUDIO_FILE_LIST.diskMode error!\r\n");
		return; 
	}
	sysprintf("startPlayMP4 %s\r\n", suFileName);
	AVI_play(suFileName);//AVI_play("D:\\girlMV.avi");
 // fsAsciiToUnicode("D:\\1.avi", suFileName, TRUE);
//if (aviPlayFile(suFileName, 0, 0, 1, avi_play_control) < 0)
//	sysprintf("Playback failed, code \n");
		
	MP3Control_Dis();
	loadPlayVideo_Menu();
}



static void MP4_Dealkey(unsigned short input)
{
	char audioFilePath[80];
	char *ptr = NULL;
	char *p = audioFilePath;
	int num;
	
	switch(input)
	{
		case BUTTON_UP:
			if(videoPlayStatus == PLAY_CTRL_STOP)
			{	
				if(VIDEO_FILE_LIST.diskMode == NANDFLASH_MODE)
				{
					if(VIDEO_FILE_LIST.nandflash_audioFile_curPos > 0) VIDEO_FILE_LIST.nandflash_audioFile_curPos--;
					Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
				}
				else if(VIDEO_FILE_LIST.diskMode == SD_MODE)
				{
					if(VIDEO_FILE_LIST.sd_audioFile_curPos > 0) VIDEO_FILE_LIST.sd_audioFile_curPos--;
					Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
				}else if(VIDEO_FILE_LIST.diskMode == IDLE_MODE)
				{
					if(nAudioPlayVolume > 0)nAudioPlayVolume--;
						aviSetPlayVolume(nAudioPlayVolume);
					Audio_AdjustVolume();
				}
			}
			else if(videoPlayStatus == PLAY_CTRL_SPEED)
			{
			//	mflPlayControl(&_tMvCfg,PLAY_CTRL_PAUSE,1);
			//	UpdateMusicPlayStatus(PLAY_CTRL_PAUSE, TRUE);
				sysprintf("MP4 PLAY_CTRL_PAUSE\r\n");
			}
			break;
		case BUTTON_DOWN:
			if(videoPlayStatus == PLAY_CTRL_STOP)
			{
				if(VIDEO_FILE_LIST.diskMode == NANDFLASH_MODE)
				{
					if(VIDEO_FILE_LIST.nandflash_audioFile_curPos < VIDEO_FILE_LIST.nandflash_audio_file_num - 1) VIDEO_FILE_LIST.nandflash_audioFile_curPos++;
					Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
				}
				else if(VIDEO_FILE_LIST.diskMode == SD_MODE)
				{
					if(VIDEO_FILE_LIST.sd_audioFile_curPos < VIDEO_FILE_LIST.sd_audio_file_num - 1) VIDEO_FILE_LIST.sd_audioFile_curPos++;
					Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
				}else if(VIDEO_FILE_LIST.diskMode == IDLE_MODE)
				{
					if(nAudioPlayVolume < 30)nAudioPlayVolume++;
					else	nAudioPlayVolume = 31;
					aviSetPlayVolume(nAudioPlayVolume);
					Audio_AdjustVolume();
				}
			}
			else if(videoPlayStatus == PLAY_CTRL_SPEED)
			{
			//	mflPlayControl(&_tMvCfg,PLAY_CTRL_RESUME,1);
			//	UpdateMusicPlayStatus(PLAY_CTRL_RESUME, TRUE);
				sysprintf("MP4 PLAY_CTRL_RESUME\r\n");
			}
			break;
		case BUTTON_LOCK:
			if(videoPlayStatus == PLAY_CTRL_STOP)
			{
				if(VIDEO_FILE_LIST.diskMode < SD_MODE)
					VIDEO_FILE_LIST.diskMode++;
				else
					VIDEO_FILE_LIST.diskMode = IDLE_MODE;
				
				if(VIDEO_FILE_LIST.diskMode == NANDFLASH_MODE)
					Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
				else if(VIDEO_FILE_LIST.diskMode == SD_MODE)
					Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
				else
					Select_File((FILE_LIST *)(&VIDEO_FILE_LIST));
				sysprintf("press lock button diskMode:%d\n", AUDIO_FILE_LIST.diskMode);	
			}		
			else if(videoPlayStatus == PLAY_CTRL_SPEED)
			{
				//mflPlayControl(&_tMvCfg,PLAY_CTRL_PAUSE,1);
				//sysprintf("MP3 PLAY_CTRL_PAUSE\r\n");
			}
			break;
		case BUTTON_OK:
			if(videoPlayStatus == PLAY_CTRL_STOP)
			{
				if(VIDEO_FILE_LIST.diskMode == NANDFLASH_MODE)
				{
					memset(audioFilePath, 0, sizeof(audioFilePath));
					//sprintf(audioFilePath, "%s%s", AUDIO_FILE_NANDFLASH_PATH, AUDIO_FILE_LIST.nandflash_audio_name[AUDIO_FILE_LIST.nandflash_audioFile_curPos]);
					sysprintf("play music nandflash:%s\r\n",VIDEO_FILE_LIST.nandflash_audio_name[VIDEO_FILE_LIST.nandflash_audioFile_curPos]);
					ptr = (char *)VIDEO_FILE_LIST.nandflash_audio_name[VIDEO_FILE_LIST.nandflash_audioFile_curPos];
					num = GUI_UC_ConvertUTF82UC((const char *)ptr, strlen(ptr), (U16 * )audioFilePath, 80/2);
					sysprintf("NandFlash num:%d [0]:%x [1]:%x [2]:%x [3]:%x [4]:%x [5]:%x [6]:%x [7]:%x [8]:%x [9]:%x [10]:%x [11]:%x [12]:%x [13]:%x [14]:%x\r\n", \
					num, p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14]);
					if (xQueueSend(PlayVideoName_Message,audioFilePath,strlen(audioFilePath) + 1) == errQUEUE_FULL)
					{
						 sysprintf("1 PlayVideoName_Message fail\r\n");
					}
				}
				else if(VIDEO_FILE_LIST.diskMode == SD_MODE)
				{
					memset(audioFilePath, 0, sizeof(audioFilePath));
					ptr =(char *)VIDEO_FILE_LIST.sd_audio_name[VIDEO_FILE_LIST.sd_audioFile_curPos];
					num = GUI_UC_ConvertUTF82UC((const char *)ptr, strlen(ptr), (U16 * )audioFilePath, 80/2);
					sysprintf("SD num:%d [0]:%x [1]:%x [2]:%x [3]:%x [4]:%x [5]:%x [6]:%x [7]:%x [8]:%x [9]:%x [10]:%x [11]:%x [12]:%x [13]:%x [14]:%x\r\n", \
					num, p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14]);
					if (xQueueSend(PlayVideoName_Message,audioFilePath,strlen(audioFilePath) + 1) == errQUEUE_FULL)
					{
						 sysprintf("2 PlayVideoName_Message fail\r\n");
					}
				}
			}
			else if(videoPlayStatus == PLAY_CTRL_SPEED)
			{
			//		mflPlayControl(&_tMvCfg,PLAY_CTRL_STOP,1);
					//UpdateMusicPlayStatus(PLAY_CTRL_STOP, TRUE);
					sysprintf("MP4 PLAY_CTRL_STOP\r\n");
			}
			break;
		case BUTTON_RETURN:
		//	mflPlayControl(&_tMvCfg,PLAY_CTRL_STOP,1);
			if(videoPlayStatus == PLAY_CTRL_STOP)
			{
				vTaskResume(Task2LCDTest_Handler);
				vTaskResume(Task3PlayMusic_Handler);
				vTaskResume(Task3PlayMusicContrl_Handler);
				sysprintf("MP4 PLAY_CTRL_STOP\r\n");
//				sysStopTimer(TIMER0);
				DrawMenu(cur_item);
				mainMenuIndex = MENU_IDLE;
			}
			break;
		}
}

void Task4_Play_Video_Contrl(void *p_arg)
{
	static unsigned int key=0;
	
	sysprintf("Task4_Play_Video_Contrl task creation\r\n"); 
	while(1)
	{
		if(PlayVideoMenu_Key_Queue != NULL)
		{
			if(xQueueReceive(PlayVideoMenu_Key_Queue,&key,portMAX_DELAY))
			{
				MP4_Dealkey(key);
			}
		}
		vTaskDelay(50);
	}
}

void Task_Play_Video(void *p_arg)
{
	static char RxFileName[MESSAGE_FILE_NAME_LEN];

	sysprintf("Task_Play_Video task creation\r\n"); 
	while(1)
	{
		if(xQueueReceive(PlayVideoName_Message,RxFileName,portMAX_DELAY))
		{
			sysprintf("recive Task_Play_Video message\r\n");
			videoPlayStatus = PLAY_CTRL_SPEED;
			UpdateMusicPlayStatus(musicPlayStatus, TRUE);
			startPlayMP4(RxFileName);
	
			videoPlayStatus = PLAY_CTRL_STOP;
			UpdateMusicPlayStatus(musicPlayStatus, TRUE);
			vTaskDelay(50);
		}
	}
}