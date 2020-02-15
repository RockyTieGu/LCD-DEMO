#ifndef	_CONN_H_
#define	_CONN_H_

#define	STATUS_POWERON	0x01
#define	STATUS_SELFTEST	0x02
#define	STATUS_POWEROFF	0x04
#define	STATUS_IDLE		0x08
#define	STATUS_HOLD		0x10
#define	STATUS_RUNNING	0x20
#define	STATUS_ERROR		0x40
#define	STATUS_WASHING	0x80
#define	STATUS_DRAIN		0x11

#define	MODE_USER			0x00
#define	MODE_CLOUD		0x01
#define	MODE_ECO			0x02
#define	MODE_YIJUN			0x04
#define	MODE_WIFIORDER	0x10

unsigned char	BuffCompare(unsigned char* src,unsigned char* dst);
void CommParse(void);
#endif

