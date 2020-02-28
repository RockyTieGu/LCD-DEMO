#include "stdio.h"
#include "string.h"
#include "task_play_music_app.h"
#include "w55fa93_vpost.h"
#include "nvtfat.h"
#include "xbf_font.h"
#include "spu.h"
#include "graph.h"
#include "GUI.h"

volatile char nAudioPlayVolume = 31;
static UINT32 u32FragSize;
static UINT16 u16IntCount = 2;
static MV_CFG_T	_tMvCfg;			//音乐文件配置信息	
volatile PLAY_CTRL_E musicPlayStatus = PLAY_CTRL_STOP;

static void musicPlay_BackGround(void)
{
	
	GUI_SetBkColor(GUI_BLUE);
	GUI_Clear();
	GUI_SetFont(GUI_FONT_20B_ASCII); //GUI_Font32B_1   GUI_FONT_32B_ASCII
	GUI_SetColor(GUI_WHITE);
	GUI_DrawLine(0,_LCD_HEIGHT/5, _LCD_WIDTH, _LCD_HEIGHT/5);
	GUI_DrawLine(0,_LCD_HEIGHT*4/5, _LCD_WIDTH, _LCD_HEIGHT*4/5);
	GUI_DrawLine(_LCD_WIDTH/2,0, _LCD_WIDTH/2, _LCD_HEIGHT*4/5);
//GUI_FillRect(0,_LCD_HEIGHT/5, _LCD_WIDTH, _LCD_HEIGHT/5);
	GUI_SetColor(GUI_WHITE);
	GUI_DispStringHCenterAt("NAND FLASH D:", _LCD_WIDTH/4 , _LCD_HEIGHT/5/3);
	GUI_DispStringHCenterAt("MINI SD X:", _LCD_WIDTH*3/4 , _LCD_HEIGHT/5/3);
	GUI_SetFont(GUI_FONT_24B_ASCII);
	GUI_DispStringHCenterAt("Music play", _LCD_WIDTH/2 , _LCD_HEIGHT*4/5 + 1);
	GUI_UC_SetEncodeUTF8(); 
	SetFont_Xbf();
}

void Audio_AdjustVolume(void)
{
	char volumBuff[5] = {0};
	
	GUI_SetBkColor(GUI_BLUE);								//设置背景颜色 GUI_LIGHTBLUE
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_16_ASCII);//GUI_FONT_24B_ASCII
	GUI_ClearRect(0, _LCD_HEIGHT*4/5 + 1, _LCD_WIDTH/4 - 1, _LCD_HEIGHT);	 //清除Nanflash音频文件显示区域
	sprintf(volumBuff, "Volum:%d", nAudioPlayVolume);
	GUI_DispStringHCenterAt(volumBuff, _LCD_WIDTH/8 , _LCD_HEIGHT*4/5 + _LCD_HEIGHT/10);
}

void UpdateMusicPlayStatus(PLAY_CTRL_E status, BOOL updateFlag)
{
	GUI_SetBkColor(GUI_BLUE);								//设置背景颜色 GUI_LIGHTBLUE
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_16_ASCII);//GUI_FONT_24B_ASCII
	
	taskENTER_CRITICAL();
	if(updateFlag == TRUE)
		GUI_ClearRect(_LCD_WIDTH/4, _LCD_HEIGHT*4/5 + 24, _LCD_WIDTH*3/4, _LCD_HEIGHT);//清除SD卡音频文件显示区域
	
	if(status == PLAY_CTRL_PAUSE)
		GUI_DispStringHCenterAt("Play  PAUSE\r\n",_LCD_WIDTH/2,_LCD_HEIGHT*4/5 + _LCD_HEIGHT/10);
	else if(status == PLAY_CTRL_RESUME)
		GUI_DispStringHCenterAt("Play  RESUME\r\n",_LCD_WIDTH/2,_LCD_HEIGHT*4/5 + _LCD_HEIGHT/10);
	else if(status == PLAY_CTRL_STOP)
		GUI_DispStringHCenterAt("Play  STOP\r\n",_LCD_WIDTH/2,_LCD_HEIGHT*4/5 + _LCD_HEIGHT/10);
	else if(status == PLAY_CTRL_SPEED)
		GUI_DispStringHCenterAt("Play  START\r\n",_LCD_WIDTH/2,_LCD_HEIGHT*4/5 + _LCD_HEIGHT/10);
	taskEXIT_CRITICAL() ;
}

