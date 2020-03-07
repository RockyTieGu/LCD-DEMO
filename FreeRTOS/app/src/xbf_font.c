#include "xbf_font.h"
#include "wbtypes.h"
#include "nvtfat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wbtypes.h"
#include "wblib.h"
#include "unicode.h"
/*
1.emwin 所用得的字库文件是UTF-8格式的，而KIEL5 文本是用GB2312码。因此在使用emwin 字库接口函数，应该转化
*/
static INT  handle;
//GUI_XBF_DATA XBF_Data;
INT XBF_Data;
GUI_FONT     XBF_Font;
INT bw;
#define SIF_Font_song20 &XBF_Font
#define EXT_SRAM_ADDR  	((UINT32)0x01900000)

/***************************************************************************** 
 * 将一个字符的Unicode(UCS-2)编码转换成UTF-8编码. Unicode 要小于 0xffff
 ****************************************************************************/  
char * unicode_to_utf8(char *in) 
{  
	static char Temp[256];	
	static  char *p1;
	UINT16 unic=0,i=0,l=0;		
	 

	p1=Temp;	
	memset(Temp, 0, sizeof(Temp));
		
	unic =((*(in+1))<<8)+(*in);//  高位再前
	if(unic==0)return "ERROR";	
	while(i<128)
	{
		if ( unic <= 0x0000007F )  
		{  
			// * U-00000000 - U-0000007F:  0xxxxxxx  
			*p1	= (unic & 0x7F);  
			//if(*p1==' ')break;
			l=1;
		}  
		else if ( unic >= 0x00000080 && unic <= 0x000007FF )  
		{  
			// * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx  
			*(p1+1) = (unic & 0x3F) | 0x80;  
			*p1     = ((unic >> 6) & 0x1F) | 0xC0;  
			l=2; 		
		}  
		else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )  
		{  
			// * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx  
			*(p1+2) = (unic & 0x3F) | 0x80;  
			*(p1+1) = ((unic >>  6) & 0x3F) | 0x80;  
			*p1     = ((unic >> 12) & 0x0F) | 0xE0;  
			l=3; 
		}
		else
		{
			return "ERROR";
		}
		in+=2;
		unic =((*(in+1))<<8)+(*in);	
		if(unic==0)break;	
		p1+=l;
		i++;
	}
	
	return Temp; //可以返回指针
}

#define	GB_OFFSET_NUMBER	94		//the number of one GB zone.
#define	GB_MIN_ZONE			0xA0	//min value of the zone number of GB code
#define	GB_MAX_ZONE			0xF8	//max value of the zone number of GB code
#define	GB_MIN_OFFSET		0xA1	//min value of the offset number of GB code
#define	GB_MAX_OFFSET		0xFF	//max value of the offset number of GB code
#define	MAX_OFFSET_IN_TABLE	8272	//the max offset of the conversion table.

unsigned short fsGBToUnicode(					//return the number of UCS2 code that have been converted.
						unsigned short *ucs2_ptr, 	//destination: string pointer to UCS2 string.
						const char *gb_ptr,//source: string pointer to GB string
						unsigned short len			//the length to convert from GB to UCS2.
						)
{
	unsigned short tmp_gb, count = 0;
	unsigned short offset,ucs2_code;

	while((len > 0) && (len < 0xfff)){
		len--;
		count += 1;
		tmp_gb = (UINT16)(*gb_ptr++);

		if (tmp_gb<0x80){	//An ASCII
			*ucs2_ptr++ = tmp_gb;
		}	
		else if ((tmp_gb >= GB_MIN_ZONE) && (tmp_gb < GB_MAX_ZONE))
		{
			offset = (UINT16)(*gb_ptr++);
			len--;
		
			if ((offset >= GB_MIN_OFFSET) && (offset < GB_MAX_OFFSET)){
				offset = (offset - GB_MIN_OFFSET) + (tmp_gb - GB_MIN_ZONE) * GB_OFFSET_NUMBER;
				//*ucs2_ptr++ = (offset < MAX_OFFSET_IN_TABLE) ? gb2312_to_ucs2_table[offset] : 0x00; 

				ucs2_code = (offset < MAX_OFFSET_IN_TABLE) ? gb2312_to_ucs2_table[offset] : 0x00; 
				*ucs2_ptr++ = ucs2_code;
			}
			else{
				//An invalidate code.
				*ucs2_ptr++ = 0x00;
			}
		}
		else
		{
			//An invalidate code.
			*ucs2_ptr++ = 0x00;
			gb_ptr++;
			len--;
		}
	}
	
	return count;
} 

