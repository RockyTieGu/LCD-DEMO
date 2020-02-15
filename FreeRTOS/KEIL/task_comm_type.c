#include "task_comm_type.h"
#include "wb_include.h"
#include "dx_lcdconfig.h"
#include "xbf_font.h"
#include "nvtfat.h"
#include "stdio.h"
#include "string.h"
#include "GUI.h"

#define	TOTAL_EXT1			(4)
#define	TOTAL_VIDEO_INDEX	(2)
#define	TOTAL_AUDIO_TYPE	(2)

const char* IMAGE_FILE1[TOTAL_EXT1] = {"JPG","jpg","JPEG","jpeg"};
const char* VIDEO_FILE[TOTAL_VIDEO_INDEX] = {"AVI","avi"};
const char* AUDIO_FILE[TOTAL_VIDEO_INDEX] = {"mp3","MP3"};

volatile FILE_LIST AUDIO_FILE_LIST;
volatile FILE_LIST VIDEO_FILE_LIST;

volatile HMI_MENU  mainMenuIndex = MENU_IDLE;

void MP3Control_Init(void)
{	
	gpio_setportdir(GPIO_PORTB, BIT4, BIT4);		//GPIOB4 output mode
	gpio_setportval(GPIO_PORTB, BIT4, 0);        	//GPIOB4 high to enable Amplifier 
	gpio_setportpull(GPIO_PORTB, BIT4, BIT4);		//GPIOB4 pull high
	
}
void MP3Control_En(void)
{
	gpio_setportval(GPIO_PORTB, BIT4, BIT4);        	//GPIOB4 high to enable Amplifier 
}

void MP3Control_Dis(void)
{
	gpio_setportval(GPIO_PORTB, BIT4, 0);        	//GPIOB4 high to enable Amplifier 
}


/*查找文件名后缀*/
BOOL isExtPhotoFile(char *fname)
{
	int i,len;
	char ext[4];

	memset(ext,0,4);
	len = strlen(fname);
	for(i=len-1;i>0;i--){
		if(fname[i-1]=='.')break;
		}
	if(i == 1)
		return FALSE;
	strcpy(ext,fname+i);
	for(i=0;i<TOTAL_EXT1;i++){
		if(strcmp(ext,IMAGE_FILE1[i])==0)return TRUE;
		}
	return FALSE;
}


/*查找视频文件后缀为 avi的文件*/
BOOL isExtVideoFile(char *fname)
{
	int i,len;
	char ext[4];

	memset(ext,0,4);
	len = strlen(fname);
	for(i=len-1;i>0;i--)
	{
		if(fname[i-1]=='.')break;
	}
	if(i == 1)
		return FALSE;		
	strcpy(ext,fname+i);
	for(i=0;i<TOTAL_VIDEO_INDEX;i++)
	{
		if(strcmp(ext,VIDEO_FILE[i])==0)
			return TRUE;
	}
	
	return FALSE;
}

/*查找音频文件后缀为 mp3的文件*/
BOOL isExtAudioFile(char *fname)
{
	int i,len;
	char ext[4];

	memset(ext,0,4);
	len = strlen(fname);
	for(i=len-1;i>0;i--)
	{
		if(fname[i-1]=='.')break;
	}
	if(i == 1)
		return FALSE;		
	strcpy(ext,fname+i);
	for(i=0;i<TOTAL_AUDIO_TYPE;i++)
	{
		if(strcmp(ext,AUDIO_FILE[i])==0)
			return TRUE;
	}
	
	return FALSE;
}

