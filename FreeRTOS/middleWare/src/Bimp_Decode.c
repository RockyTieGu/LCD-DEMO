#include "Bimp_Decode.h"
#include "w55fa93_vpost.h"
#include "Nvtfat.h"
#include <string.h>

#define DebugBimp sysprintf
unsigned char bmp_buffer[1600];
PIC_POS PICINFO;//ͼ��λ����Ϣ
extern __align(32) unsigned short OSD_FrameRGB565[_LCD_HEIGHT*_LCD_WIDTH*2];
extern __align(32) unsigned short LCDOSDBuffer1[_LCD_HEIGHT][_LCD_WIDTH];
#ifdef	BMPONLY
#define	TOTAL_EXT	4
const char* IMAGE_FILE[TOTAL_EXT] = {"bmp","BMP","bin","BIN"};
#else
#define	TOTAL_EXT	8
const char* IMAGE_FILE[TOTAL_EXT] = {"bmp","BMP","JPG","jpg","JPEG","jpeg","bin","BIN"};
#endif

char isExtFile(char *fname)
{
	int i,len;
	char ext[4];

	memset(ext,0,4);
	len = strlen(fname);
	for(i=len-1;i>0;i--){
		if(fname[i-1]=='.')break;
		}
	strcpy(ext,fname+i);
	for(i=0;i<TOTAL_EXT;i++){
		if(strcmp(ext,IMAGE_FILE[i])==0)return TRUE;
		}
	return FALSE;
}

int strstr_ext (
	const char *src,
	const char *dst
)
{
	int si, di;
	char s, d;

	si = strlen(src);
	di = strlen(dst);
	if (si < di) return 0;
	si -= di; di = 0;
	do {
		s = src[si++];
		if (s >= 'a' && s <= 'z') s -= 0x20;
		d = dst[di++];
		if (d >= 'a' && d <= 'z') d -= 0x20;
	} while (s && s == d);
	return (s == d);
}

//�������BMP�ļ�	
void AI_Drow_Init(void)
{
	float temp,temp1;	   
	temp=(float)PICINFO.S_Width/PICINFO.ImgWidth;
	temp1=(float)PICINFO.S_Height/PICINFO.ImgHeight;
						 
	if(temp<temp1)temp1=temp;//ȡ��С���Ǹ�	 
	if(temp1>1)temp1=1;	  
	//ʹͼƬ��������������м�
	PICINFO.S_XOFF+=(PICINFO.S_Width-temp1*PICINFO.ImgWidth)/2;
	PICINFO.S_YOFF+=(PICINFO.S_Height-temp1*PICINFO.ImgHeight)/2;
	temp1*=10000;//����10000��	 
	PICINFO.Div_Fac=temp1;
			   
	PICINFO.staticx=500;
	PICINFO.staticy=500;//�ŵ�һ�������ܵ�ֵ����			 										    
}   

//�ж���������Ƿ������ʾ
//(x,y) :����ԭʼ����
//chg   :���ܱ���. 
//����ֵ:0,����Ҫ��ʾ.1,��Ҫ��ʾ
 unsigned char IsElementOk(unsigned short x,unsigned short y,unsigned char chg)
{				  
	if(x!=PICINFO.staticx||y!=PICINFO.staticy){
		if(chg==1){
			PICINFO.staticx=x;
			PICINFO.staticy=y;
		} 
		return 1;
	}

	else return 0;
}