void  mp3_play_callback(MV_CFG_T *ptMvCfg)
{
	MV_INFO_T 	*ptMvInfo;
	static INT	last_time = 0;
	char buffer[50];//_LCD_HEIGHT/10
	GUI_RECT Rect = {_LCD_WIDTH*3/4,_LCD_HEIGHT*4/5 + 16,_LCD_WIDTH,_LCD_HEIGHT};
	
	mflGetMovieInfo(ptMvCfg, &ptMvInfo);

	if ((sysGetTicks(TIMER0) - last_time > 500) &&
		ptMvInfo->uAuTotalFrames)
	{
		memset(buffer, 0 , sizeof(buffer));
//		sprintf(buffer,"T=%d, Progress = %02d:%02d / %02d:%02d\n", 
//					sysGetTicks(TIMER0) / 100,
//					ptMvInfo->uPlayCurTimePos / 6000, (ptMvInfo->uPlayCurTimePos / 100) % 60,
//					ptMvInfo->uMovieLength / 6000, (ptMvInfo->uMovieLength / 100) % 60);
		sprintf(buffer,"%02d:%02d / %02d:%02d\n", 
					ptMvInfo->uPlayCurTimePos / 6000, (ptMvInfo->uPlayCurTimePos / 100) % 60,
					ptMvInfo->uMovieLength / 6000, (ptMvInfo->uMovieLength / 100) % 60);
		last_time = sysGetTicks(TIMER0);
	//	GUI_FillRectEx(&Rect);
		GUI_ClearRectEx(&Rect);
		//GUI_DispStringInRectEx(buffer,&Rect,GUI_TA_HCENTER | GUI_TA_VCENTER,strlen(buffer),GUI_ROTATE_0);
	//	GUI_DispStringAt("        ",_LCD_WIDTH*3/4,_LCD_HEIGHT*4/5 + _LCD_HEIGHT/10);
		GUI_DispStringAt(buffer,_LCD_WIDTH*3/4,_LCD_HEIGHT*4/5 + _LCD_HEIGHT/10);
		vTaskDelay(5);
	}
	
}

