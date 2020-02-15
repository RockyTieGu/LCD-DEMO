#ifndef	_CTP_H_
#define	_CTP_H_
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
void CTP_Init(void);
int CPT_ReadPacket(void);
unsigned char bt_parser_pixcir(void);
unsigned char bt_parser_fts(void);
void ctp_isr(void);
void BackLightAdj(unsigned char ulight);
#endif