/***************************************************************************************
 ����:  bmp���뺯��
****************************************************************************************/
char BmpDecode(char *BmpFileName)
{
	char	suFileName[40];
	unsigned short count;		    	   
	unsigned char  rgb ,color_byte;
	unsigned short x ,y,color,tmp_color ;	  
	unsigned short uiTemp;	   //x�᷽�����ؼ����� 
	unsigned short countpix=0;//��¼���� 
	unsigned short x_s, y_s;
	//x,y��ʵ������	
	unsigned short  realx=0;
	unsigned short realy=0;
	unsigned char  end,yok=1;  	
	//int hStatus;
	int	hFile,readcnt;
	//unsigned int r_cnt;			   
	BITMAPINFO *pbmp;//��ʱָ�� 

	fsAsciiToUnicode(BmpFileName, suFileName, TRUE);
	hFile = fsOpenFile(suFileName, NULL, O_RDONLY);
	if(hFile < 0 ){
		sysprintf("Fail open %s file hFILE:0x%x\n", BmpFileName, hFile);
		return 0;
	}
	fsReadFile(hFile, bmp_buffer, 1024, &readcnt);
#if 1
	DebugBimp("read %s file %dbytes\n", BmpFileName, readcnt);
	pbmp=(BITMAPINFO*)bmp_buffer;//�õ�BMP��ͷ����Ϣ   
	count=pbmp->bmfHeader.bfOffBits;        //����ƫ��,�õ����ݶεĿ�ʼ��ַ
	color_byte=pbmp->bmiHeader.biBitCount/8;//��ɫλ 16/24/32  
	PICINFO.ImgHeight=pbmp->bmiHeader.biHeight;//�õ�ͼƬ�߶�
	PICINFO.ImgWidth=pbmp->bmiHeader.biWidth;  //�õ�ͼƬ���   
	DebugBimp("photo width %d height %d bytes\n", PICINFO.ImgWidth, PICINFO.ImgHeight);
	DebugBimp("photo start address %d color_byte:%d\n", count, color_byte);
	//ˮƽ���ر�����4�ı���!!
	if((PICINFO.ImgWidth*color_byte)%4)
		uiTemp=((PICINFO.ImgWidth*color_byte)/4+1)*4;
	else
		uiTemp=PICINFO.ImgWidth*color_byte;	    
	AI_Drow_Init();//��ʼ�����ܻ�ͼ	    
	//��ʼ����BMP   	 
	if(readcnt<1024)
		end=1;
	else end = 0;
	//x =0 ;
	x = 0;
	y= PICINFO.ImgHeight;
	rgb=0;      
	realy=y*PICINFO.Div_Fac/10000;
	//realx=x*PICINFO.Div_Fac/10000;
	while(1){	
//		 DebugBimp("count:%d readcnt:%d\n", count, readcnt);
		while(count<readcnt){  //��ȡһ��512���� (SectorsPerClust ÿ��������)
			if(color_byte==3){   //24λ��ɫͼ
				switch (rgb) {
					case 0:
						tmp_color = bmp_buffer[count]>>3 ;
						color |= tmp_color;
						break ;	   
					case 1: 
						tmp_color = bmp_buffer[count]>>2 ;
						tmp_color <<= 5 ;
						color |= tmp_color ;
						break;	  
					case 2 : 
						tmp_color = bmp_buffer[count]>>3 ;
						tmp_color <<= 11 ;
						color |= tmp_color ;
						break ;			
				}   
			}else{
				if(color_byte==2){  //16λ��ɫͼ
					switch(rgb){
						case 0 : 
							color=bmp_buffer[count]&0x1f;
						    	tmp_color=bmp_buffer[count]>>5;
							tmp_color<<=6;
							color|=tmp_color;
							break ;   
						case 1 : 			  
							tmp_color=bmp_buffer[count];
							tmp_color<<=9 ;
							color |= tmp_color ;
							break ;	 
					}		     
				}
				else {
					if(color_byte==4){//32λ��ɫͼ
						switch (rgb){
							case 0 :  
								tmp_color=bmp_buffer[count];
								color|=tmp_color>>3;
								break ;     
							case 1 :  
								tmp_color=bmp_buffer[count];
								tmp_color>>=2;
								color|=tmp_color<<5;
								break ;	  
							case 2 :  
								tmp_color=bmp_buffer[count];
								tmp_color>>=3;
								color|=tmp_color<<11;
								break ;	 
							case 3 :break ;   
						}		  	 
					} 
				}     
			}//λͼ��ɫ�õ�	
			rgb++;	  
			count++ ;		  
			if(rgb==color_byte){ //ˮƽ�����ȡ��1���������ݺ���ʾ
				if(x<PICINFO.ImgWidth){	
					realx=x*PICINFO.Div_Fac/10000;//x��ʵ��ֵ
					if(IsElementOk(realx,realy,1)&&yok){//��������
						x_s = realx+PICINFO.S_XOFF;
						y_s = realy+PICINFO.S_YOFF-1;
				//		DebugBimp("x_s:%d y_s:%d color:0x%x\n", x_s, y_s, color);
						LCDOSDBuffer1[y_s][x_s] = color;
					}   									    
				}
				x++;//x������һ������ 
				color=0x00; 
				rgb=0;  		  
			}
			countpix++;//�����ۼ�
			if(countpix>=uiTemp){//ˮƽ��������ֵ����.����
				y--; 
				if(y<=0){
					fsCloseFile(hFile);
					DebugBimp("file end\n");
					return TRUE; 
				}
				realy=y*PICINFO.Div_Fac/10000;//ʵ��yֵ�ı�	 
				if(IsElementOk(realx,realy,0))yok=1;//�˴����ı�PICINFO.staticx,y��ֵ	 
				else yok=0; 
				x=0; 
				countpix=0;
				color=0x00;
				rgb=0;
			}	 
		} 
		if(end==1)break;
		fsReadFile(hFile, bmp_buffer, 1024, &readcnt);
		if(readcnt<1024)end=1;
	   	count=0 ;
	}	
	#endif
	fsCloseFile(hFile);
	return TRUE;//BMP��ʾ����.    					   
} 

