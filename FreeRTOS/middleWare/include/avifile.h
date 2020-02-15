#ifndef __AVIFILE_H
#define __AVIFILE_H
#include "wbtypes.h"

//#define PrintInfo	//��Ϣ��ӡ����

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
	unsigned int  SecPerFrame;	//��ʾÿ֡��ʱ��
	unsigned int  MaxByteSec; 	//������ݴ�����
	unsigned int  ChunkBase; 	//�鳤��Ϊ��ֵ�ı�����ͨ��Ϊ2048
	unsigned int  SpecProp;	//AVI�ļ�����������
	unsigned int  TotalFrame;	//�ļ���֡��
	unsigned int  InitFrames;  //��ʼ����ǰ�ĳ�ʼ��֡��
	unsigned int  Streams;		//����������������
	unsigned int  RefBufSize;  //����ʹ�õĻ�������С��ͨ��Ϊ�洢һ֡ͼ����ͬ����Ƶ����֮��
	unsigned int  Width;		//ͼ���
	unsigned int  Height;		//ͼ���
	unsigned int  Reserved;	//����
}avih_TypeDef;

typedef struct
{	
	unsigned char     StreamType[4];//���������࣬vids(0x73646976):��Ƶ,auds:��Ƶ
	unsigned char     Handler[4];	//���ݽ�ѹ���������������
	unsigned int  StreamFlag;  //����������
	unsigned short   Priority;	//�����������ŵ����ȼ�
	unsigned short   Language;	//��Ƶ�����Դ���
	unsigned int  InitFrames;  //��ʼ����ǰ�ĳ�ʼ��֡��
	unsigned int  Scale;		//��Ƶÿ֡�Ĵ�С����Ƶ�Ĳ�����
	unsigned int  Rate; 		//Scale/Rate=ÿ�������
	unsigned int  Start;		//��������ʼ���ŵ�λ�ã���λΪScale
	unsigned int  Length;		//������������������λΪScale
 	unsigned int  RefBufSize;  //����ʹ�õĻ�������С
    	unsigned int  Quality;		//��ѹ������������ֵԽ������Խ��
	unsigned int  SampleSize;	//��Ƶ��������С
	struct{				
	   	short  Left;
		short  Top;
		short  Right;
		short  Bottom;
	}Frame;				//��Ƶ֡��ռ�ľ��� 
}strh_TypeDef;

typedef struct tagBMPHEADER
{
   	unsigned int  	Size;		//���ṹ��ռ���ֽ���
	int   	Width;		//ͼ���
	int   	Height;		//ͼ���
	unsigned short   	Planes;		//ƽ����������Ϊ1
	unsigned short   	BitCount;	//����λ��
	unsigned char  	Compression[4];	//ѹ�����ͣ�MJPEG
	unsigned int  	SizeImage;	//ͼ���С
	int   	XpixPerMeter;//ˮƽ�ֱ���
	int   	YpixPerMeter;//��ֱ�ֱ���
	unsigned int  	ClrUsed;		//ʵ��ʹ���˵�ɫ���е���ɫ��,ѹ����ʽ�в�ʹ��
	unsigned int  	ClrImportant;//��Ҫ����ɫ
}BMPHEADER;//�ýṹռ��40�ֽ�

typedef struct tagRGBQUAD 
{
	unsigned char  rgbBlue;	//��ɫ������(ֵ��ΧΪ0-255)
	unsigned char  rgbGreen; //��ɫ������(ֵ��ΧΪ0-255)
	unsigned char  rgbRed; 	//��ɫ������(ֵ��ΧΪ0-255)
	unsigned char  rgbReserved;//����������Ϊ0
}RGBQUAD;//��ɫ��

typedef struct tagBITMAPINFO 
{
	BMPHEADER  bmiHeader;  //λͼ��Ϣͷ
	RGBQUAD    bmColors[1];//��ɫ��
}BITMAPINFO;  

typedef struct 
{
   	unsigned short  FormatTag;
	unsigned short  Channels;	  //������
	unsigned int SampleRate; //������
	unsigned int BaudRate;   //������
	unsigned short  BlockAlign; //���ݿ�����־
	unsigned short  Size;//�ýṹ��С
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

