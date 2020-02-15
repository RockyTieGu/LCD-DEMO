/****************************************************************************
*                                                                           *
* Copyright (c) 2009 Nuvoton Tech. Corp. All rights reserved.               *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   libadc.c
*
* VERSION
*   1.0
*
* DESCRIPTION
*   ADC touch screen library source file
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*
* HISTORY
*
* REMARK
*   None
****************************************************************************/
#include "W55fa93_adc.h"
#include "wb_include.h"

//#include "wb_reg.h"
//#include "wb_timer.h"

/* Verbose debug information */
#define DBG_PRintF(...)		//sysprintf 

#define SORT_FIFO			4	

/* Define the operation state machine */
#define E_ADC_UNINIT	0
#define E_ADC_INIT		1
#define E_ADC_OPEN	2

#define ADC_STATE_WT		0  // waiting for trigger mode
#define ADC_STATE_AUTO	1  // auto

static unsigned char _opened = E_ADC_UNINIT;
// These two will always store the latest sampled value

static unsigned char volatile _state;
static int i32_x=0, i32_y=0;
//#define USING_int
#define Debug //sysprintf
typedef enum{ 
	eADC_BAND_P0P5 = 0,                                             
	eADC_BAND_P0P75
}E_ADC_UPBAND;
    
typedef enum{ 
	eADC_BAND_N0P5 = 0,                                             
	eADC_BAND_N0P75
}E_ADC_DOWNBAND;   

typedef enum{ 
	eADC_PRE_P0 = 0,                                             
	eADC_PRE_P8,
	eADC_PRE_P14,
	eADC_PRE_P20
}E_ADC_PREGAIN;  

typedef enum{ 
	eADC_NG_N30 = 0,                                             
	eADC_NG_N36,
	eADC_NG_N42,
	eADC_NG_N48
}E_ADC_NOISEGATE;  

typedef enum{ 
	eADC_POST_N12 = 0,                                             
	eADC_POST_N11P25,
	eADC_POST_N10P5,
	eADC_POST_N9P75,
	eADC_POST_N9,
	eADC_POST_N8P25,
	eADC_POST_N7P5,	
	eADC_POST_N6P75,
	eADC_POST_N6,
	eADC_POST_N5P25,
	eADC_POST_N4P5,
	eADC_POST_N3P75,
	eADC_POST_N3,
	eADC_POST_N2P25,
	eADC_POST_N1P5,
	eADC_POST_N0P75,
	
	eADC_POST_P0,
	eADC_POST_P0P75,
	eADC_POST_P1P5,
	eADC_POST_P2P25,
	eADC_POST_P3,
	eADC_POST_P3P75,
	eADC_POST_P4P5,
	eADC_POST_P5P25,
	eADC_POST_P6,
	eADC_POST_P6P75,
	eADC_POST_P7P5,
	eADC_POST_P8P25,
	eADC_POST_P9,
	eADC_POST_P9P75,
	eADC_POST_P10P5,
	eADC_POST_P11P25,
	
	eADC_POST_P12,
	eADC_POST_P12P75,
	eADC_POST_P13P5,
	eADC_POST_P14P25,
	eADC_POST_P15,
	eADC_POST_P15P75,
	eADC_POST_P16P5,
	eADC_POST_P17P25,
	eADC_POST_P18,
	eADC_POST_P18P75,
	eADC_POST_P19P5,
	eADC_POST_P20P25,
	eADC_POST_P21,
	eADC_POST_P21P75,
	eADC_POST_P22P5,
	eADC_POST_P23P25,
	
	eADC_POST_P24,
	eADC_POST_P24P75,
	eADC_POST_P25P5,
	eADC_POST_P26P25,
	eADC_POST_P27,
	eADC_POST_P27P75,
	eADC_POST_P28P5,
	eADC_POST_P29P25,
	eADC_POST_P30,
	eADC_POST_P30P75,
	eADC_POST_P31P5,
	eADC_POST_P32P25,
	eADC_POST_P33,
	eADC_POST_P33P75,
	eADC_POST_P34P5,
	eADC_POST_P35P25
}E_ADC_POSTGAIN;

typedef enum{                            
	eADC_TSCREEN_4WIRE = 0,         	  
	eADC_TSCREEN_5WIRE,     /* FA93 reserved for the item*/
	eADC_TSCREEN_8WIRE,	/* FA93 reserved for the item*/
	eADC_TSCREEN_UNUSED   /* FA93 reserved for the item*/                                                  
}E_ADC_TSC_TYPE; 

typedef void (*PFN_ADC_CALLBACK)(void);	
static PFN_ADC_CALLBACK g_psADCCallBack[4]={0, 0, 0, 0};


//static void pfnRecordCallback(void)
//{
//	//sysprintf("hello,rocky!\n");
//}
static __inline
 void DrvADC_PollingADC(void)
{
	while( (inp32(REG_ADC_CON) & ADC_INT) != ADC_INT);	/* Wait until ADC int */ 
	 outp32(REG_ADC_CON, (inp32(REG_ADC_CON) & 	  /* Clean the ADC interrupt */     	
		        				~(WT_INT | LVD_INT)) |  
		        				(WT_INT |ADC_INT));
}
static __inline
 void DrvADC_PollingWT(void)
{
	while( (inp32(REG_ADC_CON) & WT_INT) != WT_INT);	/* Wait until ADC int */ 
	outp32(REG_ADC_CON, (inp32(REG_ADC_CON) &    /* Clean the touch panel interrupt */    
		        					~WT_INT) );
	outp32(REG_ADC_CON, (inp32(REG_ADC_CON) &    /* Clean the touch panel interrupt */    
		        				~(LVD_INT | ADC_INT)) |  
		        				(WT_INT |ADC_INT));
}
static __inline char DrvADC_IsPenDown(void)
{
	if ( (inp32(REG_ADC_TSC) & ADC_UD) == ADC_UD)
		return TRUE;	//Pen down;
	else
		return FALSE;	//Pen up;		
}
/* =============================== sorting =============================== */
void swap(unsigned short *x,unsigned short *y) 
{   
	unsigned short temp; 
	temp = *x; 
	*x = *y; 
	*y = temp; 
} 
unsigned short choose_pivot(unsigned short i,unsigned short j ) 
{ 
	return((i+j) /2); 
} 
void quicksort(unsigned short list[],int m,int n) 
{    
	int key,i,j,k; 
	if( m < n) 
	{ 
		k = choose_pivot(m,n); 
		swap(&list[m],&list[k]); 
		key = list[m]; 
		i = m+1; 
		j = n; 
		while(i <= j) 
		{ 
			while((i <= n) && (list[i] <= key)) 
				i++; 
			while((j >= m) && (list[j] > key))                 
				j--; 
			if( i < j) 
				swap(&list[i],&list[j]); 
		} 
		// swap two elements 
		swap(&list[m],&list[j]); 
		// recursively sort the lesser list 
		quicksort(list,m,j-1); 
		quicksort(list,j+1,n);
	} 
} 

