#ifndef	_CTP_H_
#define	_CTP_H_

#define TP_PRES_DOWN 0x80  //触屏被按下	  
#define TP_CATH_PRES 0x40  //有按键按下了 
#define CT_MAX_TOUCH  5    //电容屏支持的点数,固定为5点


//I2C读写命令	0xba
#define GT_CMD_WR 		0Xba     	//写命令
#define GT_CMD_RD 		0X29		//读命令

#define GT_GSTID_REG 	0X814E   	//GT9147当前检测到的触摸情况
#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址  

//触摸屏控制器
typedef struct
{
	unsigned char (*init)(void);			//初始化触摸屏控制器
	unsigned char (*scan)(unsigned char);				//扫描触摸屏.0,屏幕扫描;1,物理坐标;	 
	void (*adjust)(void);		//触摸屏校准 
	unsigned short x[CT_MAX_TOUCH]; 		//当前坐标
	unsigned short y[CT_MAX_TOUCH];		//电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
								//x[4],y[4]存储第一次按下时的坐标. 
	unsigned char  sta;					//笔的状态 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
/////////////////////触摸屏校准参数(电容屏不需要校准)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
//   1,横屏(适合左右为Y坐标,上下为X坐标的TP) 
//b1~6:保留.
//b7:0,电阻屏
//   1,电容屏 
	unsigned char touchtype;
}_m_tp_dev;


/**
For CTP
**/
#define	CTP_I2C_ID		0x70
#define I2C_STARTTCH_READ   0xF9
#define I2C_WORK_STARTREG   0xFC
#define	PROTOCOL_LEN	26

 /* 
 Error status codes:
 */
#define CTPM_NOERROR            (0x01 << 0)
#define CTPM_ERR_PARAMETER      (0x01 << 1)
#define CTPM_ERR_PROTOCOL       (0x01 << 2)
#define CTPM_ERR_ECC            (0x01 << 3)
#define CTPM_ERR_MODE           (0x01 << 4)
#define CTPM_ERR_I2C            (0x01 << 5)
#define CTPM_ERR_SCAN           (0x01 << 6)

/*fts 26bytes protocol,support 5 points*/
#define CTPM_26BYTES_POintS_MAX 0x05

/*the information of one touch point */
typedef struct{
	/*x coordinate*/
	unsigned short    w_tp_x;
	/*y coordinate*/
	unsigned short    w_tp_y;

	/*point id: start from 1*/
	unsigned char    bt_tp_id;

	/*0 means press down; 1 means put up*/
	unsigned char    bt_tp_property;

	/*the strength of the press*/
	unsigned short    w_tp_strenth;
}ST_TOUCH_POint, *PST_TOUCH_POint;

typedef enum{
	TYPE_Z00M_IN,
	TYPE_Z00M_OUT,
	TYPE_INVALIDE
}E_GESTURE_TYPE;

/*the information of one touch 1个触摸点的信息*/
typedef struct{
	/*the number of touch points触摸点数量*/
	unsigned char            bt_tp_num;
	/*touch gesture触摸姿势*/
	E_GESTURE_TYPE      bt_gesture;
	/*point to a list which stored 1 to 5 touch points information*/
	ST_TOUCH_POint*     pst_point_info;
}ST_TOUCH_INFO, *PST_TOUCH_INFO;

enum{
	TS_READY = 0,
	TS_START,
	TS_RELEASE
};

typedef struct {
	unsigned char	 type;           // 消息的类型。
	short	data[2];
}msg_t;

typedef struct 
{
	unsigned char		curpage;
	char		(*dealkey)(unsigned short key);
	char		(*dealtspress)(unsigned short x,unsigned short y);
	char		(*dealtsrelease)(unsigned short x,unsigned short y);
	char		(*dealtsmove)(unsigned short x,unsigned short y);
	char		(*dealtskeep)(unsigned short x,unsigned short y);
}hmi_t;

enum{
	MSG_NULL = 0,
	MSG_KEY,
	MSG_TP_PRESS,
	MSG_TP_RELEASE,
	MSG_TP_MOVE,
	MSG_TP_KEEP
};

#define	MAX_MSG	20

#define	GUI_KEY_PRESS		0x1000
#define	GUI_KEY_KEEP		0x2000
#define	GUI_KEY_RELEASE	0x8000
#define	KEY_1HZ		0xFF01
#define	KEY_10HZ		0xFF02
#define	KEY_SMART		0x0000|GUI_KEY_PRESS
#define	KEY_MENU		0x0001|GUI_KEY_PRESS
#define	KEY_ORDER		0x0002|GUI_KEY_PRESS
#define	KEY_POWER		0x0003|GUI_KEY_PRESS
#define	KEY_OK			0x0010|GUI_KEY_PRESS
#define	KEY_UP			0x0011|GUI_KEY_PRESS
#define	KEY_DOWN		0x0012|GUI_KEY_PRESS
#define	KEY_RETURN		0x0013|GUI_KEY_PRESS

void sendMsg(msg_t* msg);
void ClearMsg(unsigned char msgtype);
unsigned char	getmessage(msg_t* msg0);
unsigned char CTP_Init(void);
unsigned char CPT_ReadPacket(void);
unsigned char bt_parser_pixcir(void);
unsigned char bt_parser_fts(void);
void ctp_isr(void);
void BackLightAdj(unsigned char ulight);
#endif