//	load_file(tmpp, 0,0,LCD_SIZE_X,LCD_SIZE_Y);
int load_file(char *FileName,unsigned int sx,unsigned int sy,unsigned int ex,unsigned int ey)
{		
	//�õ���ʾ�����С	  	 
	if(ey>sy)PICINFO.S_Height=ey-sy;
	else PICINFO.S_Height=sy-ey;	 
	if(ex>sx)PICINFO.S_Width=ex-sx;
	else PICINFO.S_Width=sx-ex;
	//��ʾ������Ч
	if(PICINFO.S_Height==0||PICINFO.S_Width==0){
		PICINFO.S_Height=_LCD_WIDTH;
		PICINFO.S_Width=_LCD_HEIGHT;
		return 0;   
	}
	//��ʾ�Ŀ�ʼ�����
	PICINFO.S_YOFF=sy;
	PICINFO.S_XOFF=sx;
	//�ļ�������
	if(strstr_ext(FileName, ".bmp")){
		DebugBimp("Decode start!\n");
		if (!BmpDecode(FileName)){
			return 0;
		} 
		DebugBimp("Decode finished!\n");
		return 1;
	}
	return 0;
}


#if 0
/*****************************************************************************
			������ʾ�ļ���ͼƬ
*****************************************************************************/
char photoplay(char* dir_name)
{
	FRESULT res;
	char tmpp[100];
	
	for(;;) {
		res = f_readdir(&pdir, &pfileinfo);
		if ((res != FR_OK) || !pfileinfo.fname[0]){
		//	res = f_opendir(&pdir, dir_name);
		//	res = f_readdir(&pdir, &pfileinfo);
		//	break;
			return FALSE;
			}
		if (pfileinfo.fattrib & AM_DIR) {
		} else {
			if(isExtFile(pfileinfo.fname)){
				break;
				}
			}
		}
	sprintf(tmpp,"%s\\%s",dir_name,pfileinfo.fname);
	load_file(tmpp, 0,0,LCD_SIZE_X,LCD_SIZE_Y);
	return TRUE;
}
#endif