/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* FUNCTION                                                                                                */
/* 		DrvADC_EnableInt()		                                   	   	                                   */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*     	enable ADC interrupt and setup callback function 	        	                                   */
/*                                                                                                         */
/* INPUTS                                                                                                  */
/*      callback                                                                                           */
/*          The callback funciton                                                                          */
/*                                                                                                         */
/*      u32UserData                                                                                        */
/*          The user's data to pass to the callback function                                               */
/*                                                                                                         */
/* OUTPUTS                                                                                                 */
/*      none 														                                       */
/*                                                                                                         */
/* RETURN                                                                                                  */
/*      none														                                       */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
unsigned int  
adc_enableInt(
	E_ADC_int eIntType
	)
{
    /* Enable adc interrupt */
#ifdef USING_int    
    switch(eIntType)
    {
    	case eADC_ADC_int: 
		 	outp32(REG_ADC_CON, inp32(REG_ADC_CON) | ADC_INT_EN);	    		                                     
    	    break; 
	   	case eADC_AUD_int:
	   		outp32(REG_AUDIO_CON, inp32(REG_AUDIO_CON) | AUDIO_INT_EN);	 
	   		break;
		case eADC_LVD_int:
			outp32(REG_ADC_CON, inp32(REG_ADC_CON) | LVD_INT_EN);	 
			break;
		case eADC_WT_int:     
			outp32(REG_ADC_CON, inp32(REG_ADC_CON) | WT_INT_EN);	 
			break;	
		default:
			return -1;
    }			
#endif    
    return Successful;
}



unsigned int 
adc_disableInt(
	E_ADC_int eIntType
	)
{
    /* Enable adc interrupt */
    switch(eIntType)
    {
    	case eADC_ADC_int:
    			outp32(REG_ADC_CON, inp32(REG_ADC_CON) & ~ADC_INT_EN);	                                      
    	    	break; 
	   	case eADC_AUD_int:
	   		outp32(REG_AUDIO_CON, inp32(REG_AUDIO_CON) & ~AUDIO_INT_EN);	 
	   		break;
		case eADC_LVD_int:
			outp32(REG_ADC_CON, inp32(REG_ADC_CON) & ~LVD_INT_EN);	
			break;
		case eADC_WT_int:
			outp32(REG_ADC_CON, inp32(REG_ADC_CON) & ~WT_INT_EN);     
			break;	
		default:
			return Fail;
		break;
    }			
    return Successful;
}

/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* FUNCTION                                                                                                */
/* 		DrvADC_SetTouchScreen()	                      		                                               */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*     	Set the parameter for TSC														                   */
/*                                                                                                         */
/* INPUTS                                                                                                  */
/*      eTscMode    	Normal mode, Semi-Auto, Auto or Wait for trigger   					               */
/*      eTscWire 		4 wire, 5 wire, 8 wire or unused   					           					   */
/*      bIsPullup		Control the internal pull up PMOS in switch box									   */
/*      bMAVFilter      Enable or disable MAV filter in TSC auto mode                                      */
/* OUTPUTS                                                                                                 */
/*      none 														                                       */
/*                                                                                                         */
/* RETURN                                                                                                  */
/*      none																				               */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void adc_setTouchScreen(
	E_ADC_TSC_MODE eTscMode,
	unsigned int u32DleayCycle,
	char bIsPullup,
	char bMAVFilter	
	)
{  				
	//Change mode should reset ADC IP. otherwise, the ADC will hang-up
	outp32(REG_APBIPRST, inp32(REG_APBIPRST) | ADCRST);
	outp32(REG_APBIPRST, inp32(REG_APBIPRST) & ~ADCRST);	
	
	if(eTscMode==eADC_TSCREEN_AUTO)
		_state = ADC_STATE_AUTO;
	else if(eTscMode==eADC_TSCREEN_TRIG)
		_state = ADC_STATE_WT;
	outp32(REG_ADC_CON, inp32(REG_ADC_CON) & ~ADC_CON_ADC_EN);
    outp32(REG_ADC_CON, (inp32(REG_ADC_CON) & ~(ADC_TSC_MODE | ADC_MUX)) | 	
   				(eTscMode << 14) );
    				  				    				
    outp32(REG_ADC_DLY, u32DleayCycle); 				
    outp32(REG_ADC_TSC, (inp32(REG_ADC_TSC) & ~(ADC_TSC_TYPE | ADC_PU_EN | ADC_TSC_MAV_EN)) | 	
    				( (((eADC_TSCREEN_4WIRE << 1) & ADC_TSC_TYPE) | ((bIsPullup <<3) & ADC_PU_EN)) |
    				((bMAVFilter<<9) & ADC_TSC_MAV_EN) ));	
	outp32(REG_ADC_CON, inp32(REG_ADC_CON) | ADC_CON_ADC_EN);  											
}   
void DrvADC_Conversion(void)
{
	outp32(REG_ADC_CON, inp32(REG_ADC_CON) | ADC_CONV);	
}

void DrvADC_GetNormalData(unsigned short * pu16Data)
{  		
	*pu16Data = inp32(REG_ADC_XDATA);					     				     											     											     											     				
}

void DrvADC_GetTscData(
	unsigned short * pu16XData,
	unsigned short * pu16YData
	)
{  		
	*pu16XData = inp32(REG_ADC_XDATA);
	*pu16YData = inp32(REG_ADC_YDATA);					     				     											     											     											     				
}

static char g_bIsADCInt =FALSE;
static char g_bIsWTInt =FALSE;
static char g_bIsAudioInt =FALSE;

static void adc_isr(void)
{
    unsigned int u32Reg;
    
    u32Reg = inp32(REG_ADC_CON);
    
    /* Process ADC interrupt */
    if( (u32Reg & (ADC_INT_EN | ADC_INT)) == (ADC_INT_EN | ADC_INT))
    {//ADC interrupt
	if(g_psADCCallBack[eADC_ADC_int]!=0)
            	g_psADCCallBack[eADC_ADC_int]();   
        g_bIsADCInt = TRUE;           				
        outp32(REG_ADC_CON, (inp32(REG_ADC_CON) & 	  /* Clean the ADC interrupt */     	
        				~(WT_INT | LVD_INT)) |  
        				(WT_INT |ADC_INT));
		sysprintf("world!\n");
    	return;
    }      
    if( (u32Reg & (LVD_INT_EN | LVD_INT)) == (LVD_INT_EN | LVD_INT))
    {//LVD interrupt
    	if(g_psADCCallBack[eADC_LVD_int]!=0)
            	g_psADCCallBack[eADC_LVD_int]();           
        outp32(REG_ADC_CON, (inp32(REG_ADC_CON) & 	 /* Clean the LVD interrupt */
        				~(WT_INT | ADC_INT)) |  
        				LVD_INT);    
    	return;
    }  
    if( (u32Reg & (WT_INT_EN | WT_INT)) == (WT_INT_EN | WT_INT))
    {//Wait for trigger    
    	/*Disable WT int  */
    	if((inp32(REG_ADC_CON) & ADC_TSC_MODE)==ADC_TSC_MODE)
	    	adc_disableInt(eADC_WT_int);	
	    	sysprintf("hello!\n");		    	
    	if(g_psADCCallBack[eADC_WT_int]!=0)
            	g_psADCCallBack[eADC_WT_int]();
        g_bIsWTInt = TRUE;           
		outp32(REG_ADC_CON, (inp32(REG_ADC_CON) &    /* Clean the touch panel interrupt */    
        				~(LVD_INT | ADC_INT)) |  
        				(WT_INT |ADC_INT));
    	return;
    }    
    u32Reg = inp32(REG_AUDIO_CON);
    if( (u32Reg & (AUDIO_INT_EN | AUDIO_INT_EN)) == (AUDIO_INT_EN | AUDIO_INT_EN))
    {//Audio record interrupt
    	if(g_psADCCallBack[eADC_AUD_int]!=0)
            	g_psADCCallBack[eADC_AUD_int]();    	
        g_bIsAudioInt = TRUE;    	    	
        outp32(REG_AUDIO_CON, inp32(REG_AUDIO_CON) | AUDIO_INT_EN);	/* Clean the record interrupt */
    }   
}