/*搜索指定目录下的特定文件，非递归查找*/
int  SearcheSpecifiedDirectory_filePath(char *suDirName)
{
	int				nStatus;
	char			*pcPtr;
	char			szLongName[MAX_FILE_NAME_LEN/2];
	char			uniDirName[MAX_FILE_NAME_LEN/2];
	char			SaveName[MAX_FILE_NAME_LEN/2];
	FILE_FIND_T  	tFileInfo;
	char 			DiskDriveNumber[2] = {'D','X'}; //存储介质的磁盘号 D:NandFlash X:SD卡
	char			DriveNumber;					// 0:NandFlash 1:SD卡
	char			*p= NULL;
	char			*p1= NULL;
	char			*p2= NULL;
	
	if(suDirName == NULL)
	{
		sysprintf("SearcheSpecifiedDirectory_filePath set dirFile is empty!\n");
		return -1;
	}
	if(suDirName[0] == DiskDriveNumber[0])	DriveNumber = 0;
	else if(suDirName[0] == DiskDriveNumber[1]) DriveNumber = 1;
	else	return -1;
	memset(szLongName, 0, sizeof(szLongName));
	memset(uniDirName, 0, sizeof(uniDirName));
	memset(SaveName, 0, sizeof(SaveName));
	memset((UINT8 *)&tFileInfo, 0, sizeof(tFileInfo));	
	fsAsciiToUnicode(suDirName, uniDirName, TRUE);
	nStatus = fsFindFirst(uniDirName, NULL, &tFileInfo);
	if (nStatus < 0)
	{
		sysprintf("fail fsFindFirst errorCode:%d\n", nStatus);
		return nStatus;
	}
	sysprintf("SearcheSpecifiedDirectory_filePath start!\r\n");
	do 
	{
		pcPtr = tFileInfo.szShortName;
		if ((tFileInfo.ucAttrib & A_DIR) && 
			(!strcmp(pcPtr, ".") || !strcmp(pcPtr, "..")))
			strcat(tFileInfo.szShortName, ".");
		memset(szLongName, 0, sizeof(szLongName));
	//	strcpy(szLongName, tFileInfo.suLongName);
		fsUnicodeToAscii(tFileInfo.suLongName, szLongName, 1);
#if 0
		sysprintf("NAND===%s  len:%d===\n",szLongName, strlen(szLongName));
		p = szLongName;
		sysprintf("%d ,[0]:%x [1]:%x [2]:%x [3]:%x [4]:%x [5]:%x [6]:%x [7]:%x [8]:%x [9]:%x [10]:%x [11]:%x [12]:%x [13]:%x [14]:%x\r\n", AUDIO_FILE_LIST.nandflash_audio_file_num,\
		p[0],p[1],p[2],p[3],p[4],p[5],p[6],p[7],p[8],p[9],p[10],p[11],p[12],p[13],p[14]);
		p1 = tFileInfo.suLongName;
		sysprintf("=%d ,[0]:%x [1]:%x [2]:%x [3]:%x [4]:%x [5]:%x [6]:%x [7]:%x [8]:%x [9]:%x [10]:%x [11]:%x [12]:%x [13]:%x [14]:%x\r\n", AUDIO_FILE_LIST.nandflash_audio_file_num,\
		p1[0],p1[1],p1[2],p1[3],p1[4],p1[5],p1[6],p1[7],p1[8],p1[9],p1[10],p1[11],p1[12],p1[13],p1[14]);
#else
		if(DriveNumber == 0)
			nStatus = AUDIO_FILE_LIST.nandflash_audio_file_num;
		else
			nStatus = AUDIO_FILE_LIST.sd_audio_file_num;
		p1 = tFileInfo.suLongName;
		sysprintf("=%d ,[0]:%x [1]:%x [2]:%x [3]:%x [4]:%x [5]:%x [6]:%x [7]:%x [8]:%x [9]:%x [10]:%x [11]:%x [12]:%x [13]:%x [14]:%x\r\n", nStatus,\
			p1[0],p1[1],p1[2],p1[3],p1[4],p1[5],p1[6],p1[7],p1[8],p1[9],p1[10],p1[11],p1[12],p1[13],p1[14]);
#endif	
		if (tFileInfo.ucAttrib & A_DIR) //目录
		{
			sysprintf("find DIR\r\n");
		}
		else	//文件
		{
			sysprintf("NAND Name:%s\n", szLongName);
			if(isExtAudioFile(szLongName) == TRUE)	//查找符合条件的音乐文件
			{
				if(DriveNumber == 0)// 从NandFlash D盘
				{	
					strcpy((char *)AUDIO_FILE_LIST.nandflash_audio_name[AUDIO_FILE_LIST.nandflash_audio_file_num], unicode_to_utf8(tFileInfo.suLongName));
					AUDIO_FILE_LIST.nandflash_audio_file_num++;
				}else if(DriveNumber == 1)// 从SD卡 X盘 //tFileInfo.suLongName   szLongName
				{
					strcpy((char *)AUDIO_FILE_LIST.sd_audio_name[AUDIO_FILE_LIST.sd_audio_file_num], unicode_to_utf8(tFileInfo.suLongName));
					AUDIO_FILE_LIST.sd_audio_file_num++;
				}	
				sysprintf("find mp3 file\r\n");
			}else if(isExtVideoFile(szLongName) == TRUE)//查找符合条件的视频文件
			{
				if(DriveNumber == 0)// 从NandFlash D盘
				{	
					strcpy((char *)VIDEO_FILE_LIST.nandflash_audio_name[VIDEO_FILE_LIST.nandflash_audio_file_num], unicode_to_utf8(tFileInfo.suLongName));
					VIDEO_FILE_LIST.nandflash_audio_file_num++;
				}else if(DriveNumber == 1)// 从SD卡 X盘 //tFileInfo.suLongName   szLongName
				{
					strcpy((char *)VIDEO_FILE_LIST.sd_audio_name[VIDEO_FILE_LIST.sd_audio_file_num], unicode_to_utf8(tFileInfo.suLongName));
					VIDEO_FILE_LIST.sd_audio_file_num++;
				}	
				sysprintf("find avi file\r\n");
			}
		}		 
	} while (!fsFindNext(&tFileInfo));
	
	fsFindClose(&tFileInfo);
	sysprintf("SearcheSpecifiedDirectory_filePath finished!\r\n");
	
	return 0;
}



