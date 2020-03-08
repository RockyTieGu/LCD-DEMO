#include "avifile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wb_include.h"
#include "jpegsample.h"
#include "jpegcodec.h"
#include "w55fa93_vpost.h"
#include "DrvSPU.h"
#include "spu.h"
#include "nvtfat.h"
#include "key.h"
#define PrintInfo 1
#if 1
AVI_TypeDef AVI_file;
avih_TypeDef* avihChunk;
strh_TypeDef* strhChunk;
BITMAPINFO* bmpinfo;
WAVEFORMAT* wavinfo;
unsigned int temp=0x00;
unsigned char vids_ID;
unsigned char auds_ID;
unsigned char  Frame_buf[20480]={0x00};
unsigned char  Sound_buf1[80*1024]={0x00};
unsigned char  Sound_buf2[80*1024]={0x00};
unsigned char* Dbuf=Frame_buf;
unsigned int  mid;
unsigned int  Strsize;
unsigned short  Strtype;
unsigned int  Audsize;
unsigned char   buffer_switch=1;
unsigned char   finish=0;
//#define FLASH_VIDEO_ADDR 0
static unsigned char audioBuff[4][1024*80+8];
volatile int curPost = 0;
extern volatile LCD_BUFFER setBufferType;
#define LS_OPEN_SOUND_SWITCH	0
#define	PrintInfo 1
unsigned char AVI_Parser(unsigned char *buffer)
{
	temp=ReadUnit(buffer,0,4,1);//��"RIFF"
	if(temp!=RIFF_ID)return 1;
	AVI_file.RIFFchunksize=ReadUnit(buffer,4,4,1);//RIFF���ݿ鳤��
	temp=ReadUnit(buffer,8,4,1);//��"AVI "
	if(temp!=AVI_ID)return 2;
	temp=ReadUnit(buffer,12,4,1);//��"LIST"
	if(temp!=LIST_ID)return 3;
	AVI_file.LISTchunksize=ReadUnit(buffer,16,4,1);//LIST���ݿ鳤��
	temp=ReadUnit(buffer,20,4,1);//��"hdrl"
	if(temp!=hdrl_ID)return 4;
	temp=ReadUnit(buffer,24,4,1);//��"avih"
	if(temp!=avih_ID)return 5;
	AVI_file.avihsize=ReadUnit(buffer,28,4,1);//avih���ݿ鳤��	
	return 0;				
}

unsigned char Avih_Parser(unsigned char *buffer)
{
	avihChunk=(avih_TypeDef*)buffer;
#ifdef PrintInfo
	sysprintf("\r\navih���ݿ���Ϣ:\n");
	sysprintf("\r\nSecPerFrame:%d",avihChunk->SecPerFrame);
	sysprintf("\r\nMaxByteSec:%d",avihChunk->MaxByteSec);
	sysprintf("\r\nChunkBase:%d",avihChunk->ChunkBase);
	sysprintf("\r\nSpecProp:%d",avihChunk->SpecProp);
	sysprintf("\r\nTotalFrame:%d",avihChunk->TotalFrame);
	sysprintf("\r\nInitFrames:%d",avihChunk->InitFrames);
	sysprintf("\r\nStreams:%d",avihChunk->Streams);
	sysprintf("\r\nRefBufSize:%d",avihChunk->RefBufSize);
	sysprintf("\r\nWidth:%d",avihChunk->Width);
	sysprintf("\r\nHeight:%d",avihChunk->Height);
#endif
//	if((avihChunk->Width>480)||(avihChunk->Height>320))return 1;//��Ƶ�ߴ粻֧��
//	if(avihChunk->Streams!=2)return 2;//��Ƶ������֧��
	return 0;
}