unsigned int  
adc_installCallback(
	E_ADC_int eIntType,
	PFN_ADC_CALLBACK pfnCallback,
	PFN_ADC_CALLBACK* pfnOldCallback
	)
{
	switch(eIntType)
    	{
    		case eADC_ADC_int:
	    		*pfnOldCallback = g_psADCCallBack[eADC_ADC_int];
	    		g_psADCCallBack[eADC_ADC_int] = pfnCallback; 	                                      
	    	   	break; 
	   	case eADC_AUD_int:
	   		*pfnOldCallback = g_psADCCallBack[eADC_AUD_int];
    			g_psADCCallBack[eADC_AUD_int] = pfnCallback; 	 
	   		break;
		case eADC_LVD_int:
			*pfnOldCallback = g_psADCCallBack[eADC_LVD_int];
    			g_psADCCallBack[eADC_LVD_int] = pfnCallback; 
			break;
		case eADC_WT_int:
			*pfnOldCallback = g_psADCCallBack[eADC_WT_int];
    			g_psADCCallBack[eADC_WT_int] = pfnCallback;  
			break;	
		default:
			return E_ADC_INVALID_int;
    }			
    return Successful;
}

void adc_setWorkingFreq(unsigned int u32WorkingFreq)
{
	unsigned int u32WorkingKHz, u32UpllKHz, u32ApllKHz;
	unsigned int u32Div, u32Div0, u32Div1;
	unsigned int u32ADCSrc;
	
	u32WorkingKHz = sysGetExternalClock();
	u32UpllKHz = sysGetPLLOutputKhz(eSYS_UPLL, u32WorkingKHz);
	DBG_PRintF("UPLL output clock = %dMHz\n", u32UpllKHz/1000);
	u32ApllKHz = sysGetPLLOutputKhz(eSYS_APLL, u32WorkingKHz);
	DBG_PRintF("APLL output clock = %dMHz\n", u32ApllKHz/1000);
	if( (u32UpllKHz > 25000) )
	{//>25MHz. 
		u32ADCSrc = 0x18<<16;
		u32Div = u32UpllKHz/u32WorkingFreq;
		if( (u32UpllKHz%u32WorkingFreq) != 0)
			u32Div = u32Div +1;	
		u32WorkingKHz = 	u32UpllKHz/u32Div;	
	}
	else if (u32ApllKHz> 25000)		
	{//>25MHz 
		u32ADCSrc = 0x10<<16;
		u32Div = u32ApllKHz/u32WorkingFreq;
		if( (u32ApllKHz%u32WorkingFreq) !=0)
			u32Div = u32Div +1;
		u32WorkingKHz = 	u32ApllKHz/u32Div;	
	}
	 
	/* Search for fit divider */
	for(u32Div1=1; u32Div1<256; u32Div1=u32Div1+1)
	{
		for(u32Div0=1; u32Div0<8; u32Div0=u32Div0+1)
		{//u32Div0!=0
			if((u32Div0*u32Div1)==u32Div)
				break;
		}
		if((u32Div0*u32Div1)==u32Div)
				break;
	}	
	if(u32Div0>=1)
		u32Div0 = u32Div0 -1; 
	if( u32Div1>=1)
		u32Div1 = u32Div1 -1; 		
	DBG_PRintF("Total divider = %d\n", u32Div);
	DBG_PRintF("Div0 = %d, Div1 = %d \n", u32Div0, u32Div1);
	
	DBG_PRintF("ADC working frequency = %d\n", u32WorkingKHz);
	outp32(REG_CLKDIV3, (inp32(REG_CLKDIV3) & ~(ADC_N1 | ADC_S | ADC_N0)) | 
						((u32ADCSrc) | ((u32Div0<<16) | (u32Div1<<24))) );

}

/*-----------------------------------------------------------------------------------------------------------
	Function: adc_init                                                               
	The function initialize the ADC IP.
	1. Get the PLL as system clock source.   	
	2. Set the working frequency. Max frequency = 25MHz

-----------------------------------------------------------------------------------------------------------*/
void adc_init(void)
{
	//WB_CLKFREQ_T clk;
	//sysGetClockFreq(&clk);
	
	outp32(REG_APBCLK, inp32(REG_APBCLK) | ADC_CKE);	
	adc_setWorkingFreq(100*50);	
	
	outp32(REG_APBIPRST, inp32(REG_APBIPRST) | ADCRST);
	outp32(REG_APBIPRST, inp32(REG_APBIPRST) & ~ADCRST);
	outp32(REG_ADC_CON, inp32(REG_ADC_CON) | ADC_CON_ADC_EN);
	_opened = E_ADC_INIT;
}
/*-----------------------------------------------------------------------------------------------------------
	Function: adc_normalRead                                                               
	The function is use to converse the voltage(analog) to 10 bits gigital data  
	u32Channel = 2, 3, 4.

-----------------------------------------------------------------------------------------------------------*/
unsigned int adc_normalread(unsigned int u32Channel, unsigned short * pu16Data)
{
	unsigned short data[SORT_FIFO], u32Idx;
	
	if( (u32Channel>4)&&(u32Channel<2) )
		return E_DRVADC_INVALID_CHANNEL;
	if(_state != ADC_STATE_WT)	
		return E_DRVADC_INVALID_TIMING;
	
	adc_disableInt(eADC_WT_int);	
	adc_setTouchScreen(eADC_TSCREEN_AUTO,			//E_DRVADC_TSC_MODE eTscMode,
								0x150,
								TRUE,					//char bIsPullup,
								TRUE);
    	adc_enableInt(eADC_ADC_int);	
	
	outp32( REG_ADC_CON,( ADC_CON_ADC_EN | ADC_CONV | ADC_INT 
						| (u32Channel<<9)) );
	for(u32Idx = 0; u32Idx<SORT_FIFO; u32Idx=u32Idx+1)		
	{	
		DrvADC_Conversion();	
		while( (inp32(REG_ADC_CON) & ADC_INT) != ADC_INT);	/* Wait until ADC INT */ 
		DrvADC_PollingADC();
		DrvADC_GetNormalData(&data[u32Idx]);		
	}
	quicksort(data, 0, SORT_FIFO-1); 
	*pu16Data = (data[SORT_FIFO-4]+data[SORT_FIFO-3]+data[SORT_FIFO-2])/3;
	outp32(REG_ADC_CON, (inp32(REG_ADC_CON) & ~(ADC_TSC_MODE|ADC_MUX))|(ADC_INT | WT_INT)); 	
	/* set to WT mode */
	adc_setTouchScreen(eADC_TSCREEN_TRIG,					//E_DRVADC_TSC_MODE eTscMode,
								0x150,
								TRUE,						//char bIsPullup,
								TRUE);						//char bMAVFilter
	adc_enableInt(eADC_ADC_int);									
	adc_enableInt(eADC_WT_int);

	return Successful;					
}

