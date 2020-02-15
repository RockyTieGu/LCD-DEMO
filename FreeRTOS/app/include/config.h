#ifndef	_config_H_
#define	_config_H_
#include "iniparser.h"


#define INIFILENAME ("C:\\cl500.ini")
#define MAX_ECC_LEN (256)

void SaveConfigFile(char *name, dictionary* ini);
int ReadConfigFile(void);
void UpdateTPCal(void);

typedef struct
{
    // BassicSetup
    int 	deviceAddress; 
    int 	baudRate;   
    char 	daySetting[16]; 
    char 	timeSetting[16]; 
    char 	password[8]; 

    // EquipmentManage
    char 	chancel4_20MA;  
    char 	tcpMinIP[16]; 
    char 	tcpMaxIP[16];
    int 	rs485MinAddress;
    int	rs485MaxAddress;

    // StorageFunction
    short dataFun;   
    short dataCycl; 

    //DataPrint
    short printMode;   
    short dataPrintCycle;
    short dataPrintContent;

    //MSGMode
    char MSGSwitch[8];   
    char msgNum0[16];
    char msgNum1[16];
    char msgNum2[16];
    char msgNum3[16];
    char msgNum4[16];
    char msgNum5[16];
	
} Config;
#endif