static void MP3_Dealkey(unsigned short input)
{
	char audioFilePath[MESSAGE_FILE_NAME_LEN];
	char *ptr = NULL;
	char *p = audioFilePath;
	int num;
	
	switch(input)
	{
		case BUTTON_UP:
			if(musicPlayStatus == PLAY_CTRL_STOP)
			{	
				if(AUDIO_FILE_LIST.diskMode == NANDFLASH_MODE)
				{
					if(AUDIO_FILE_LIST.nandflash_audioFile_curPos > 0) AUDIO_FILE_LIST.nandflash_audioFile_curPos--;
					Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
				}
				else if(AUDIO_FILE_LIST.diskMode == SD_MODE)
				{
					if(AUDIO_FILE_LIST.sd_audioFile_curPos > 0) AUDIO_FILE_LIST.sd_audioFile_curPos--;
					Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
				}else if(AUDIO_FILE_LIST.diskMode == IDLE_MODE)
				{
					if(nAudioPlayVolume > 0)nAudioPlayVolume--;
						aviSetPlayVolume(nAudioPlayVolume);
					Audio_AdjustVolume();
				}
			}
			else if(musicPlayStatus == PLAY_CTRL_SPEED)
			{
				mflPlayControl(&_tMvCfg,PLAY_CTRL_PAUSE,1);
				UpdateMusicPlayStatus(PLAY_CTRL_PAUSE, TRUE);
				sysprintf("MP3 PLAY_CTRL_PAUSE\r\n");
			}
			break;
		case BUTTON_DOWN:
			if(musicPlayStatus == PLAY_CTRL_STOP)
			{
				if(AUDIO_FILE_LIST.diskMode == NANDFLASH_MODE)
				{
					if(AUDIO_FILE_LIST.nandflash_audioFile_curPos < AUDIO_FILE_LIST.nandflash_audio_file_num - 1) AUDIO_FILE_LIST.nandflash_audioFile_curPos++;
					Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
				}
				else if(AUDIO_FILE_LIST.diskMode == SD_MODE)
				{
					if(AUDIO_FILE_LIST.sd_audioFile_curPos < AUDIO_FILE_LIST.sd_audio_file_num - 1) AUDIO_FILE_LIST.sd_audioFile_curPos++;
					Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
				}else if(AUDIO_FILE_LIST.diskMode == IDLE_MODE)
				{
					if(nAudioPlayVolume < 30)nAudioPlayVolume++;
					else	nAudioPlayVolume = 31;
					aviSetPlayVolume(nAudioPlayVolume);
					Audio_AdjustVolume();
				}
			}
			else if(musicPlayStatus == PLAY_CTRL_SPEED)
			{
				mflPlayControl(&_tMvCfg,PLAY_CTRL_RESUME,1);
				UpdateMusicPlayStatus(PLAY_CTRL_RESUME, TRUE);
				sysprintf("MP3 PLAY_CTRL_RESUME\r\n");
			}
			break;
		case BUTTON_LOCK:
			if(musicPlayStatus == PLAY_CTRL_STOP)
			{
				if(AUDIO_FILE_LIST.diskMode < SD_MODE)
					AUDIO_FILE_LIST.diskMode++;
				else
					AUDIO_FILE_LIST.diskMode = IDLE_MODE;
				
				if(AUDIO_FILE_LIST.diskMode == NANDFLASH_MODE)
					Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
				else if(AUDIO_FILE_LIST.diskMode == SD_MODE)
					Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
				else
					Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
				sysprintf("press lock button diskMode:%d\n", AUDIO_FILE_LIST.diskMode);	
			}		
			else if(musicPlayStatus == PLAY_CTRL_SPEED)
			{
				//mflPlayControl(&_tMvCfg,PLAY_CTRL_PAUSE,1);
				//sysprintf("MP3 PLAY_CTRL_PAUSE\r\n");
			}
			break;
		case BUTTON_OK:
			if(musicPlayStatus == PLAY_CTRL_STOP)
			{
				if(AUDIO_FILE_LIST.diskMode == NANDFLASH_MODE)
				{
					memset(audioFilePath, 0, sizeof(audioFilePath));
					//sprintf(audioFilePath, "%s%s", AUDIO_FILE_NANDFLASH_PATH, AUDIO_FILE_LIST.nandflash_audio_name[AUDIO_FILE_LIST.nandflash_audioFile_curPos]);
					//sysprintf("play music nandflash:%s\r\n",AUDIO_FILE_LIST.nandflash_audio_name[AUDIO_FILE_LIST.nandflash_audioFile_curPos]);
					ptr = (char *)AUDIO_FILE_LIST.nandflash_audio_name[AUDIO_FILE_LIST.nandflash_audioFile_curPos];
					num = GUI_UC_ConvertUTF82UC((const char *)ptr, strlen(ptr), (U16 * )audioFilePath, 80/2);
					sysprintf("NandFlash num:%d [0]:%x [1]:%x [2]:%x [3]:%x [4]:%x [5]:%x [6]:%x [7]:%x [8]:%x [9]:%x [10]:%x [11]:%x [12]:%x [13]:%x [14]:%x\r\n", \
					num, p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14]);
					if (xQueueSend(PlayMusicName_Message,audioFilePath,strlen(audioFilePath) + 1) == errQUEUE_FULL)
					{
						 sysprintf("1 PlayMusicName_Message fail\r\n");
					}
				}
				else if(AUDIO_FILE_LIST.diskMode == SD_MODE)
				{
					memset(audioFilePath, 0, sizeof(audioFilePath));
					ptr = (char *)AUDIO_FILE_LIST.sd_audio_name[AUDIO_FILE_LIST.sd_audioFile_curPos];
					num = GUI_UC_ConvertUTF82UC((const char *)ptr, strlen(ptr), (U16 * )audioFilePath, 80/2);
				//	sysprintf("SD num:%d [0]:%x [1]:%x [2]:%x [3]:%x [4]:%x [5]:%x [6]:%x [7]:%x [8]:%x [9]:%x [10]:%x [11]:%x [12]:%x [13]:%x [14]:%x\r\n", \
					num, p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14]);
					if (xQueueSend(PlayMusicName_Message,audioFilePath,strlen(audioFilePath) + 1) == errQUEUE_FULL)
					{
						 sysprintf("2 PlayMusicName_Message fail\r\n");
					}
				}
			}
			else if(musicPlayStatus == PLAY_CTRL_SPEED)
			{
					mflPlayControl(&_tMvCfg,PLAY_CTRL_STOP,1);
					UpdateMusicPlayStatus(PLAY_CTRL_STOP, TRUE);
					sysprintf("MP3 PLAY_CTRL_STOP\r\n");
			}
			break;
		case BUTTON_RETURN:
			if(musicPlayStatus == PLAY_CTRL_STOP)
			{
				vTaskResume(Task2LCDTest_Handler);
				vTaskResume(Task4PlayVideoContrl_Handler);
				vTaskResume(Task4PlayVideo_Handler);
				
				mflPlayControl(&_tMvCfg,PLAY_CTRL_STOP,1);
				sysprintf("MP3 PLAY_CTRL_STOP\r\n");
//				sysStopTimer(TIMER0);
				DrawMenu((char )cur_item);
				mainMenuIndex = MENU_IDLE;
			}
			break;
		}
}