/*-----------------------------------------------------------------------------------------------------------
	Function: adc_open                                                               
	The function open ADC driver
	1. Set the working frequency. Max frequency = 25MHz


-----------------------------------------------------------------------------------------------------------*/
PFN_ADC_CALLBACK pfnOldCallback;
int adc_open(unsigned char type, unsigned short hr, unsigned short vr)
{
	static unsigned short  _hr, _vr;
	
	if(_opened == 0)
		adc_init();
	if(_opened != 1)
		return(-1);
	_opened = 2;
		
	switch (type) 
	{
		
		case ADC_TS_4WIRE:	
			outp32(REG_APBCLK, inp32(REG_APBCLK) | ADC_CKE);		
			adc_setTouchScreen(eADC_TSCREEN_TRIG,		//E_DRVADC_TSC_MODE eTscMode,							
								0x180,
								TRUE,						//char bIsPullup,
								FALSE);						//MAV enable 
			outp32(REG_CLKDIV3, (inp32(REG_CLKDIV3) & ~(ADC_N1 | ADC_S | ADC_N0) ) | (1<<24));	//Come from XTAL directly 
			return (-1);	
			break;
			
	}
	_hr = hr;
	_vr = vr;
	sysInstallISR(IRQ_LEVEL_7, IRQ_ADC, (void *)adc_isr);	
	sysEnableInterrupt(IRQ_ADC);

//	adc_enableInt(eADC_ADC_int);							
//	adc_installCallback(eADC_ADC_int,
//							pfnRecordCallback,
//							&pfnOldCallback);
	_opened = E_ADC_OPEN;
	return(0);
}

void adc_close(void)
{
	if(_opened != 2)
		return;
	sysDisableInterrupt(IRQ_ADC);
	
	outp32(REG_ADC_CON, inp32(REG_ADC_CON) & ~ADC_CON_ADC_EN);
	outp32(REG_APBIPRST, inp32(REG_APBIPRST) | ADCRST);
	outp32(REG_APBIPRST, inp32(REG_APBIPRST) & ~ADCRST);
	outp32(REG_APBCLK, inp32(REG_APBCLK) & ~ADC_CKE);	
	
	_opened = E_ADC_UNINIT;	
	return;
}

void printlist(unsigned short list[],int n) 
{   
	int i; 
	for(i=0;i<n;i++) 	   
		sysprintf("%d\t",list[i]); 
	sysprintf("\n");
} 

/*-----------------------------------------------------------------------------------------------------------
	Function adc_read.
		Get touch panel position. 
	mode: ADC_NONBLOCK or ADC_BLOCK.
	x: X position
	y: Y position 
	
	return 0: No any position.  The (x, y) is unavilable.
		  1: Panel was touched. The (x, y) is avilable.  
-----------------------------------------------------------------------------------------------------------*/
int adc_read_init(unsigned char mode)
{
	volatile UINT32 u32Idx=0;
	
	if(_opened != 2)return(-1);//ȷ��adc�Ѵ�
				
	if(mode != ADC_NONBLOCK && mode != ADC_BLOCK)//�������Է��� 	
	{
		sysDisableInterrupt(IRQ_ADC); 	
		return(-1);
	}		

	/*0211 */
	outp32(REG_ADC_CON, (inp32(REG_ADC_CON) & ~ADC_TSC_MODE) | (2 << 14) ); 
	adc_disableInt(eADC_ADC_int);	
	
	/* set to WT mode */
	adc_setTouchScreen(eADC_TSCREEN_TRIG,					//E_DRVADC_TSC_MODE eTscMode,
								0x180,
								FALSE,						//BOOL bIsPullup,
								FALSE);						//BOOL bMAVFilter																		
	g_bIsWTInt = 0;	
	adc_enableInt(eADC_WT_int);	
	
	u32Idx=0x10;
	while(u32Idx--);	//��ʱһ��ʱ��
	
 return 1;
}

