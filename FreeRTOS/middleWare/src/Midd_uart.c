#include "Midd_uart.h"
#include "wb_include.h"

void UART_Init(char num)
{
	WB_UART_T 	uart;
	
	if(num == LOW_UART_232)//低速串口 用于打印信息
    {
		sysUartPort(LOW_UART_232);	
		uart.uiFreq = 12000000;					//use XIN clock
		uart.uiBaudrate = 115200;
		uart.uiDataBits = WB_DATA_BITS_8;
		uart.uiStopBits = WB_STOP_BITS_1;
		uart.uiParity = WB_PARITY_NONE;
		uart.uiRxTriggerLevel = LEVEL_1_BYTE;
		sysInitializeUART(&uart);
	}else if(num == HIGHT_UART_485)//高速 串口 用于通信使用 RS485
	{
		sysUartPort(HIGHT_UART_485);	
		uart.uiFreq = 12000000;	 //use XIN clock
		uart.uiBaudrate = 115200;//256000;//460800;
		uart.uiDataBits = WB_DATA_BITS_8;
		uart.uiStopBits = WB_STOP_BITS_1;
		uart.uiParity = WB_PARITY_NONE;
		uart.uiRxTriggerLevel = LEVEL_1_BYTE;
		sysInitializeUART(&uart);
		//UART0_RS485_Init();
	}
	
}	