void loadPlayMusic_Menu(void)
{
	musicPlay_BackGround();
	Select_File((FILE_LIST *)(&AUDIO_FILE_LIST));
	Audio_AdjustVolume();
}


void Task3_Play_Music_Contrl(void *p_arg)
{
	static unsigned int key=0;
	
	sysprintf("%s task creation\r\n", (char *)Task3_Play_Music_Contrl);
	while(1)
	{
		if(PlayMusicMenu_Key_Queue != NULL)
		{
			if(xQueueReceive(PlayMusicMenu_Key_Queue,&key,portMAX_DELAY))
			{
				MP3_Dealkey(key);
			}
		}
		vTaskDelay(50);
	}
}
static void startPlayMP3(char * audioFileName)
{

	int			nStatus;
	char		suFileName[256] ;
	char 		assicFilName[128];

	GUI_SetBkColor(GUI_BLUE);								//设置背景颜色 GUI_LIGHTBLUE
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(GUI_FONT_16_ASCII);//GUI_FONT_24B_ASCII
	memset(suFileName,0,sizeof(suFileName));
	if(AUDIO_FILE_LIST.diskMode == NANDFLASH_MODE)
	{	
		fsAsciiToUnicode(AUDIO_FILE_NANDFLASH_PATH,suFileName, TRUE);
		fsUnicodeStrCat(suFileName, audioFileName);
	}
	else if(AUDIO_FILE_LIST.diskMode == SD_MODE)
	{
		fsAsciiToUnicode(AUDIO_FILE_SD_PATH,suFileName,TRUE);
		fsUnicodeStrCat(suFileName, audioFileName);
	}
	else
	{
		sysprintf("AUDIO_FILE_LIST.diskMode error!\r\n");
		return; 
	}
	_tMvCfg.eInMediaType			= MFL_MEDIA_MP3;
	_tMvCfg.eInStrmType				= MFL_STREAM_FILE;
	_tMvCfg.szIMFAscii				= NULL;
	_tMvCfg.suInMetaFile			= NULL;
	_tMvCfg.szITFAscii				= NULL;
	_tMvCfg.nAudioPlayVolume		= nAudioPlayVolume;
	_tMvCfg.uStartPlaytimePos		= 0;
	_tMvCfg.nAuABRScanFrameCnt		= 50;
	_tMvCfg.ap_time					= mp3_play_callback;
	_tMvCfg.suInMediaFile			= suFileName;
	MP3Control_En();

	if ( (nStatus = mflMediaPlayer(&_tMvCfg)) < 0 )
		sysprintf("Playback failed, code = %x\n", nStatus);
	else
		sysprintf("Playback done.\n");
	
	MP3Control_Dis();
}


void Task_Play_Music(void *p_arg)
{
	static char RxFileName[80];

	sysprintf("Task_Play_Music task creation\r\n");
	while(1)
	{
		if(xQueueReceive(PlayMusicName_Message,RxFileName,portMAX_DELAY))
		{
			sysprintf("recive Task_Play_Music message\r\n");
			musicPlayStatus = PLAY_CTRL_SPEED;
			UpdateMusicPlayStatus(musicPlayStatus, TRUE);
			startPlayMP3(RxFileName);
	
			musicPlayStatus = PLAY_CTRL_STOP;
			UpdateMusicPlayStatus(musicPlayStatus, TRUE);
			vTaskDelay(50);
		}
	}
}

#if 0
int playCallBack1(UINT8 * pu8Buffer)
{	
	UINT32 u32Offset = 0;
	UINT32 len = sizeof(SPU_SOURCE);
	
	u32Offset = ( u32FragSize / 2) * u16IntCount;
	if (u32Offset >= len)		// Reach the end of the song, restart from beginning
	{		
		u16IntCount = 2;
		u32Offset = 0;
		bPlaying = FALSE;
		sysprintf("playCallBack1 end!\n");
		return TRUE;
	}	
	memcpy(pu8Buffer, &SPU_SOURCE[u32Offset], u32FragSize/2);
	sysprintf("u16IntCount:%d\n",u16IntCount);
	u16IntCount++;
	
	return FALSE;
}
#endif