unsigned char Strl_Parser(unsigned char *buffer)
{
	temp=ReadUnit(buffer,0,4,1);//��"LIST"
	if(temp!=LIST_ID)return 1;
	AVI_file.strlsize=ReadUnit(buffer,4,4,1);//strl���ݿ鳤��
	temp=ReadUnit(buffer,8,4,1);//��"strl"
	if(temp!=strl_ID)return 2;
	temp=ReadUnit(buffer,12,4,1);//��"strh"
	if(temp!=strh_ID)return 3;
	AVI_file.strhsize=ReadUnit(buffer,16,4,1);//strh���ݿ鳤��
	strhChunk=(strh_TypeDef*)(buffer+20);		 //108
#ifdef PrintInfo
	sysprintf("\r\nstrh���ݿ���Ϣ:\n");	
	sysprintf("\r\nStreamType:%s",strhChunk->StreamType);
	sysprintf("\r\nHandler:%s",strhChunk->Handler);//��������MJPEG
	sysprintf("\r\nStreamFlag:%d",strhChunk->StreamFlag);
	sysprintf("\r\nPriority:%d",strhChunk->Priority);
	sysprintf("\r\nLanguage:%d",strhChunk->Language);
	sysprintf("\r\nInitFrames:%d",strhChunk->InitFrames);
	sysprintf("\r\nScale:%d",strhChunk->Scale);
	sysprintf("\r\nRate:%d",strhChunk->Rate);
	sysprintf("\r\nStart:%d",strhChunk->Start);
	sysprintf("\r\nLength:%d",strhChunk->Length);
	sysprintf("\r\nRefBufSize:%d",strhChunk->RefBufSize);
	sysprintf("\r\nQuality:%d",strhChunk->Quality);
	sysprintf("\r\nSampleSize:%d",strhChunk->SampleSize);
	sysprintf("\r\nFrameLeft:%d",strhChunk->Frame.Left);
	sysprintf("\r\nFrameTop:%d",strhChunk->Frame.Top);
	sysprintf("\r\nFrameRight:%d",strhChunk->Frame.Right);
	sysprintf("\r\nFrameBottom:%d",strhChunk->Frame.Bottom);
#endif
	if(strhChunk->Handler[0]!='M')return 4;
	return 0;
}

unsigned char Strf_Parser(unsigned char *buffer)
{
	temp=ReadUnit(buffer,0,4,1);//��"strf"
	if(temp!=strf_ID)return 1;
	if(strhChunk->StreamType[0]=='v'){//��һ����Ϊ��Ƶ��
		vids_ID='0';
		auds_ID='1';
		bmpinfo=(BITMAPINFO*)(buffer+8);
		wavinfo=(WAVEFORMAT*)(buffer+4332);
	}
	else if(strhChunk->StreamType[0]=='a'){//��һ����Ϊ��Ƶ��
		vids_ID='1';
		auds_ID='0';
		wavinfo=(WAVEFORMAT*)(buffer+8);
		bmpinfo=(BITMAPINFO*)(buffer+4332);
	}
#ifdef PrintInfo		
	sysprintf("\r\nstrf���ݿ���Ϣ(��Ƶ��):\n");		
	sysprintf("\r\n���ṹ���С:%d",bmpinfo->bmiHeader.Size);
	sysprintf("\r\nͼ���:%d",bmpinfo->bmiHeader.Width);
	sysprintf("\r\nͼ���:%d",bmpinfo->bmiHeader.Height);
	sysprintf("\r\nƽ����:%d",bmpinfo->bmiHeader.Planes);
	sysprintf("\r\n����λ��:%d",bmpinfo->bmiHeader.BitCount);
	sysprintf("\r\nѹ������:%s",bmpinfo->bmiHeader.Compression);
	sysprintf("\r\nͼ���С:%d",bmpinfo->bmiHeader.SizeImage);
	sysprintf("\r\nˮƽ�ֱ���:%d",bmpinfo->bmiHeader.XpixPerMeter);
	sysprintf("\r\n��ֱ�ֱ���:%d",bmpinfo->bmiHeader.YpixPerMeter);
	sysprintf("\r\nʹ�õ�ɫ����ɫ��:%d",bmpinfo->bmiHeader.ClrUsed);
	sysprintf("\r\n��Ҫ��ɫ:%d",bmpinfo->bmiHeader.ClrImportant);

	sysprintf("\r\nstrf���ݿ���Ϣ(��Ƶ��):\n");
	sysprintf("\r\n��ʽ��־:%d",wavinfo->FormatTag);
	sysprintf("\r\n������:%d",wavinfo->Channels);
	sysprintf("\r\n������:%d",wavinfo->SampleRate);
	sysprintf("\r\n������:%d",wavinfo->BaudRate);
	sysprintf("\r\n�����:%d",wavinfo->BlockAlign);
	sysprintf("\r\n���ṹ���С:%d",wavinfo->Size);
#endif
	return 0;
}

unsigned short Search_Movi(unsigned char* buffer)
{
	unsigned short i;
	for(i=0;i<20480;i++){
	   	if(buffer[i]==0x6d)
			if(buffer[i+1]==0x6f)
				if(buffer[i+2]==0x76)	
					if(buffer[i+3]==0x69)return i;//�ҵ�"movi"	
	}
	return 0;		
}

unsigned short Search_Fram(unsigned char* buffer)
{
	unsigned short i;
	for(i=0;i<20480;i++){
	   	if(buffer[i]=='0')
			if(buffer[i+1]==vids_ID)
				if(buffer[i+2]=='d')	
					if(buffer[i+3]=='c')return i;//�ҵ�"xxdc"	
	}
	return 0;		
}

