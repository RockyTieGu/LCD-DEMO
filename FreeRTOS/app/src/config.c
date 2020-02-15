#include "wb_include.h"
#include "Nvtfat.h"
#include "config.h"

dictionary* ini;
extern unsigned short coord[4][2];
unsigned short LsSaveCtp;
volatile char lsUseConfigFile = 0;
volatile char version_num[10];
volatile char lsDisplayBKNum = 1;
volatile char lsSupServerInter = 1;
volatile char lsMenuEditable = 0;
extern unsigned char lsUseSDBackGround;
Config theConfig;
void SaveConfigFile(char *name, dictionary* ini)
{
	int dFile;
	CHAR	suFileName[40];

	fsAsciiToUnicode(INIFILENAME, suFileName, TRUE);
	dFile = fsOpenFile(suFileName, NULL, O_WRONLY);
	if(dFile < 0){
		DEBUGINI("fail open %s file errcode:0x%x\n", INIFILENAME, dFile);
		return ;
	}
	iniparser_dump_ini(ini, dFile);
  	fsCloseFile(dFile);

	return ;
}

int ReadConfigFile()
{
	int value = 0;
	char temp[50];
	//char flag = 0;
	
	ini = iniparser_load((char*)INIFILENAME);
    if (ini == NULL)	{
		DEBUGINI("¼ÓÔØË½ÓÃÃÜÔ¿ÅäÖÃÎÄ¼þ[%s]Ê§°Ü\n",INIFILENAME);
		lsUseConfigFile = 0;
	//	lsUseSDBackGround = 0;
		return 0;
	}
#if 0
//	ini = dictionary_new(0);
//	dictionary_set(ini, "tp", NULL);
//	dictionary_set(ini, "ctp", NULL);
#endif
	theConfig.deviceAddress = iniparser_getint(ini, "BassicSetup:deviceAddress",  NULL);
	DEBUGINI("read deviceAddress:%d\n", value);
	theConfig.baudRate = iniparser_getint(ini, "BassicSetup:baudRate",  NULL);
	DEBUGINI("read baudRate:%d\n", value);
	strncpy(theConfig.daySetting, iniparser_getstring(ini, "BassicSetup:daySetting", NULL), sizeof (theConfig.daySetting) - 1);
 	strncpy(theConfig.timeSetting, iniparser_getstring(ini, "BassicSetup:timeSetting", NULL), sizeof (theConfig.timeSetting) - 1);
	strncpy(theConfig.password, iniparser_getstring(ini, "BassicSetup:password", NULL), sizeof (theConfig.password) - 1);
	DEBUGINI("daySetting:%s\n", theConfig.daySetting);
	DEBUGINI("timeSetting:%s\n", theConfig.timeSetting);
	DEBUGINI("password:%s\n", theConfig.password);
	theConfig.chancel4_20MA = iniparser_getint(ini, "EquipmentManage:chancel4_20MA",  NULL);
	theConfig.rs485MinAddress = iniparser_getint(ini, "EquipmentManage:rs485MinAddress",  NULL);
	theConfig.rs485MaxAddress = iniparser_getint(ini, "EquipmentManage:rs485MaxAddress",  NULL);
	strncpy(theConfig.tcpMinIP, iniparser_getstring(ini, "EquipmentManage:tcpMinIP", NULL), sizeof (theConfig.tcpMinIP) - 1);
 	strncpy(theConfig.tcpMaxIP, iniparser_getstring(ini, "EquipmentManage:tcpMaxIP", NULL), sizeof (theConfig.tcpMaxIP) - 1);
	DEBUGINI("chancel4_20MA:%d\n", theConfig.chancel4_20MA);
	DEBUGINI("rs485MinAddress:%d\n", theConfig.rs485MinAddress);
	DEBUGINI("tcpMinIP:%s\n", theConfig.tcpMinIP);
	DEBUGINI("tcpMaxIP:%s\n", theConfig.tcpMaxIP);
	sprintf(temp, iniparser_getstring(ini, "StorageFunction:dataFun", NULL));
	if((strcmp(temp, "true") == 0) ||
		(strcmp(temp, "TRUE") == 0)){
		DEBUGINI("data fun open!\n");
	}else if((strcmp(temp, "false") == 0) ||
		(strcmp(temp, "FALSE") == 0)){
		DEBUGINI("data fun close!\n");
	}
 	theConfig.dataCycl = iniparser_getint(ini, "StorageFunction:dataCycl",  NULL);

	theConfig.printMode = iniparser_getint(ini, "DataPrint:printMode",  NULL);
	theConfig.dataPrintCycle = iniparser_getint(ini, "DataPrint:dataPrintCycle",  NULL);
	theConfig.dataPrintContent = iniparser_getint(ini, "DataPrint:dataPrintContent",  NULL);
	DEBUGINI("dataCycl:%d\n", theConfig.dataCycl);
	DEBUGINI("printMode:%d\n", theConfig.printMode);
	DEBUGINI("dataPrintCycle:%d\n", theConfig.dataPrintCycle);
	DEBUGINI("dataPrintContent:%d\n", theConfig.dataPrintContent);
	sprintf(temp, iniparser_getstring(ini, "MSGMode:MSGSwitch", NULL));
	if((strcmp(temp, "yes") == 0) ||
		(strcmp(temp, "YES") == 0)){
		DEBUGINI("MSG open!\n");
	}else if((strcmp(temp, "NO") == 0) ||
		(strcmp(temp, "no") == 0)){
		DEBUGINI("MSG close!\n");
	}
	sprintf(theConfig.msgNum0, iniparser_getstring(ini, "MSGMode:msgNum0", NULL));
	sprintf(theConfig.msgNum1, iniparser_getstring(ini, "MSGMode:msgNum1", NULL));
	sprintf(theConfig.msgNum2, iniparser_getstring(ini, "MSGMode:msgNum2", NULL));
	sprintf(theConfig.msgNum3, iniparser_getstring(ini, "MSGMode:msgNum3", NULL));
	sprintf(theConfig.msgNum4, iniparser_getstring(ini, "MSGMode:msgNum4", NULL));
	sprintf(theConfig.msgNum5, iniparser_getstring(ini, "MSGMode:msgNum5", NULL));
	DEBUGINI("msgNum0:%s\n", theConfig.msgNum0);
	DEBUGINI("msgNum1:%s\n", theConfig.msgNum1);
	DEBUGINI("msgNum2:%s\n", theConfig.msgNum2);
	DEBUGINI("msgNum3:%s\n", theConfig.msgNum3);
	DEBUGINI("msgNum4:%s\n", theConfig.msgNum4);
	DEBUGINI("msgNum5:%s\n", theConfig.msgNum5);

	return 1;
}

#if 0
void UpdateTPCal()
{	
	char buf[20];

	memset(buf, '\0', sizeof(buf));
	
	if(LsSaveCtp == 1){
		iniparser_set(ini, "tp:ls_save_tp",  "1");
	}else if (LsSaveCtp == 0){
		iniparser_set(ini, "tp:ls_save_tp",  "0");
	}
	sprintf(buf, "%d", coord[0][0]);
	iniparser_set(ini, "tp:tp_cal_x0",  buf);
	sprintf(buf, "%d", coord[0][1]);
	iniparser_set(ini, "tp:tp_cal_y0",  buf);
	sprintf(buf, "%d", coord[1][0]);	
	iniparser_set(ini, "tp:tp_cal_x1",  buf);
	sprintf(buf, "%d", coord[1][1]);
	iniparser_set(ini, "tp:tp_cal_y1",  buf);	
	SaveConfigFile(INIFILENAME, ini);
	sysprintf("save data success!\n");
}
#endif
