#ifndef _W55FA93_USBD_H_
#define _W55FA93_USBD_H_

/*
 * Standard requests
 */
#define USBR_GET_STATUS			0x00
#define USBR_CLEAR_FEATURE		0x01
#define USBR_SET_FEATURE		0x03
#define USBR_SET_ADDRESS		0x05
#define USBR_GET_DESCRIPTOR		0x06
#define USBR_SET_DESCRIPTOR		0x07
#define USBR_GET_CONFIGURATION	0x08
#define USBR_SET_CONFIGURATION	0x09
#define USBR_GET_intERFACE		0x0A
#define USBR_SET_intERFACE		0x0B
#define USBR_SYNCH_FRAME		0x0C

/*
 * Descriptor types
 */
#define USB_DT_DEVICE			0x01
#define USB_DT_CONFIG			0x02
#define USB_DT_STRING			0x03
#define USB_DT_intERFACE		0x04
#define USB_DT_ENDPOint			0x05
#define USB_DT_QUALIFIER		0x06
#define USB_DT_OSCONFIG			0x07
#define USB_DT_IFPOWER			0x08

//USB FEATURE SELECTOR			value
#define DEVICE_REMOTE_WAKEUP	1
#define ENDPOint_HALT			0
#define TEST_MODE				2

//USB TEST MODES
#define TEST_J					0x01
#define TEST_K					0x02
#define TEST_SE0_NAK			0x03
#define TEST_PACKET				0x04
#define TEST_FORCE_ENABLE		0x05

//Driver definition of tokens
#define OUT_TOK		0x00
#define IN_TOK		0x01
#define SUP_TOK 	0x02
#define PING_TOK	0x03
#define NO_TOK		0x04

//Bit Definitions of IRQ_ENB/STAT register
#define	IRQ_USB_STAT		0x01
#define IRQ_CEP				0x02
#define IRQ_NCEP			0xfc   

//Definition of Bits in USB_IRQ_STS register

#define USB_SOF			0x01	
#define USB_RST_STS		0x02
#define	USB_RESUME		0x04
#define	USB_SUS_REQ		0x08
#define	USB_HS_SETTLE	0x10
#define	USB_DMA_REQ		0x20
#define USABLE_CLK		0x40
#define USB_VBUS		0x100
//Definition of Bits in USB_OPER register
#define USB_GEN_RES     0x1
#define USB_HS		    0x2
#define USB_CUR_SPD_HS  0x4

//Definition of Bits in CEP_IRQ_STS register

#define CEP_SUPTOK	 	0x0001
#define CEP_SUPPKT		0x0002
#define CEP_OUT_TOK		0x0004
#define CEP_IN_TOK		0x0008
#define CEP_PING_TOK	0x0010
#define CEP_DATA_TXD	0x0020
#define CEP_DATA_RXD	0x0040
#define CEP_NAK_SENT	0x0080
#define CEP_STALL_SENT	0x0100
#define CEP_USB_ERR		0x0200
#define CEP_STS_END		0x0400
#define CEP_BUFF_FULL	0x0800
#define CEP_BUFF_EMPTY	0x1000

//Definition of Bits in CEP_CTRL_STS register
#define CEP_NAK_CLEAR		0x00  //writing zero clears the nak bit
#define CEP_SEND_STALL		0x02

//Definition of Bits in EP_IRQ_STS register

#define EP_BUFF_FULL	0x001
#define EP_BUFF_EMPTY	0x002
#define EP_SHORT_PKT	0x004
#define EP_DATA_TXD		0x008
#define EP_DATA_RXD		0x010
#define EP_OUT_TOK		0x020
#define EP_IN_TOK		0x040
#define EP_PING_TOK		0x080
#define EP_NAK_SENT		0x100
#define EP_STALL_SENT	0x200
#define EP_USB_ERR		0x800

//Bit Definitons of EP_RSP_SC Register
#define EP_BUFF_FLUSH   0x01
#define EP_MODE         0x06
#define EP_MODE_AUTO	0x01
#define EP_MODE_MAN 	0x02
#define EP_MODE_FLY		0x03
#define EP_TOGGLE		0x8
#define EP_HALT			0x10
#define EP_ZERO_IN      0x20
#define EP_PKT_END      0x40

/* Define Endpoint feature */
#define Ep_In        0x01
#define Ep_Out       0x00
#define Ep_Bulk      0x01
#define Ep_Int       0x02
#define Ep_Iso       0x03
#define EP_A         0x00
#define EP_B         0x01
#define EP_C         0x02
#define EP_D         0x03

typedef struct usb_cmd
{
	unsigned char	bmRequestType;
	unsigned char	bRequest;
	unsigned short	wValue;
	unsigned short	wIndex;
	unsigned short	wLength;
}	USB_CMD_T;