unsigned int ReadUnit(unsigned char *buffer,unsigned char index,unsigned char Bytes,unsigned char Format)
{
  	unsigned char off=0;
  	unsigned int a,b,c,d,unit=0;  
  	for(off=0;off<Bytes;off++)unit|=buffer[off+index]<<(off*8);
  	if(Format){	//���ģʽ
		a = unit&0xff;
		b = (unit>>8)&0xff;
		c = (unit>>16)&0xff;
		d = unit>>24;
		unit = (a<<24)|(b<<16)|(c<<8)|d;
  		}
  	return unit;
}

static volatile UINT16 u16IntCount = 2;
static volatile UINT8 bPlaying = TRUE;
static UINT32 u32FragSize;
static volatile UINT32 file_len;
static  UINT32 len ;

int playCallBack(UINT8 * pu8Buffer)
{	
	volatile UINT32 u32Offset = 0;
	u32Offset = ( u32FragSize / 2) * u16IntCount;
	if(u32Offset >= file_len)
	{
		curPost ++;
		if(curPost > 3) curPost = 0;	
		u16IntCount =2;
		u32Offset = ( u32FragSize / 2) * u16IntCount;
	}
#if LS_OPEN_SOUND_SWITCH	
	memcpy(pu8Buffer, &audioBuff[curPost][u32Offset], u32FragSize/2);
#endif
	sysprintf("playCallBack() curPost:%d u32Offset:%d\r\n",curPost, u32Offset);
	u16IntCount++;

	return FALSE;
}