#if 1
/*-----------------------------------------------------------------------------------------------------------
	Function adc_read.
		Get touch panel position. 
	mode: ADC_NONBLOCK or ADC_BLOCK.
	x: X position
	y: Y position 
	
	return 0: No any position.  The (x, y) is unavilable.
		  1: Panel was touched. The (x, y) is avilable.  
-----------------------------------------------------------------------------------------------------------*/
int  adc_read(unsigned char mode, unsigned short *x, unsigned short *y)
{
	volatile unsigned int u32Idx=0;
	
	unsigned short au16XPos[SORT_FIFO];
	unsigned short au16YPos[SORT_FIFO];
	char bIsXnear, bIsYnear;
	//_state = ADC_STATE_WT;		
	if(_opened != 2)
		return(-1);		
			
	if(mode != ADC_NONBLOCK && mode != ADC_BLOCK) {
		sysDisableInterrupt(IRQ_ADC); 	
		return(-1);
	}	
	
	/* set to Auto mode */
	/*0211 */ 
	adc_disableInt(eADC_WT_int);
	/*0211 */ 
	
	g_bIsADCInt = 0;
	adc_setTouchScreen(eADC_TSCREEN_AUTO,			//E_DRVADC_TSC_MODE eTscMode,
								0x180,
								FALSE,					//char bIsPullup,
								FALSE);

/*0211 */  	
    adc_enableInt(eADC_ADC_int);	
/*0211 */    

	g_bIsADCInt = 0;
	DrvADC_Conversion();	
#ifdef USING_int		
	while(g_bIsADCInt==0);				
#else
	//Debug("1hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii\n");
	DrvADC_PollingADC( );
	//Debug("2iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiihi\n");
#endif									
	
	u32Idx = 0x100;
	while(u32Idx--);								
	for(u32Idx = 0; u32Idx<SORT_FIFO; u32Idx=u32Idx+1)	
	{	

		g_bIsADCInt = 0;					
		DrvADC_Conversion();	
#ifdef USING_int				
		while(g_bIsADCInt==0);
#else
		DrvADC_PollingADC( );
#endif		
		DrvADC_GetTscData(&au16XPos[u32Idx],&au16YPos[u32Idx]);	
	}																
		
	quicksort(au16XPos, 0, SORT_FIFO-1); 
    	quicksort(au16YPos, 0, SORT_FIFO-1); 
          
	*x = (au16XPos[SORT_FIFO-4]+au16XPos[SORT_FIFO-3]+au16XPos[SORT_FIFO-2])/3;
	*y = (au16YPos[SORT_FIFO-4]+au16YPos[SORT_FIFO-3]+au16YPos[SORT_FIFO-2])/3;
	bIsXnear = ((i32_x-16) <*x ) && (*x <(i32_x+16)); 
	bIsYnear = ((i32_y-16) <*y ) && (*y <(i32_y+16)); 
      // 	Debug("3iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiihi\n");  
        adc_setTouchScreen(eADC_TSCREEN_TRIG,			//E_DRVADC_TSC_MODE eTscMode,
								0x20,
								FALSE,						//char bIsPullup,
								FALSE);						//char bMAVFilter	 
        //	Debug("4iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiihi\n");  
   	i32_x = *x;
 	i32_y = *y;  
	return 1;			
}
#else
int tp_f=0;
int adc_read(unsigned char mode, unsigned short *x, unsigned short *y)
{
	volatile UINT32 u32Idx=0;
	
	UINT16 au16XPos[SORT_FIFO];
	UINT16 au16YPos[SORT_FIFO];
	BOOL bIsXnear, bIsYnear;



///////////////////////////////////////////////	
	if(mode == ADC_BLOCK)
	{		
    if(tp_f==0)
		{
		  adc_read_init(mode);
			tp_f=1;
			return 0;
		}
		else
    {			
			if( (inp32(REG_ADC_CON) & WT_INT) != WT_INT) return 0;	//û�д����ж� ����
		//	tp_f=0;
	
			outp32(REG_ADC_CON, (inp32(REG_ADC_CON) & ~WT_INT) );    /* Clean the touch panel interrupt */    											
			outp32(REG_ADC_CON, (inp32(REG_ADC_CON) &  ~(LVD_INT | ADC_INT)) | (WT_INT |ADC_INT));	  /* Clean the touch panel interrupt */    											 												
		}
	}	

	/*0211 */  
/////////////////////////////////////////////	
	/* set to Auto mode */
	/*0211 */ 
	adc_disableInt(eADC_WT_int);////��ADC WT�ж�
	/*0211 */ 
	
	g_bIsADCInt = 0;
	adc_setTouchScreen(eADC_TSCREEN_AUTO,			//E_DRVADC_TSC_MODE eTscMode,
								0x180,
								FALSE,					//BOOL bIsPullup,
								FALSE);

/*0211 */  	
    adc_enableInt(eADC_ADC_int);	//��ADC�ж�
/*0211 */    

	g_bIsADCInt = 0;
	DrvADC_Conversion();	//ת��
#ifdef USING_INT		
	while(g_bIsADCInt==0);				
#else
	DrvADC_PollingADC( );//�ȴ�ת����� ���� ��һ�ε�����
#endif									
	

	//OSTimeDly(1);
	sys5msDelay(200);
	
	for(u32Idx = 0; u32Idx<SORT_FIFO; u32Idx=u32Idx+1)	//6��
	{	

		g_bIsADCInt = 0;					
		DrvADC_Conversion();	//ת��
#ifdef USING_INT				
		while(g_bIsADCInt==0);
#else
		DrvADC_PollingADC( );//�ȴ�ת�����
#endif		
		DrvADC_GetTscData(&au16XPos[u32Idx],&au16YPos[u32Idx]);	//�õ�����
	}																
		
	    quicksort(au16XPos, 0, SORT_FIFO-1); //��˳��
    	quicksort(au16YPos, 0, SORT_FIFO-1); 
          	
	*x = (au16XPos[SORT_FIFO-3]+au16XPos[SORT_FIFO-2])/2;//ȥ�������Сƽ�� 
	*y = (au16YPos[SORT_FIFO-3]+au16YPos[SORT_FIFO-2])/2;	
	
	bIsXnear = ((i32_x-60) <*x ) && (*x <(i32_x+60)); 
	bIsYnear = ((i32_y-60) <*y ) && (*y <(i32_y+60)); //16 �� 60
         
        adc_setTouchScreen(eADC_TSCREEN_TRIG,			//E_DRVADC_TSC_MODE eTscMode,
								0x20,
								FALSE,						//BOOL bIsPullup,
								FALSE);						//BOOL bMAVFilter	 
          
   	i32_x = *x;
 	i32_y = *y; 

	if(mode == ADC_NONBLOCK)	
	{					
		//if(bIsXnear &&  bIsYnear)
		{	        
#ifdef USING_INT			
			if(g_bIsWTInt == 0)
#else
			if(DrvADC_IsPenDown()==FALSE)
#endif		
			{
				sysprintf("Library Pen up\n");	
				return 0;	//Means pen up.
			}	
			else
				return 1;	//Means pen down.
		}		
		return 0;					
	}	
	
	
	else
		
	
	{		
		if(bIsXnear &&  bIsYnear)
		{	        	
			return 1;
		}
		else
			return 0;		
	}	
			
}
#endif

/*---------------------------------------------------------------------------------------------------------
                                                                                                         
 FUNCTION                                                                                                
 		DrvADC_SetOffsetCancellation()	                      		                                       
                                                                                                         
 DESCRIPTION                                                                                             
      The function is only for OP offset callcellation				    								
                                                                                                         
 INPUTS                                                                                                  
      None        						    					                                       
                                                                                                         
 OUTPUTS                                                                                                 
      none 														                                       
                                                                                                         
 RETURN                                                                                                  
      Recording gain in dB.										                                       
                                                                                                         
---------------------------------------------------------------------------------------------------------*/
void DrvADC_SetOffsetCancellation(
	char bIsMuteEnable,
	char bIsOffsetCalibration,
	char bIsHardwareMode,
	unsigned int u32Offset
	)
{
	outp32(REG_OPOC, (inp32(REG_OPOC) & ~(MUTE_SW | OOC | OPOCM |OPOC_SW)) |
				 ((((bIsMuteEnable ? MUTE_SW:0) |
				 (bIsOffsetCalibration ? OOC:0)) |
				 (bIsHardwareMode ? OPOCM:0)) | 
				 ((u32Offset<<24)&OPOC_SW)) );
}	

void DrvADC_GetOffsetCancellation(
	char * pbIsMuteEnable,
	char * pbIsOffsetCalibration,
	char * pbIsHardwareMode,
	unsigned int * pu32Offset
	)
{
	unsigned int u32RegData = inp32(REG_OPOC);
	*pbIsMuteEnable = (u32RegData & MUTE_SW)>>31;
	*pbIsOffsetCalibration = (u32RegData & OOC)>>30;
	*pbIsHardwareMode = (u32RegData & OPOCM)>>29;
	*pu32Offset = (u32RegData & OPOC_SW)>>24;
}	