typedef void (*PFN_USBD_CALLBACK)(void);
typedef char (PFN_USBD_EXIT_CALLBACK)(void);
typedef void (*PFN_USBD_EP_CALLBACK)(unsigned int u32IntEn,unsigned int u32IntStatus);

__packed  typedef struct{
/* Descriptor pointer */
	unsigned int * pu32DevDescriptor;
	unsigned int * pu32QulDescriptor;
	unsigned int * pu32HSConfDescriptor;
	unsigned int * pu32FSConfDescriptor;	
	unsigned int * pu32HOSConfDescriptor;
	unsigned int * pu32FOSConfDescriptor;	
	unsigned int * pu32StringDescriptor[5];
	
	/* Descriptor length */
	unsigned int	u32DevDescriptorLen;	
	unsigned int	u32QulDescriptorLen;	
	unsigned int	u32HSConfDescriptorLen;	
	unsigned int	u32FSConfDescriptorLen;	
	unsigned int	u32HOSConfDescriptorLen;	
	unsigned int	u32FOSConfDescriptorLen;	
	unsigned int 	u32StringDescriptorLen[5];

	/* USBD Init */
	PFN_USBD_CALLBACK pfnHighSpeedInit;
	PFN_USBD_CALLBACK pfnFullSpeedInit;

	/* Endpoint Number */
	int	i32EPA_Num;
	int	i32EPB_Num;
	int	i32EPC_Num;
	int	i32EPD_Num;	
				
	/* Endpoint Call Back */	
	PFN_USBD_EP_CALLBACK pfnEPACallBack;
	PFN_USBD_EP_CALLBACK pfnEPBCallBack;	
	PFN_USBD_EP_CALLBACK pfnEPCCallBack;
	PFN_USBD_EP_CALLBACK pfnEPDCallBack;
				
	/* Class Call Back */	
	PFN_USBD_CALLBACK pfnClassDataINCallBack;
	PFN_USBD_CALLBACK pfnClassDataOUTCallBack;
	PFN_USBD_CALLBACK pfnDMACompletion;
	PFN_USBD_CALLBACK pfnReset;
	PFN_USBD_CALLBACK pfnSOF;	
	PFN_USBD_CALLBACK pfnPlug;	
	PFN_USBD_CALLBACK pfnUnplug;	
	
	/* Transfer Control */
	unsigned int	_usbd_remlen;
	char	_usbd_remlen_flag;
	unsigned int	*_usbd_ptr;
	
	/* Command Status Control */
	unsigned char	GET_DEV_Flag;
	unsigned char	GET_CFG_Flag;
	unsigned char	GET_QUL_Flag;
	unsigned char	GET_OSCFG_Flag;
	unsigned char	GET_STR_Flag;
	unsigned char	GET_VEN_Flag;
	unsigned char	GET_VENO_Flag;
	unsigned char	CLASS_CMD_Flag;
	unsigned char	CLASS_CMD_Iflag;
	unsigned char	CLASS_CMD_Oflag;
	
	unsigned int	usbdMaxPacketSize;
	
	/* DMA Status flag */
	unsigned char	_usbd_DMA_Flag;
	unsigned char	_usbd_DMA_In;
	 
	/* Status flag */	
	unsigned char	_usbd_resume;
	unsigned char	USBModeFlag;   
	unsigned int	_usbd_devstate;
	unsigned int	_usbd_address;
	unsigned int	_usbd_speedset;
	unsigned short	_usbd_confsel;
	unsigned short	_usbd_intfsel;
	unsigned short	_usbd_altsel;
	unsigned int	_usbd_feature;
	int	_usbd_haltep;
	int	_usbd_unhaltep;
	int	remotewakeup;
	int	testmode;
	int	enableremotewakeup;
	int	enabletestmode;
	int	disableremotewakeup;
	int	disabletestmode;
	int	testselector;
	unsigned int	usbdGetStatusData;
	unsigned char	usbdGetConfig;
	unsigned char	usbdGetInterface;
	unsigned char	usbdGetStatus;

	/* for isochronous */
	unsigned int  AlternateFlag;		
	unsigned int  u32VbusStatus;
	
}USBD_INFO_T;

__packed  typedef struct{
	unsigned int  appConnected;		
	unsigned int  usbConnected;	
}USBD_STATUS_T;


typedef struct
{
	unsigned char EP_Num;
	unsigned char EP_Dir;
	unsigned char EP_Type;
}	USB_EP_Inf_T;


void udcOpen(void);
void udcClose(void);
void udcInit(void);
void udcDeinit(void);
char udcIsAttached(void);

#endif
