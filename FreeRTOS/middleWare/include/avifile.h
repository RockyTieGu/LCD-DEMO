#ifndef __AVIFILE_H
#define __AVIFILE_H
#include "wbtypes.h"

//#define PrintInfo	//信息打印开关

#define RIFF_ID		0x52494646
#define AVI_ID		0x41564920
#define LIST_ID		0x4c495354
#define hdrl_ID		0x6864726c
#define avih_ID		0x61766968
#define strl_ID		0x7374726c
#define strh_ID		0x73747268
#define strf_ID		0x73747266
#define movi_ID		0x6d6f7669

typedef struct
{	
  	unsigned int  RIFFchunksize;
  	unsigned int  LISTchunksize;
	unsigned int  avihsize;
	unsigned int  strlsize;
	unsigned int  strhsize;
}AVI_TypeDef;

typedef struct
{	
	unsigned int  SecPerFrame;	//显示每帧的时间
	unsigned int  MaxByteSec; 	//最大数据传输率
	unsigned int  ChunkBase; 	//块长度为此值的倍数，通常为2048
	unsigned int  SpecProp;	//AVI文件的特殊属性
	unsigned int  TotalFrame;	//文件总帧数
	unsigned int  InitFrames;  //开始播放前的初始化帧数
	unsigned int  Streams;		//包含的数据流种类
	unsigned int  RefBufSize;  //建议使用的缓冲区大小，通常为存储一帧图像与同步音频数据之和
	unsigned int  Width;		//图像宽
	unsigned int  Height;		//图像高
	unsigned int  Reserved;	//保留
}avih_TypeDef;

typedef struct
{	
	unsigned char     StreamType[4];//数据流种类，vids(0x73646976):视频,auds:音频
	unsigned char     Handler[4];	//数据解压缩的驱动程序代号
	unsigned int  StreamFlag;  //数据流属性
	unsigned short   Priority;	//此数据流播放的优先级
	unsigned short   Language;	//音频的语言代号
	unsigned int  InitFrames;  //开始播放前的初始化帧数
	unsigned int  Scale;		//视频每帧的大小或音频的采样率
	unsigned int  Rate; 		//Scale/Rate=每秒采样数
	unsigned int  Start;		//数据流开始播放的位置，单位为Scale
	unsigned int  Length;		//数据流的数据量，单位为Scale
 	unsigned int  RefBufSize;  //建议使用的缓冲区大小
    	unsigned int  Quality;		//解压缩质量参数，值越大，质量越好
	unsigned int  SampleSize;	//音频的样本大小
	struct{				
	   	short  Left;
		short  Top;
		short  Right;
		short  Bottom;
	}Frame;				//视频帧所占的矩形 
}strh_TypeDef;

typedef struct tagBMPHEADER
{
   	unsigned int  	Size;		//本结构所占用字节数
	int   	Width;		//图像宽
	int   	Height;		//图像高
	unsigned short   	Planes;		//平面数，必须为1
	unsigned short   	BitCount;	//像素位数
	unsigned char  	Compression[4];	//压缩类型，MJPEG
	unsigned int  	SizeImage;	//图像大小
	int   	XpixPerMeter;//水平分辨率
	int   	YpixPerMeter;//垂直分辨率
	unsigned int  	ClrUsed;		//实际使用了调色板中的颜色数,压缩格式中不使用
	unsigned int  	ClrImportant;//重要的颜色
}BMPHEADER;//该结构占用40字节

typedef struct tagRGBQUAD 
{
	unsigned char  rgbBlue;	//蓝色的亮度(值范围为0-255)
	unsigned char  rgbGreen; //绿色的亮度(值范围为0-255)
	unsigned char  rgbRed; 	//红色的亮度(值范围为0-255)
	unsigned char  rgbReserved;//保留，必须为0
}RGBQUAD;//颜色表

typedef struct tagBITMAPINFO 
{
	BMPHEADER  bmiHeader;  //位图信息头
	RGBQUAD    bmColors[1];//颜色表
}BITMAPINFO;  

typedef struct 
{
   	unsigned short  FormatTag;
	unsigned short  Channels;	  //声道数
	unsigned int SampleRate; //采样率
	unsigned int BaudRate;   //波特率
	unsigned short  BlockAlign; //数据块对齐标志
	unsigned short  Size;//该结构大小
}WAVEFORMAT;

#define	 MAKEWORD(ptr)	(unsigned short)(((unsigned short)*((unsigned char*)(ptr))<<8)|(unsigned short)*(unsigned char*)((ptr)+1))
#define  MAKEDWORD(ptr)	(unsigned int)(((unsigned short)*(unsigned char*)(ptr)|(((unsigned short)*(unsigned char*)(ptr+1))<<8)|\
						(((unsigned short)*(unsigned char*)(ptr+2))<<16)|(((unsigned short)*(unsigned char*)(ptr+3))<<24))) 

#define  T_vids	 		0x6463
#define  T_auds	 		0x7762

#define	AVI_LENGTH		1664202

unsigned char AVI_Parser(unsigned char *buffer);
unsigned char Avih_Parser(unsigned char *buffer);
unsigned char Strl_Parser(unsigned char *buffer);
unsigned char Strf_Parser(unsigned char *buffer);
unsigned short Search_Movi(unsigned char* buffer);
unsigned short Search_Fram(unsigned char* buffer);
unsigned int ReadUnit(unsigned char *buffer,unsigned char index,unsigned char Bytes,unsigned char Format);
void AVI_play(char *FilePath);

void MP3Control_Init(void);
void MP3Control_En(void);
void MP3Control_Dis(void);
#endif

