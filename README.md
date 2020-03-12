备注：
1.该DEMO是5寸屏通用板。
2.该目录下，NANDFLASH文件夹为固件，请将固件拷贝到LCD DEMO板NandFlash中。
3.该目录下，SD卡文件夹为音视频文件。建议将视频文件，放入到SD卡中。
4.目前demo板只支持AVI视频文件，若需要播放其它视频，请将视频格式转化为AVI MOTION JPEG格式即可。
5.当LCD 需要VGL,VGH,AVDD,VCOM 时，将转接板R056,R057,R058,R059焊接为0欧。借用主板的四路电压来，供给屏。


修改信息
2020.3.11
1.增加按键控制 EMWIN DEMO.
2.修改lcd 初始化延时函数，由定时器延时改为for循环延时。

2020.3.12
1.该工程把如下LCD 全部都包含进去。
  修改#define SELECT_LCD_TYPE 			(0)  
  宏定义，编译选定的LCD.编译完成后，将固件output/conprog.bin文件拷贝到NANDFLASH-1中，即可运行。
  C035QL01N_RGB_SPINIT_320_240_KEY = 0,
  C035QC05N_RGB_NOINIT_320_240_KEY = 1,
  C043WVH02NE_RGB_NOINIT_800_480_KEY =2 ,
  C050WQ01_RGB_NOINIT_480_272_KEY = 3,
  C070WSH01HH_RGB_NOINIT_1024_600_KEY =4