#if 1
void AVI_play(char *FilePath)
{
	unsigned char  res,saveBuff = 0;
	unsigned int addr;
	unsigned int u32Width,u32Height,u32Format;
	char	suFileName[40];
	int fil;
	int realLen = 0,nWriteLen ;
	unsigned int fileSize;
	UINT32 u32TestChannel, uSamplingRate;	
	UINT32	nr; 
	unsigned char u8SrcFormat;
	int wrFile;

#if LS_OPEN_SOUND_SWITCH
	spuOpen(eDRVSPU_FREQ_44100);
	
	spuIoctl(SPU_IOCTL_SET_VOLUME, 0x3f, 0x3f);
	spuIoctl(SPU_IOCTL_GET_FRAG_SIZE, (UINT32)&u32FragSize, 0);
	
	spuEqOpen(eDRVSPU_EQBAND_2, eDRVSPU_EQGAIN_P7DB);
	spuDacOn(1);
#endif
	//sysDelay(30);
		
	if(!FilePath)
	{
		#ifdef PrintInfo
		sysprintf("video file path is empty!");
		#endif
		return ;
	}
	fil=fsOpenFile(FilePath, NULL, O_RDONLY|O_FSEEK);
	if (fil ==0){
		#ifdef PrintInfo
		sysprintf("fail open %s", FilePath);
		#endif
	}
	fileSize = fsGetFileSize(fil);
	sysprintf(" %s size:%d bytes\n", FilePath, fileSize);
	Dbuf=Frame_buf;
	fsReadFile(fil, (unsigned char *)Dbuf, sizeof(Frame_buf), &realLen);
	sysprintf("read Dbuf %d len", realLen);

	res=AVI_Parser(Dbuf);//����AVI�ļ���ʽ
	if(res)
	{
		#ifdef PrintInfo
		sysprintf("err0 res:%d", res);
		#endif
		return;
	}
	res=Avih_Parser(Dbuf+32);//����avih���ݿ�
	if(res)
	{
		#ifdef PrintInfo
		sysprintf("err1 res:%d", res);
		#endif
		return;
	}
	res=Strl_Parser(Dbuf+88);//����strh���ݿ�
	if(res)
	{
		#ifdef PrintInfo
		sysprintf("err2 res:%d", res);
		#endif
		return;
	}
	res=Strf_Parser(Dbuf+164);//����strf���ݿ�
	if(res)
	{
		#ifdef PrintInfo
		sysprintf("err3 res:%d", res);
		#endif
		return;
	}
	mid=Search_Movi(Dbuf);//Ѱ��movi ID		
	if(!mid)
	{
		#ifdef PrintInfo
		sysprintf("err4 mid:%d", mid);
		#endif
		return;
	}
	Strtype=MAKEWORD(Dbuf+mid+6);//������
	Strsize=MAKEDWORD(Dbuf+mid+8);//����С
	if(Strsize%2)Strsize++;//������1		
	addr =(mid+12);
	finish=0;
	g_pu8JpegBuffer = (unsigned char *)malloc(sizeof(CHAR) * Strsize);
	sysprintf("video Play start!\n");
	setBufferType = VPOST;
#if LS_OPEN_SOUND_SWITCH	
	spuStartPlay((PFN_DRVSPU_CB_FUNC *) playCallBack, (UINT8 *)audioBuff[0]);
#endif
	while(!finish){		
		if(Strtype==T_vids)
		{//��ʾ֡
			fsFileSeek(fil, addr, SEEK_SET);
			fsReadFile(fil, ((unsigned char *)(g_pu8JpegBuffer+0x80000000)), Strsize, &realLen);
			addr +=Strsize;	

			if(ParsingOldJPEG((unsigned char *)((unsigned int)g_pu8JpegBuffer | 0x80000000), Strsize, &u32Width, &u32Height, &u32Format, TRUE) == ERR_MODE)
			{
				sysprintf("\tNot Support the JPEG sampling\n");	
		    	goto END_ERROR_FORMAT;
			}

			/* JPEG Init */
			jpegInit();
			
			/* Set Bit stream Address */   
			jpegIoctl(JPEG_IOCTL_SET_BITSTREAM_ADDR,(unsigned int) g_pu8JpegBuffer| 0x80000000, 0);	
#if (LCD_BITS_MODE == 16)		          
			/* Decode mode */	
			jpegIoctl(JPEG_IOCTL_SET_DECODE_MODE, JPEG_DEC_PRIMARY_PACKET_RGB565, 0);	
#elif ((LCD_BITS_MODE == 18) || (LCD_BITS_MODE == 24)) 
			jpegIoctl(JPEG_IOCTL_SET_DECODE_MODE, JPEG_DEC_PRIMARY_PACKET_RGB888, 0);	
#endif			
			/* Set JPEG Header Decode End Call Back Function */
			jpegIoctl(JPEG_IOCTL_SET_HEADERDECODE_CALBACKFUN, (unsigned int) JpegOldDecHeaderComplete, 0);	
			
			/* Trigger JPEG decoder */
			jpegIoctl(JPEG_IOCTL_DECODE_TRIGGER, 0, 0);  
		        
			/* Wait for complete */
			jpegWait();
	//		sysGetChar();
		}//��ʾ֡
		else if(Strtype==T_auds)
		{		
							
			if(!buffer_switch){	
				fsFileSeek(fil, addr, SEEK_SET);
				sysprintf("\tL auto addr:%d size:%d\n", addr,Strsize);
				fsReadFile(fil, (unsigned char *)Sound_buf2, Strsize+8, &realLen); 	
				
//				fsWriteFile(wrFile, (UINT8 *)Sound_buf2, realLen, &nWriteLen);
//				sysprintf("1.write %d bytes, real write %d bytes\r\n",realLen,nWriteLen );
//				
//				
				Dbuf=Sound_buf2;				
				addr +=Strsize;	   
				u32TestChannel=0;
#if LS_OPEN_SOUND_SWITCH				
				spuStartPlay(NULL, (UINT8 *)Sound_buf2);	 
				DrvSPU_StartPlay();
#endif
			}
			 else { 
#if LS_OPEN_SOUND_SWITCH				 
				 saveBuff++;
				 if(saveBuff > 3)saveBuff = 0;
				 do{
					nr=curPost;
					 if(nr) nr--;
					 else
						 nr = 3;
				 }while(saveBuff == nr);
#endif
				fsFileSeek(fil, addr, SEEK_SET);
				fsReadFile(fil, (unsigned char *)audioBuff[saveBuff], Strsize+8, &realLen); 
			//	Dbuf=Sound_buf1;
				file_len = realLen;
				addr +=Strsize; 
				//sysprintf("main fun() realLen:%d\r\n",realLen);
			} 
			Audsize=Strsize;
		}
		else break;	
		
		if (boardKeyMSG_handle() == BUTTON_OK)
		{
			free(g_pu8JpegBuffer);
			fsCloseFile(fil);
			fsCloseFile(wrFile);
#if LS_OPEN_SOUND_SWITCH	
			spuEqClose();	
			spuClose();
#endif
			sysprintf("Exit video play!\n");
		//	vTaskDelay(5);
			return ;
		}
		
		vTaskDelay(2);
END_ERROR_FORMAT:	
		fsFileSeek(fil, addr, SEEK_SET);
		fsReadFile(fil, (unsigned char *)Dbuf, 8, &realLen);
		//spiFlashRead(addr, 8, (unsigned int)Dbuf);
		addr +=8;	
		Strtype=MAKEWORD(Dbuf+2);//������
		Strsize=MAKEDWORD(Dbuf+4);//����С	
		if(Strsize%2)Strsize++;//������1
		if(addr>=fileSize)finish=1;
	}
	free(g_pu8JpegBuffer);
	fsCloseFile(fil);
	fsCloseFile(wrFile);
#if LS_OPEN_SOUND_SWITCH	
	spuEqClose();	
	spuClose();
#endif
	sysprintf("video Play end!\n");
}
#endif
#endif
