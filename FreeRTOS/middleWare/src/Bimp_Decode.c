#include "Bimp_Decode.h"
#include "w55fa93_vpost.h"
#include "Nvtfat.h"
#include <string.h>

#define DebugBimp sysprintf
unsigned char bmp_buffer[1600];
PIC_POS PICINFO;//图像位置信息
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

//解码这个BMP文件	
void AI_Drow_Init(void)
{
	float temp,temp1;	   
	temp=(float)PICINFO.S_Width/PICINFO.ImgWidth;
	temp1=(float)PICINFO.S_Height/PICINFO.ImgHeight;
						 
	if(temp<temp1)temp1=temp;//取较小的那个	 
	if(temp1>1)temp1=1;	  
	//使图片处于所给区域的中间
	PICINFO.S_XOFF+=(PICINFO.S_Width-temp1*PICINFO.ImgWidth)/2;
	PICINFO.S_YOFF+=(PICINFO.S_Height-temp1*PICINFO.ImgHeight)/2;
	temp1*=10000;//扩大10000倍	 
	PICINFO.Div_Fac=temp1;
			   
	PICINFO.staticx=500;
	PICINFO.staticy=500;//放到一个不可能的值上面			 										    
}   

//判断这个像素是否可以显示
//(x,y) :像素原始坐标
//chg   :功能变量. 
//返回值:0,不需要显示.1,需要显示
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
 描述:  bmp解码函数
****************************************************************************************/
char BmpDecode(char *BmpFileName)
{
	char	suFileName[40];
	unsigned short count;		    	   
	unsigned char  rgb ,color_byte;
	unsigned short x ,y,color,tmp_color ;	  
	unsigned short uiTemp;	   //x轴方向像素计数器 
	unsigned short countpix=0;//记录像素 
	unsigned short x_s, y_s;
	//x,y的实际坐标	
	unsigned short  realx=0;
	unsigned short realy=0;
	unsigned char  end,yok=1;  	
	//int hStatus;
	int	hFile,readcnt;
	//unsigned int r_cnt;			   
	BITMAPINFO *pbmp;//临时指针 

	fsAsciiToUnicode(BmpFileName, suFileName, TRUE);
	hFile = fsOpenFile(suFileName, NULL, O_RDONLY);
	if(hFile < 0 ){
		sysprintf("Fail open %s file hFILE:0x%x\n", BmpFileName, hFile);
		return 0;
	}
	fsReadFile(hFile, bmp_buffer, 1024, &readcnt);
#if 1
	DebugBimp("read %s file %dbytes\n", BmpFileName, readcnt);
	pbmp=(BITMAPINFO*)bmp_buffer;//得到BMP的头部信息   
	count=pbmp->bmfHeader.bfOffBits;        //数据偏移,得到数据段的开始地址
	color_byte=pbmp->bmiHeader.biBitCount/8;//彩色位 16/24/32  
	PICINFO.ImgHeight=pbmp->bmiHeader.biHeight;//得到图片高度
	PICINFO.ImgWidth=pbmp->bmiHeader.biWidth;  //得到图片宽度   
	DebugBimp("photo width %d height %d bytes\n", PICINFO.ImgWidth, PICINFO.ImgHeight);
	DebugBimp("photo start address %d color_byte:%d\n", count, color_byte);
	//水平像素必须是4的倍数!!
	if((PICINFO.ImgWidth*color_byte)%4)
		uiTemp=((PICINFO.ImgWidth*color_byte)/4+1)*4;
	else
		uiTemp=PICINFO.ImgWidth*color_byte;	    
	AI_Drow_Init();//初始化智能画图	    
	//开始解码BMP   	 
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
		while(count<readcnt){  //读取一簇512扇区 (SectorsPerClust 每簇扇区数)
			if(color_byte==3){   //24位颜色图
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
				if(color_byte==2){  //16位颜色图
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
					if(color_byte==4){//32位颜色图
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
			}//位图颜色得到	
			rgb++;	  
			count++ ;		  
			if(rgb==color_byte){ //水平方向读取到1像素数数据后显示
				if(x<PICINFO.ImgWidth){	
					realx=x*PICINFO.Div_Fac/10000;//x轴实际值
					if(IsElementOk(realx,realy,1)&&yok){//符合条件
						x_s = realx+PICINFO.S_XOFF;
						y_s = realy+PICINFO.S_YOFF-1;
				//		DebugBimp("x_s:%d y_s:%d color:0x%x\n", x_s, y_s, color);
						LCDOSDBuffer1[y_s][x_s] = color;
					}   									    
				}
				x++;//x轴增加一个像素 
				color=0x00; 
				rgb=0;  		  
			}
			countpix++;//像素累加
			if(countpix>=uiTemp){//水平方向像素值到了.换行
				y--; 
				if(y<=0){
					fsCloseFile(hFile);
					DebugBimp("file end\n");
					return TRUE; 
				}
				realy=y*PICINFO.Div_Fac/10000;//实际y值改变	 
				if(IsElementOk(realx,realy,0))yok=1;//此处不改变PICINFO.staticx,y的值	 
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
	return TRUE;//BMP显示结束.    					   
} 

//	load_file(tmpp, 0,0,LCD_SIZE_X,LCD_SIZE_Y);
int load_file(char *FileName,unsigned int sx,unsigned int sy,unsigned int ex,unsigned int ey)
{		
	//得到显示方框大小	  	 
	if(ey>sy)PICINFO.S_Height=ey-sy;
	else PICINFO.S_Height=sy-ey;	 
	if(ex>sx)PICINFO.S_Width=ex-sx;
	else PICINFO.S_Width=sx-ex;
	//显示区域无效
	if(PICINFO.S_Height==0||PICINFO.S_Width==0){
		PICINFO.S_Height=_LCD_WIDTH;
		PICINFO.S_Width=_LCD_HEIGHT;
		return 0;   
	}
	//显示的开始坐标点
	PICINFO.S_YOFF=sy;
	PICINFO.S_XOFF=sx;
	//文件名传递
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
			轮流显示文件夹图片
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




