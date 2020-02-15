#ifndef _SPILIB_H_
#define _SPILIB_H_

#define SPI_SET_CLOCK			0

#define SPI_8BIT	8
#define SPI_16BIT	16
#define SPI_32BIT	32

typedef struct _spi_info_t
{
	int	nPort;
	char	bIsSlaveMode;
	char	bIsClockIdleHigh;
	char	bIsLSBFirst;
	char	bIsAutoSelect;
	char	bIsActiveLow;
	char	bIsTxNegative;
} SPI_INFO_T;

/* extern function */
void spiIoctl(int spiPort, int spiFeature, int spicArg0, int spicArg1);
int  spiOpen(SPI_INFO_T *pInfo);
int  spiRead(int port, int RxBitLen, int len, CHAR *pDst);
int  spiWrite(int port, int TxBitLen, int len, CHAR *pSrc);
int  spiEnable(int spiPort);
int  spiDisable(int spiPort);


/* for SPI Flash */
int  spiFlashInit(void);
int  spiFlashEraseSector(unsigned int addr, unsigned int secCount);
int  spiFlashEraseAll(void);
int  spiFlashWrite(unsigned int addr, unsigned int len, unsigned int *buf);
int  spiFlashRead(unsigned int addr, unsigned int len, unsigned int *buf);
int spiFlashWriteByte(unsigned int addr, unsigned int len, unsigned char *buf);
int  spiFlashReadByte(unsigned int addr, unsigned int len, unsigned char *buf);
int spiFlashPDMARead(unsigned int addr, unsigned int len, unsigned int *buf);

/* internal function */
int spiActive(int port);
int spiTxLen(int port, int count, int bitLen);

void spiSetClock(int port, int clock_by_MHz, int output_by_kHz);


#endif
