#ifndef	_CTP_H_
#define	_CTP_H_

#define TP_PRES_DOWN 0x80  //����������	  
#define TP_CATH_PRES 0x40  //�а��������� 
#define CT_MAX_TOUCH  5    //������֧�ֵĵ���,�̶�Ϊ5��


//I2C��д����	0xba
#define GT_CMD_WR 		0Xba     	//д����
#define GT_CMD_RD 		0X29		//������

#define GT_GSTID_REG 	0X814E   	//GT9147��ǰ��⵽�Ĵ������
#define GT_TP1_REG 		0X8150  	//��һ�����������ݵ�ַ
#define GT_TP2_REG 		0X8158		//�ڶ������������ݵ�ַ
#define GT_TP3_REG 		0X8160		//���������������ݵ�ַ
#define GT_TP4_REG 		0X8168		//���ĸ����������ݵ�ַ
#define GT_TP5_REG 		0X8170		//��������������ݵ�ַ  

//������������
typedef struct
{
	unsigned char (*init)(void);			//��ʼ��������������
	unsigned char (*scan)(unsigned char);				//ɨ�败����.0,��Ļɨ��;1,��������;	 
	void (*adjust)(void);		//������У׼ 
	unsigned short x[CT_MAX_TOUCH]; 		//��ǰ����
	unsigned short y[CT_MAX_TOUCH];		//�����������5������,����������x[0],y[0]����:�˴�ɨ��ʱ,����������,��
								//x[4],y[4]�洢��һ�ΰ���ʱ������. 
	unsigned char  sta;					//�ʵ�״̬ 
								//b7:����1/�ɿ�0; 
	                            //b6:0,û�а�������;1,�а�������. 
								//b5:����
								//b4~b0:���ݴ��������µĵ���(0,��ʾδ����,1��ʾ����)
/////////////////////������У׼����(����������ҪУ׼)//////////////////////								
	float xfac;					
	float yfac;
	short xoff;
	short yoff;	   
//�����Ĳ���,��������������������ȫ�ߵ�ʱ��Ҫ�õ�.
//b0:0,����(�ʺ�����ΪX����,����ΪY�����TP)
//   1,����(�ʺ�����ΪY����,����ΪX�����TP) 
//b1~6:����.
//b7:0,������
//   1,������ 
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

/*the information of one touch 1�����������Ϣ*/
typedef struct{
	/*the number of touch points����������*/
	unsigned char            bt_tp_num;
	/*touch gesture��������*/
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
	unsigned char	 type;           // ��Ϣ�����͡�
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