void DrvADC_SetOffsetCancellationEx(
	unsigned int u32SampleNumber,
	unsigned int u32DelaySampleCount
	)
{
	outp32(REG_OPOC, (inp32(REG_OPOC) & ~(OPOC_TCSN | OPOC_DSC)) |
				 (((u32SampleNumber<<16) & OPOC_TCSN) |
				 (u32DelaySampleCount & OPOC_DSC)) );
}
void DrvADC_GetOffsetCancellationEx(
	unsigned int * pu32SampleNumber,
	unsigned int * pu32DelaySampleCount
	)
{
	unsigned int u32RegData = inp32(REG_OPOC);	
	*pu32SampleNumber = u32RegData & OPOC_TCSN;
	*pu32DelaySampleCount = u32RegData & OPOC_DSC;
}


/*---------------------------------------------------------------------------------------------------------
                                                                                                         
 FUNCTION                                                                                                
 		ADC_SetNoiseGate()	                      		                                                   
                                                                                                         
 DESCRIPTION                                                                                             
     	Set Pre-Amplifer, Post-Amplifer and offset(Offset Cancellation					                   
                                                                                                         
 INPUTS                                                                                                  
      None        						    					                                       
                                                                                                         
 OUTPUTS                                                                                                 
      none 														                                       
                                                                                                         
 RETURN                                                                                                  
      Noise gate Level gain in -24, -30, -36, -42dB.										               
                                                                                                         
---------------------------------------------------------------------------------------------------------*/
void ADC_SetNoiseGate(
	char bIsEnable, 
	E_ADC_NOISEGATE eNoiseGateLevel
	)
{

     outp32(REG_AGC_CON, (inp32(REG_AGC_CON) & ~(NG_EN |NG_LEVEL)) | 
     				(((bIsEnable <<31)& NG_EN) |
     				((eNoiseGateLevel <<12)& NG_LEVEL)) );									     				
    				
}	

/*---------------------------------------------------------------------------------------------------------
                                                                                                         
 FUNCTION                                                                                                
 		ADC_GetNoiseGate()	                      		                                                   
                                                                                                         
 DESCRIPTION                                                                                             
     	Set Pre-Amplifer, Post-Amplifer and offset(Offset Cancellation					                   
                                                                                                         
 INPUTS                                                                                                  
      None        						    					                                       
                                                                                                         
 OUTPUTS                                                                                                 
      none 														                                       
                                                                                                         
 RETURN                                                                                                  
      Noise gate Level gain in -24, -30, -36, -42dB.										               
                                                                                                         
---------------------------------------------------------------------------------------------------------*/
void ADC_GetNoiseGate(
	char * pbIsEnable, 
	E_ADC_NOISEGATE* peNoiseGateLevel
	)
{
	unsigned int u32RegData = inp32(REG_AGC_CON);
	*pbIsEnable = (u32RegData & NG_EN)>>31;
	*peNoiseGateLevel = (E_ADC_NOISEGATE)((u32RegData & NG_LEVEL)>>12);    						     				
}	

/*---------------------------------------------------------------------------------------------------------
                                                                                                         
 FUNCTION                                                                                                
 		DrvADC_SetAutoGainTiming()	                      		                                           
                                                                                                         
 DESCRIPTION                                                                                             
     	Set the parameter for AGC														                   
                                                                                                         
 INPUTS                                                                                                  
      u32Period    	Detect max peak in the how many samples    					                       
      u32Attack 		      						    					           					   
      u32Recovery        	A band in the uper side from u32OutputLevel+-eUpBand						   
      u32Hold       A band in the buttom side from u32OutputLevel+-eUpBand					           
                                                                                                         
 OUTPUTS                                                                                                 
      none 														                                       
                                                                                                         
 RETURN                                                                                                  
      none																				               
                                                                                                         
---------------------------------------------------------------------------------------------------------*/
void ADC_SetAutoGainTiming(
	unsigned int u32Period,
	unsigned int u32Attack,
	unsigned int u32Recovery,
	unsigned int u32Hold
	)
{  			
     outp32(REG_AGC_CON, ( (inp32(REG_AGC_CON) & ~(PERIOD | ATTACK | RECOVERY| HOLD)) | 
     				( ((u32Period<<16) & PERIOD) |
     				((u32Attack<<8) & ATTACK)) ) |     				
     				( ((u32Recovery <<4)& RECOVERY) | 
     				(u32Hold & HOLD) ) );
    															     				     											     											     											     				
}	
void ADC_GetAutoGainTiming(
	unsigned int * pu32Period,
	unsigned int * pu32Attack,
	unsigned int * pu32Recovery,
	unsigned int * pu32Hold
	)
{  		

	unsigned int u32RegData = inp32(REG_AGC_CON);		
	*pu32Period = (u32RegData & PERIOD) >> 16;
	*pu32Attack = (u32RegData & ATTACK) >> 8;
	*pu32Recovery = (u32RegData & RECOVERY) >> 4;
	*pu32Hold = u32RegData & HOLD; 								     				     											     											     											     				
}   
/*---------------------------------------------------------------------------------------------------------
                                                                                                         
 FUNCTION                                                                                                
 		ADC_SetGainControl()	                      		                                               
                                                                                                         
 DESCRIPTION                                                                                             
     	Set Pre-Amplifer and Post-Amplifer					                   							   
                                                                                                         
 INPUTS                                                                                                  
      None        						    					                                       
                                                                                                         
 OUTPUTS                                                                                                 
      none 														                                       
                                                                                                         
 RETURN                                                                                                  
      Recording gain in dB.										                                       
                                                                                                         
---------------------------------------------------------------------------------------------------------*/
void ADC_SetGainControl(
	E_ADC_PREGAIN ePreGain, 
	E_ADC_POSTGAIN ePostGain
	)
{
     outp32(REG_AGCP1, (inp32(REG_AGCP1) & ~(AUDIO_VOL|PRAGA)) | 
     				((ePreGain<<8)|ePostGain)); 								     				
}	

void ADC_GetGainControl(
	E_ADC_PREGAIN* pePreGain, 
	E_ADC_POSTGAIN* pePostGain
	)
{
	unsigned int u32RegData = inp32(REG_AGCP1);
	*pePreGain =  (E_ADC_PREGAIN)((u32RegData & PRAGA)>>8);
	*pePostGain = (E_ADC_POSTGAIN)(u32RegData & AUDIO_VOL);							     				
}	

/*---------------------------------------------------------------------------------------------------------
                                                                                                         
 FUNCTION                                                                                                
 		ADC_SetAutoGainControl()	                      		                                               
                                                                                                         
 DESCRIPTION                                                                                             
     	Set the parameter for AGC														                   
                                                                                                         
 INPUTS                                                                                                  
      bIsEnable    	Enable AGC    						    					                       
      u32OutputLevel  Output target level      						    					           
      eUpBand        	A band in the uper side from u32OutputLevel+-eUpBand							   
      eDownBand       A band in the buttom side from u32OutputLevel+-eUpBand					           
                                                                                                         
 OUTPUTS                                                                                                 
      none 														                                       
                                                                                                         
 RETURN                                                                                                  
      none																				               
                                                                                                         
---------------------------------------------------------------------------------------------------------*/
void ADC_SetAutoGainControl(
	char bIsEnable, 
	unsigned int u32OutputLevel,
	E_ADC_UPBAND eAdcUpBand,
	E_ADC_DOWNBAND eAdcDownBand
	)
{
     outp32(REG_AGC_CON, (inp32(REG_AGC_CON) & ~AGC_EN) | 
     				((bIsEnable <<30)& AGC_EN) );     				
     outp32(REG_AGCP1, ( (inp32(REG_AGCP1) & ~(OTL | UPBAND | DOWNBAND)) | 
     				((u32OutputLevel<<12) & OTL) ) |
     				(((eAdcUpBand <<11)& UPBAND) | 
     				((eAdcDownBand <<10)& DOWNBAND)) );											     				     											     											     											     				
}	
	
