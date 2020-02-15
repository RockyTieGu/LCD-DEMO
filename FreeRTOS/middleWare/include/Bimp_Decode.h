#ifndef _FATFS_H_
#define _FATFS_H_
//#include "ff.h"
#include "integer.h"
#include "wbtypes.h"

#define	_VOLUMES	1
#define	BMPONLY	//ֻ֧��bmp���룬��֧��jpeg

typedef __packed struct
{
	unsigned long biSize ;		   //˵��BITMAPINFOHEADER�ṹ����Ҫ��������
	long  biWidth ;		   //˵��ͼ��Ŀ�ȣ�������Ϊ��λ 
	long  biHeight ;	   //˵��ͼ��ĸ߶ȣ�������Ϊ��λ 
	unsigned short  biPlanes ;	   //ΪĿ���豸˵��λ��������ֵ�����Ǳ���Ϊ1 
	unsigned short  biBitCount ;	   //˵��������/���أ���ֵΪ1��4��8��16��24����32
	unsigned long biCompression ;  //˵��ͼ������ѹ�������͡���ֵ����������ֵ֮һ��
			//BI_RGB��û��ѹ����
			//BI_RLE8��ÿ������8���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����(�ظ����ؼ�������ɫ����)��  
			//BI_RLE4��ÿ������4���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����
			//BI_BITFIELDS��ÿ�����صı�����ָ�������������
	unsigned long biSizeImage ;//˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ������BI_RGB��ʽʱ��������Ϊ0  
	long  biXPelsPerMeter ;//˵��ˮƽ�ֱ��ʣ�������/�ױ�ʾ
	long  biYPelsPerMeter ;//˵����ֱ�ֱ��ʣ�������/�ױ�ʾ
	unsigned long biClrUsed ;	   //˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ������
	unsigned long biClrImportant ; //˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ�� 
}BITMAPINFOHEADER ;
//BMPͷ�ļ�
typedef __packed struct
{
	unsigned short  bfType ;     //�ļ���־.ֻ��'BM',����ʶ��BMPλͼ����
	unsigned long bfSize ;	   //�ļ���С,ռ�ĸ��ֽ�
	unsigned short  bfReserved1 ;//����
	unsigned short  bfReserved2 ;//����
	unsigned long bfOffBits ;  //���ļ���ʼ��λͼ����(bitmap data)��ʼ֮��ĵ�ƫ����
}BITMAPFILEHEADER ;
//��ɫ�� 
typedef __packed struct 
{
	unsigned char rgbBlue ;    //ָ����ɫǿ��
	unsigned char rgbGreen ;	  //ָ����ɫǿ�� 
	unsigned char rgbRed ;	  //ָ����ɫǿ�� 
	unsigned char rgbReserved ;//����������Ϊ0 
}RGBQUAD ;
//λͼ��Ϣͷ
typedef __packed struct
{ 
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bmiHeader;  
	//RGBQUAD bmiColors[256];  
}BITMAPINFO; 

typedef RGBQUAD * LPRGBQUAD;//��ɫ��  

typedef struct
{			 
	unsigned int ImgWidth; //ͼ���ʵ�ʿ�Ⱥ͸߶�
	unsigned int ImgHeight;

	unsigned int Div_Fac;  //����ϵ�� (������10000����)
	
	unsigned int S_Height; //�趨�ĸ߶ȺͿ��
	unsigned int S_Width;
	
	unsigned int	S_XOFF;	  //x���y���ƫ����
	unsigned int S_YOFF;

	unsigned int staticx;  //��ǰ��ʾ���ģ�������
	unsigned int staticy;
}PIC_POS;

int strstr_ext (const char *src,const char *dst);
char isExtFile(char *fname);
//FRESULT fatfs_getstatus(unsigned char drv);
//FRESULT scan_files (char* path);
int load_file(char *FileName,unsigned int sx,unsigned int sy,unsigned int ex,unsigned int ey);
//FRESULT getphotofile(char* dir_name);
//char photoplay(char* dir_name);
char BmpDecode(CHAR *BmpFileName);
#endif