unsigned short UnicodeToGB2312(unsigned short unicode)//用二分查找算法
{
 
   int mid, low, high, len;
	len = sizeof(UnicodeToGB2312_Tab)/sizeof(UnicodeToGB2312_Tab[0]);
	low = 0;
	high = len - 1;
	if( (unicode >= 0x20 && unicode <= 0x5b) || (unicode >= 0x5d && unicode <= 0x7e))
	   return unicode;
	while(low <= high)
	{
	 
	    mid = (low + high) / 2;
	if(UnicodeToGB2312_Tab[mid][0] > unicode)
	    high = mid - 1;
	else if(UnicodeToGB2312_Tab[mid][0] < unicode)
	    low =  mid + 1;
	else
	    return UnicodeToGB2312_Tab[mid][1];
	 
	}
	return 0 ; //没找到
 
} 

int UniToGB2312(unsigned char *unicode, unsigned short len, unsigned char *str)
{
	UINT16 code, i = 0, m = 0;

	for(i = 0; i < len/2; i++){
		//DeBugFont("unicode[%d]:0x%x\n", i,unicode[i]);
		//DeBugFont("unicode[%d + 1]:0x%x\n", i + 1,unicode[i+1]);
		code= (unicode[i*2] << 8) + unicode[i*2 +1]; 
		//sysprintf("[%d]:0x%x\n", i,code);
		if((code==0)||(code==0xffff))break;	
		
		if((code&0xff00)==0){//对字符处理
			if((code>=0x20)&&(code<=0x7e)){
				str[m++]=(unsigned char)code;              
			}else{
				str[m++]='?';//无法识别的用？代替 	
			}
			continue;
		}

		if(code>=0x4e00){ //是汉字
			code=UnicodeToGB2312(code);//把unicode转换为gb2312 	
			str[m++]=(code>>8)&0xff;
			str[m++]=code&0xff; 
		}else{
				str[m++]='?';//无法识别的用？代替 	
		}
	}
	str[m] ='\0';
	return m;
}

int _cbGetData(U32 Off, U16 NumBytes, void * pVoid, void * pBuffer)
{
	INT  handle , *handle1;
	INT	 nStatus;

	handle1=(INT*)pVoid;
	handle = *handle1;   
	

	nStatus =fsFileSeek(handle, Off, SEEK_SET);
	
	if (nStatus < 0) 
	{
		return 1;
	}

	nStatus = fsReadFile(handle, pBuffer, NumBytes, &bw);
	
	if (nStatus != FS_OK) 
	{
		return 1;
	}
	return 0;

}

static UINT8 *_acBuffer = NULL;
void _ShowXBF(void) 
{
	CHAR		suFileName[128];

	
	//_acBuffer=(UINT8 *)EXT_SRAM_ADDR;

	fsAsciiToUnicode("c:\\Font\\song16.sif", suFileName, TRUE);
	handle = fsOpenFile(suFileName, "",O_RDONLY);

	if (handle <= 0) 
	{
		sysprintf("open %s file fail! handle:0x%x\r\n", "c:\\Font\\song16.sif", handle);
		return ;
	}
	else
	{
		sysprintf("success open %s file \r\n", "c:\\Font\\song16.sif");
	}
	
	_acBuffer = (unsigned char *)malloc(sizeof(CHAR) * fsGetFileSize(handle));
	if(_acBuffer == NULL)
	{
		sysprintf("Malloc %d bytes fail for %s font \r\n", fsGetFileSize(handle),"c:\\Font\\song16.sif");
		return ;
	}
	fsReadFile(handle, _acBuffer, fsGetFileSize(handle), &bw);
	GUI_SIF_CreateFont(_acBuffer, SIF_Font_song20, GUI_SIF_TYPE_PROP);	
}

GUI_FONT * SetFont_Xbf(void)
{
    return ((GUI_FONT *)GUI_SetFont(&XBF_Font));

}

static char T[300]={0};  //最大256字	
char * zh(char * pText)
{
	char * pT;
	pT=T;
	memset(T, 0, sizeof(T));
	while(!((*pText == '+')&&(*(pText+1) == '-')))
	{
		 *pT=*pText;
			pT++;
		  pText++;
	}
	return T;
}