//选择音频文件
void Select_File(FILE_LIST *typeFileList )
{
	char i;
	char max_list_num = (_LCD_HEIGHT -_LCD_HEIGHT*2/5)/24-1;  //计算显示区有显示多少行音频文件（24号字体）11
	static SELECT_DISK lastSelectMode = IDLE_MODE;
	short sd_tempIndex,nandflash_tempIndex;
	SELECT_DISK selectMode;
	short index;
	
	selectMode = typeFileList->diskMode;
	if(selectMode == NANDFLASH_MODE) index = typeFileList->nandflash_audioFile_curPos;
	else if(selectMode == SD_MODE) index = typeFileList->sd_audioFile_curPos;
	
	sysprintf("Select_AudioFile index:%d mode:%d\r\n", index,selectMode);
	if (selectMode == IDLE_MODE)
	{
		GUI_SetBkColor(GUI_BLUE);								//设置背景颜色 GUI_LIGHTBLUE
		GUI_ClearRect(0, _LCD_HEIGHT/5 + 1, _LCD_WIDTH/2 - 1, _LCD_HEIGHT*4/5 - 1);	 //清除Nanflash音频文件显示区域
		GUI_ClearRect(_LCD_WIDTH/2+1, _LCD_HEIGHT/5 + 1, _LCD_WIDTH, _LCD_HEIGHT*4/5-1);//清除SD卡音频文件显示区域
		GUI_SetColor(GUI_WHITE);
	//	GUI_SetFont(GUI_FONT_24B_ASCII);
		GUI_UC_SetEncodeUTF8(); 
		SetFont_Xbf();
		if(typeFileList->nandflash_audio_file_num == 0)
			GUI_DispStringHCenterAt("No audio file found!",_LCD_WIDTH/4 ,_LCD_HEIGHT/2);
		if(typeFileList->sd_audio_file_num == 0)
			GUI_DispStringHCenterAt("No audio file found!",_LCD_WIDTH*3/4 ,_LCD_HEIGHT/2);
		if((typeFileList->nandflash_audio_file_num == 0) ||\
			(index > typeFileList->nandflash_audio_file_num))
		{
			sysprintf("Fail Select_AudioFile index:%d nandflash_audio_file_num:%d\r\n", index, typeFileList->nandflash_audio_file_num);
			//return ;
		}
		if((typeFileList->sd_audio_file_num == 0) ||\
			(index > typeFileList->sd_audio_file_num))
		{
			sysprintf("Fail Select_AudioFile index:%d sd_audio_file_num:%d\r\n", index, typeFileList->sd_audio_file_num);
			//return ;
		}
		
		
		//NANDFLASH音频文件显示
		if((typeFileList->nandflash_audio_file_num <= max_list_num) || (typeFileList->nandflash_audioFile_curPos  < max_list_num))
		{
			for(i = 0; i < typeFileList->nandflash_audio_file_num;i++)
			{
				if(i < max_list_num)
				{
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt(typeFileList->nandflash_audio_name[i],10,_LCD_HEIGHT/5 + 1 + 24*i);
				}
			}
		}
		else
		{
			if(typeFileList->nandflash_audioFile_curPos > max_list_num-1)
				nandflash_tempIndex = typeFileList->nandflash_audioFile_curPos - max_list_num+1;
			for(i = nandflash_tempIndex; i < (nandflash_tempIndex + max_list_num);i++)
			{
				if(i < typeFileList->nandflash_audio_file_num)
				{
					GUI_SetColor(GUI_WHITE);//  AUDIO_FILE_LIST.nandflash_audio_name[i]
					GUI_DispStringAt(typeFileList->nandflash_audio_name[i],10,_LCD_HEIGHT/5 + 1 + 24*(i - nandflash_tempIndex));

				}
			}
		}
		
		//SD卡音频文件显示
		if((typeFileList->sd_audio_file_num <= max_list_num) || (typeFileList->sd_audioFile_curPos  < max_list_num))
		{
			for(i = 0; i < typeFileList->sd_audio_file_num;i++)
			{
				if(i < max_list_num)
				{
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt(typeFileList->sd_audio_name[i],_LCD_WIDTH/2 + 10,_LCD_HEIGHT/5 + 1 + 24*i);
				}
			}
		}
		else
		{
			if(typeFileList->sd_audioFile_curPos > max_list_num-1)
				sd_tempIndex = typeFileList->sd_audioFile_curPos - max_list_num+1;

			for(i = sd_tempIndex; i < (sd_tempIndex +max_list_num) ;i++)
			{
				if(i < typeFileList->sd_audio_file_num)
				{	
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt(typeFileList->sd_audio_name[i],_LCD_WIDTH/2 + 10,_LCD_HEIGHT/5 + 1 + 24*(i - sd_tempIndex));
				}
			}
		}
	}
	else 
	{
		if(selectMode == NANDFLASH_MODE)
		{
			GUI_SetBkColor(GUI_LIGHTBLUE);
			GUI_SetColor(GUI_WHITE);
	//		GUI_SetFont(GUI_FONT_24B_ASCII);
			GUI_UC_SetEncodeUTF8(); 
			SetFont_Xbf();
			GUI_ClearRect(0, _LCD_HEIGHT/5 + 1, _LCD_WIDTH/2 - 1, _LCD_HEIGHT*4/5 - 1);	 			//清除Nanflash音频文件显示区域
			
			if(typeFileList->nandflash_audio_file_num == 0)
			GUI_DispStringHCenterAt("No  file found!",_LCD_WIDTH/4 ,_LCD_HEIGHT/2);
			if((typeFileList->nandflash_audio_file_num == 0) ||\
				(index > typeFileList->nandflash_audio_file_num))
			{
				sysprintf("Fail Select_AudioFile index:%d nandflash_audio_file_num:%d\r\n", index, typeFileList->nandflash_audio_file_num);
				return ;
			}
			nandflash_tempIndex = typeFileList->nandflash_audioFile_curPos;
			if((typeFileList->nandflash_audio_file_num <= max_list_num) || (nandflash_tempIndex < max_list_num))
			{
				for(i = 0; i < typeFileList->nandflash_audio_file_num;i++)
				{
					if(i < max_list_num)
					{
						GUI_SetColor((index != i) ? GUI_WHITE : GUI_RED);//unicode_to_utf8
						GUI_DispStringAt(typeFileList->nandflash_audio_name[i],10,_LCD_HEIGHT/5 + 1 + 24*i);
					}
				}
			}
			else
			{
				nandflash_tempIndex = (index > max_list_num-1) ? (index - max_list_num+1) :  nandflash_tempIndex;
				for(i = nandflash_tempIndex; i < (nandflash_tempIndex + max_list_num);i++)
				{
					GUI_SetColor((index != i) ? GUI_WHITE : GUI_RED);
					GUI_DispStringAt(typeFileList->nandflash_audio_name[i],10,_LCD_HEIGHT/5 + 1 + 24*(i - nandflash_tempIndex));
				}
			}
			
			GUI_SetBkColor(GUI_BLUE);
			GUI_SetColor(GUI_WHITE);
//			GUI_SetFont(GUI_FONT_24B_ASCII);
			GUI_ClearRect(_LCD_WIDTH/2+1, _LCD_HEIGHT/5 + 1, _LCD_WIDTH, _LCD_HEIGHT*4/5-1);//清除SD卡音频文件显示区域
			GUI_ClearRect(_LCD_WIDTH/4, _LCD_HEIGHT*4/5 + 1, _LCD_WIDTH*3/4, _LCD_HEIGHT*4/5 + 24);	 //清除正在播放音乐背景
			if(typeFileList->sd_audio_file_num == 0)
				GUI_DispStringHCenterAt("No audio  found!",_LCD_WIDTH*3/4 ,_LCD_HEIGHT/2);
			if(typeFileList->sd_audio_file_num == 0)
			{
				sysprintf("Fail Select_AudioFile index:%d sd_audio_file_num:%d\r\n", index, typeFileList->sd_audio_file_num);
				return ;
			}
			sd_tempIndex = typeFileList->sd_audioFile_curPos;
			if((typeFileList->sd_audio_file_num <= max_list_num) || (sd_tempIndex < max_list_num))
			{
				for(i = 0; i < typeFileList->sd_audio_file_num;i++)
				{
					if(i < max_list_num)
					{
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt(typeFileList->sd_audio_name[i],_LCD_WIDTH/2 + 10,_LCD_HEIGHT/5 + 1 + 24*i);
					}
				}
			}
			else
			{
				sd_tempIndex = (sd_tempIndex > max_list_num-1) ? (sd_tempIndex - max_list_num+1) : sd_tempIndex;
				for(i = sd_tempIndex; i < (sd_tempIndex +max_list_num);i++)
				{
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt(typeFileList->sd_audio_name[i],_LCD_WIDTH/2 + 10,_LCD_HEIGHT/5 + 1 + 24*(i - sd_tempIndex));
				}
			}
			/*显示当前音乐名*/
			if(typeFileList->nandflash_audio_file_num)
			{
				GUI_SetBkColor(GUI_BLUE);
				GUI_DispStringHCenterAt(typeFileList->nandflash_audio_name[typeFileList->nandflash_audioFile_curPos], _LCD_WIDTH/2 , _LCD_HEIGHT*4/5 + 1);
			}
			
		}
		else if(selectMode == SD_MODE)
		{
			GUI_SetBkColor(GUI_BLUE);
			GUI_SetColor(GUI_WHITE);
//			GUI_SetFont(GUI_FONT_24B_ASCII);
			GUI_UC_SetEncodeUTF8(); 
			SetFont_Xbf();
			GUI_ClearRect(0, _LCD_HEIGHT/5 + 1, _LCD_WIDTH/2 - 1, _LCD_HEIGHT*4/5 - 1);	 //清除Nanflash音频文件显示区域
			GUI_ClearRect(_LCD_WIDTH/4, _LCD_HEIGHT*4/5 + 1, _LCD_WIDTH*3/4, _LCD_HEIGHT*4/5 + 24);	 //清除正在播放音乐背景
			if(typeFileList->nandflash_audio_file_num == 0)
			GUI_DispStringHCenterAt("No  file found!",_LCD_WIDTH/4 ,_LCD_HEIGHT/2);
			if(typeFileList->nandflash_audio_file_num == 0)
			{
				sysprintf("Fail Select_AudioFile index:%d nandflash_audio_file_num:%d\r\n", index, typeFileList->nandflash_audio_file_num);
				return ;
			}
			
			nandflash_tempIndex = typeFileList->nandflash_audioFile_curPos;
			if((typeFileList->nandflash_audio_file_num <= max_list_num) || (nandflash_tempIndex < max_list_num))
			{
				for(i = 0; i < typeFileList->nandflash_audio_file_num;i++)
				{
					if(i < max_list_num)
					{
						GUI_SetColor(GUI_WHITE);
						GUI_DispStringAt(typeFileList->nandflash_audio_name[i],10,_LCD_HEIGHT/5 + 1 + 24*i);
					}
				}
			}
			else
			{
				nandflash_tempIndex = (nandflash_tempIndex > max_list_num-1) ? (nandflash_tempIndex - max_list_num+1) : nandflash_tempIndex;
				for(i = nandflash_tempIndex; i < (nandflash_tempIndex +max_list_num);i++)
				{
					GUI_SetColor(GUI_WHITE);
					GUI_DispStringAt(typeFileList->nandflash_audio_name[i],10,_LCD_HEIGHT/5 + 1 + 24*(i - nandflash_tempIndex));
				}
			}
			
			GUI_SetBkColor(GUI_LIGHTBLUE);//GUI_BLUE
			GUI_SetColor(GUI_WHITE);
			//GUI_SetFont(GUI_FONT_24B_ASCII);
			GUI_ClearRect(_LCD_WIDTH/2+1, _LCD_HEIGHT/5 + 1, _LCD_WIDTH, _LCD_HEIGHT*4/5-1);//清除SD卡音频文件显示区域
			if(typeFileList->sd_audio_file_num == 0)
				GUI_DispStringHCenterAt("No  file found!",_LCD_WIDTH*3/4 ,_LCD_HEIGHT/2);
			if((typeFileList->sd_audio_file_num == 0) ||\
				(index > typeFileList->sd_audio_file_num))
			{
				sysprintf("Fail Select_AudioFile index:%d sd_audio_file_num:%d\r\n", index, typeFileList->sd_audio_file_num);
				return ;
			}
			if((typeFileList->sd_audio_file_num <= max_list_num) || (index < max_list_num))
			{
				for(i = 0; i < typeFileList->sd_audio_file_num;i++)
				{
					if(i < max_list_num)
					{
						GUI_SetColor((index != i) ? GUI_WHITE : GUI_RED);
						GUI_DispStringAt(typeFileList->sd_audio_name[i],_LCD_WIDTH/2 + 10,_LCD_HEIGHT/5 + 1 + 24*i);
					}
				}
			}
			else
			{
				sd_tempIndex = (index > max_list_num-1) ? (index - max_list_num+1) :  index;
				for(i = sd_tempIndex; i < (sd_tempIndex +max_list_num) ;i++)
				{
					if(i < typeFileList->sd_audio_file_num)
					{
						GUI_SetColor((index != i) ? GUI_WHITE : GUI_RED);
						GUI_DispStringAt(typeFileList->sd_audio_name[i],_LCD_WIDTH/2 + 10,_LCD_HEIGHT/5 + 1 + 24*(i - sd_tempIndex));
					}
				}
			}
					/*显示当前音乐名*/
			if(typeFileList->sd_audio_file_num)
			{
				GUI_SetBkColor(GUI_BLUE);
				GUI_DispStringHCenterAt(typeFileList->sd_audio_name[typeFileList->sd_audioFile_curPos], _LCD_WIDTH/2 , _LCD_HEIGHT*4/5 + 1);
			}
			
		}
	}
}