/*---------------------------------------------------------------------------------------------------------
                                                                                                         
 FUNCTION                                                                                                
 		ADC_GetAutoGainControl()	                      		                                           
                                                                                                         
 DESCRIPTION                                                                                             
     	Set Pre-Amplifer, Post-Amplifer and offset(Offset Cancellation					                   
                                                                                                         
 INPUTS                                                                                                  
      None        						    					                                       
 OUTPUTS                                                                                                 
      bIsEnable    	Enable AGC    						    					                       
      u32OutputLevel  Output target level      						    					           
      eUpBand        	A band in the uper side from u32OutputLevel+-eUpBand							   
      eDownBand       A band in the buttom side from u32OutputLevel+-eUpBand					           
                                                                                                         
 RETURN                                                                                                  
      None.										               										   
                                                                                                         
---------------------------------------------------------------------------------------------------------*/
void ADC_GetAutoGainControl(
	char * pbIsEnable, 
	unsigned int * pu32OutputLevel,
	E_ADC_UPBAND* peAdcUpBand,
	E_ADC_DOWNBAND* peAdcDownBand
	)
{
	unsigned int u32RegData = inp32(REG_AGC_CON);
	*pbIsEnable = (u32RegData & AGC_EN)>>30;
	u32RegData = inp32(REG_AGCP1);
	*pu32OutputLevel = (u32RegData & OTL)>>12; 
	*peAdcUpBand = 	(E_ADC_UPBAND)((u32RegData & UPBAND)>>11); 					     				
	*peAdcDownBand = (E_ADC_DOWNBAND)((u32RegData & DOWNBAND)>>10); 						     				    						     						     				
}	
/*---------------------------------------------------------------------------------------------------------                                                                                                 
 FUNCTION                                                                                                
      Audio_Open()			                                                                           
                                                                                                         
 DESCRIPTION                                                                                             
      Open the ADC conversion or Audio record function  			                                       
                                                                                                         
 INPUTS                                                                                                  
      mode:   The work mode of ADC. It could be in normal                                                
              ADC conversion mode or audio recording mode		                                           
                                                                                                         
      u32ConvClock:                                                                                      
              If working in ADC_NORMAL mode, u32ConvClock is the                                         
              conversion rate.                                                                           
              If working in ADC_RECORD mode, u32ConvClock is the                                         
              sampling rate.                                                                             
                                                                                                         
 OUTPUTS                                                                                                 
      none                                                                                               
                                                                                                         
 RETURN                                                                                                  
      E_SUCCESS           Success			                                                               
      E_DRVADC_ARGUMENT   Wrong argument                                                                 
      E_DRVADC_CLOCK      Unable to output a suitable clock                                              
                                                                                                         
---------------------------------------------------------------------------------------------------------*/
char bIsAudioInitialize = FALSE;
int audio_Open(E_SYS_SRC_CLK eSrcClock, unsigned int u32ConvClock)
{
	unsigned int u32PllOutKHz, u32ExtFreq;
	unsigned int u32Tmp;
	unsigned int u32Reg;
	volatile unsigned int u32Dly=0x100;
           
    	 /* Enable clock and IP reset */
    	outp32(REG_APBCLK, inp32(REG_APBCLK) | ADC_CKE);
    	outp32(REG_APBIPRST, inp32(REG_APBIPRST) | ADCRST);
   	outp32(REG_APBIPRST,  inp32(REG_APBIPRST) & ~ADCRST);
	/* Default to use conv bit to control conversion */
	u32Reg = 0;
	u32Reg = u32Reg | (ADC_CON_ADC_EN); /* Enable ADC */

    	/* Use the same clock source as system */
 
	outp32(REG_CLKDIV3, (inp32(REG_CLKDIV3) & ~ADC_S) | 
					(eSrcClock << 19));						
	switch((char)eSrcClock)
	{
		case eSYS_X32K:	
					return -1;			/* Wrong clock source */			
					break;
		case eSYS_APLL:												
		case eSYS_UPLL:			
					{
						unsigned int u32TotalDiv;
						unsigned int u32IdxN0, u32IdxN1; 
						unsigned int u32IdxN00, u32IdxN11; 					
						u32ExtFreq = sysGetExternalClock();						
						u32PllOutKHz = sysGetPLLOutputKhz(eSrcClock, u32ExtFreq);	
						if(u32PllOutKHz==169333)
							u32PllOutKHz = 169344;					
						sysprintf("PLL clock = %d KHz\n", u32PllOutKHz);												
						//if(eDRVADC_Mode == eDRVADC_RECORD)
							u32TotalDiv = (u32PllOutKHz*1000)/(1280*u32ConvClock);
						//else
						//	u32TotalDiv = (u32PllOutKHz/50)/u32ConvClock;	
						
						if(u32TotalDiv>(8*256))						
							return -1;						
																					
						sysprintf("Total divider = %d\n", u32TotalDiv);	
						for(u32IdxN1=1;u32IdxN1<=256;u32IdxN1=u32IdxN1+1)
						{
							for(u32IdxN0=2;u32IdxN0 <= 8;u32IdxN0=u32IdxN0+1)								
							{//u32IdxN0 != 1
								if(u32TotalDiv==(u32IdxN0*u32IdxN1))
								{
									u32IdxN00 = u32IdxN0;
									u32IdxN11 = u32IdxN1;
									sysprintf("DIV_N1, DIV_N0  = %d, %d\n", u32IdxN11, u32IdxN00);										
									break; 
								}	
							}							
							if(u32TotalDiv==((u32IdxN00)*u32IdxN11))											
								break;
							
						}	
						u32Tmp = (inp32(REG_CLKDIV3) & ~(ADC_N1 | ADC_S | ADC_N0)) | 
										( (((u32IdxN11-1) <<24) | ((u32IdxN00-1) << 16) | (eSrcClock<<19) ));
						outp32(REG_CLKDIV3, u32Tmp);																					
					}					
					break;
		case eSYS_EXT:	
					{
						unsigned int u32ExtClk, u32AdcDivN1;
						u32ExtClk = sysGetExternalClock();										
						u32AdcDivN1 = (u32ExtClk)/u32ConvClock;	
						if(u32AdcDivN1>256)
							return -1;
						outp32(REG_CLKDIV3, (inp32(REG_CLKDIV3) & ~(ADC_N1 | ADC_N0)) |
										((u32AdcDivN1-1) <<24) );																													
					}									
					break;
	}														  

	outp32(REG_ADC_CON, u32Reg);

	/* Reset Record Function */	
	outp32(REG_AUDIO_CON, inp32(REG_AUDIO_CON) | AUDIO_RESET);
	while(u32Dly--);
	outp32(REG_AUDIO_CON, inp32(REG_AUDIO_CON) & ~(AUDIO_RESET));	    
	/* Default to Audio Interrupt Mode 3, op offset:b'1000, interrupt enabled */
	outp32(REG_AUDIO_CON, (AUDIO_HPEN | AUDIO_INT_EN | AUDIO_INT_EN |AUDIO_VOL_EN | AUDIO_RESET));
	outp32(REG_AUDIO_CON, (inp32(REG_AUDIO_CON) & ~(AUDIO_CCYCLE | AUDIO_RESET)) | (0x50<<16));//ADC cycle = 50

	/* Hardware offset calibration */       
	if(bIsAudioInitialize==FALSE)
	{
		bIsAudioInitialize = TRUE;
	        outp32(REG_AUDIO_CON, inp32(REG_AUDIO_CON) | 
			    			 ( AUDIO_INT_EN | AUDIO_EN ) );
	    	DrvADC_SetOffsetCancellation(FALSE,   //char bIsMuteEnable,
								FALSE, //char bIsOffsetCalibration,
								TRUE, //char bIsHardwareMode,
								0x10);	  //unsigned int u32Offset   
									  		   				  		   				
		DrvADC_SetOffsetCancellationEx(1,			//255 sample
									256);//Delay sample count   							
		{
			volatile unsigned int btime, etime;
			btime = etime = sysGetTicks(TIMER0);
			
			while( (etime - btime) <= 10 )
			{					
				etime = sysGetTicks(TIMER0);					
			}
		}			
        	outp32(REG_AUDIO_CON, inp32(REG_AUDIO_CON) & ~AUDIO_EN);
      	 	DrvADC_SetOffsetCancellation(FALSE,   //char bIsMuteEnable,
												FALSE, //char bIsOffsetCalibration,
												FALSE, //char bIsHardwareMode,
												0x10);	  //unsigned int u32Offset 
	}		
	
	/* Set ADC Parameter for audio recording*/
	
	//outp32(REG_AUDIO_CON, (inp32(REG_AUDIO_CON) & ~(AUDIO_INT_MODE)) | (1<<30));	     		           		    	 	    	    	 	    	 	    	 	    	    	    	 	    	    	 	    	 	    	       		           	    	    	 	    	    	 	    	 	    	 	    	    	    	 	    	    	 	    	 	    	 									
	outp32(REG_AUDIO_CON, (inp32(REG_AUDIO_CON) & ~(AUDIO_INT_MODE)) );	     	
	
    	ADC_SetNoiseGate(FALSE, eADC_NG_N36);			    
	ADC_SetAutoGainTiming(4, //Period
							4,  //Attack
							4,
							4);																												
    	ADC_SetGainControl(eADC_PRE_P14,			//Pre-gain 14db
							eADC_POST_P0);	//Post-gain is invalid if enable AGC 
							
    	ADC_SetAutoGainControl(TRUE,					//AGC enable
							13, 					//Output target aszOTLArray[13] = -9db 
					     		eADC_BAND_P0P5,
					     		eADC_BAND_N0P5); 
	
	outp32(0xb800E000, inp32(0xb800E000) & ~BIT8); //Audio data src from Audio buffer											       	    
	
	sysInstallISR(IRQ_LEVEL_7, IRQ_ADC, (void *)adc_isr);	
	sysEnableInterrupt(IRQ_ADC);	
		
	return 0;	
}
/*----------------------------------------------------------------------------------------------------------
 FUNCTION                                                                                                					
 		adc_StartRecord()		                                                                       			
                                                                                                         						
 DESCRIPTION                                                                                             					
     	Start to record Audio data. This function only can be used in                                      		
      ADC_RECORD mode.                                                                                   				
                                                                                                         						
 INPUTS                                                                                                  					
      none														                        
                                                                                                         						
 OUTPUTS                                                                                                 					
      none														                        
                                                                                                         						
 RETURN                                                                                                  					
      none				                                                                               				
                                                                                                         						
----------------------------------------------------------------------------------------------------------*/
void adc_StartRecord(void)
{
    //Enable Co-work with EDMA  	
    outp32(REG_AGCP1, inp32(REG_AGCP1) | EDMA_MODE);	

    // Clean int status for safe 
    outp32(REG_AUDIO_CON, inp32(REG_AUDIO_CON) | 
    				(AUDIO_RESET | AUDIO_INT_EN) );
	outp32(REG_AUDIO_CON, inp32(REG_AUDIO_CON) & 
					~(AUDIO_RESET | AUDIO_INT_EN) );
     //Enable record function 
#if 0     
    outp32(REG_AUDIO_CON, (inp32(REG_AUDIO_CON) & ~AUDIO_INT_MODE) |
    				(((1 << 30) &  AUDIO_INT_MODE) |
    			 	AUDIO_EN) );
#else
  outp32(REG_AUDIO_CON, (inp32(REG_AUDIO_CON) & ~AUDIO_INT_MODE) |
    				(((0 << 30) &  AUDIO_INT_MODE) |
    			 	AUDIO_EN) );	
#endif    			 	
    
}
/*----------------------------------------------------------------------------------------------------------
 FUNCTION                                                                                                					
 		adc_StopRecord()                                                                                			
                                                                                                         						
 DESCRIPTION                                                                                             					
     	Stop recording Audio data. This function only can be used in                                       		
      ADC_RECORD mode.                                                                                   				
                                                                                                         						
 INPUTS                                                                                                  					
      none														                        
                                                                                                         						
 OUTPUTS                                                                                                 					
      none														                        
                                                                                                         						
 RETURN                                                                                                  					
      none				                                                                               				
                                                                                                         						
----------------------------------------------------------------------------------------------------------*/
void adc_StopRecord(void)
{
    	//Disable record function 
    	 outp32(REG_AGCP1, inp32(REG_AGCP1) & ~EDMA_MODE);	
	outp32(REG_AUDIO_CON, inp32(REG_AUDIO_CON) & (~AUDIO_EN));
}
/*---------------------------------------------------------------------------------------------------------                                                                                                     
 FUNCTION                                                                                                
 		adc_GetRecordData()	                                                                           
                                                                                                         
 DESCRIPTION                                                                                             
     	Get the converted ADC value in ADC_RECORD mode    				                                   
                                                                                                         
 INPUTS                                                                                                  
      none														                                       
                                                                                                         
 OUTPUTS                                                                                                 
      none 														                                       
                                                                                                         
 RETURN                                                                                                  
      The pointer to the record data. The data length is 8 samples                                        
                                                                                                         
---------------------------------------------------------------------------------------------------------*/
short * 
adc_GetRecordData(
	void
	)
{
     	//Return the base address of converted data. There are 8 samples 
    	return (short *)REG_AUDIO_BUF0;
}