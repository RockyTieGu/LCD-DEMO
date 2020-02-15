/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (c) Nuvoton Technology Corp. All rights reserved.                                             */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wb_include.h"
#include "RTC.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
#define RTC_GLOBALS

//#define RTC_DEBUG
#ifdef RTC_DEBUG
#define RTCDEBUG     DrvSIO_printf
#else
#define RTCDEBUG(...)
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Global file scope (static) variables                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
static PFN_RTC_CALLBACK  *g_pfnRTCCallBack_Tick    = NULL, *g_pfnRTCCallBack_Alarm   = NULL, *g_pfnRTCCallBack_PSWI   = NULL;
                   
static unsigned int volatile g_u32RTC_Count  = 0;
static CHAR g_chHourMode = 0;
static char volatile g_bIsEnableTickInt  = FALSE;
static char volatile g_bIsEnableAlarmInt = FALSE;

static unsigned int volatile g_u32Reg, g_u32Reg1,g_u32hiYear,g_u32loYear,g_u32hiMonth,g_u32loMonth,g_u32hiDay,g_u32loDay;
static unsigned int volatile g_u32hiHour,g_u32loHour,g_u32hiMin,g_u32loMin,g_u32hiSec,g_u32loSec;

/*---------------------------------------------------------------------------------------------------------*/
/* Functions                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     <RTC_ISR>                                                                                 */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               void                                                                                      */
/* Returns:                                                                                                */
/*               None                                                                                      */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* Description:                                                                                            */
/*               Install ISR to handle interrupt event                                                     */
/*---------------------------------------------------------------------------------------------------------*/

static void RTC_ISR (void)
{ 
    unsigned int volatile u32RegRIIR;

    u32RegRIIR = inp32(RIIR);
    if (u32RegRIIR & RTC_TICK_int)                                       /* tick interrupt occurred */
    {
          outp32(RIIR, RTC_TICK_int);
          g_u32RTC_Count++;                                              /* maintain RTC tick count */

          if (g_pfnRTCCallBack_Tick != NULL)                             /* execute tick callback function */
          {
              g_pfnRTCCallBack_Tick();
          }

    }

    if (u32RegRIIR & RTC_ALARM_int)                                     /* alarm interrupt occurred */
    {    
          outp32(RIIR, RTC_ALARM_int);

          RTC_Ioctl(0,RTC_IOC_DISABLE_int,RTC_ALARM_int,0);
           
          if (g_pfnRTCCallBack_Alarm != NULL)                            /* execute alarm callback function */
          {
              g_pfnRTCCallBack_Alarm();
          }
   }
    if (u32RegRIIR & RTC_PSWI_int)                                     /* alarm interrupt occurred */
    {    
          outp32(RIIR, RTC_PSWI_int);

         // RTC_Ioctl(0,RTC_IOC_DISABLE_int,RTC_PSWI_int,0);
           
          if (g_pfnRTCCallBack_PSWI != NULL)                            /* execute alarm callback function */
          {
              g_pfnRTCCallBack_PSWI();
          }
   }   
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     RTC_SetFrequencyCompensation                 	                                           */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               float number                                                                              */
/* Returns:                                                                                                */
/*               E_SUCCESS					Success.                                                       */
/*               E_RTC_ERR_FCR_VALUE		Wrong Compenation VALUE                                        */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*                                                                                                         */
/*               Set Frequecy Compenation Data                                                             */
/*---------------------------------------------------------------------------------------------------------*/
unsigned int RTC_SetFrequencyCompensation(FLOAT fnumber)
{
    int i32intergerPart  ;
    int i32fractionPart ;
    int i32RegInt,i32RegFra ;
   	unsigned int u32Reg;

    i32intergerPart = (int) (fnumber) ;
    i32fractionPart = ((int) ( ((fnumber - i32intergerPart) *100)) / 100);
    i32RegInt = i32intergerPart - RTC_FCR_REFERENCE ;
    i32RegFra = ((i32fractionPart) * 60);
    /*-----------------------------------------------------------------------------------------------------*/
    /* Judge Interger part is reasonable                                                                   */
    /*-----------------------------------------------------------------------------------------------------*/

    if ( (i32RegInt < 0) | (i32RegInt > 15) )
    {
        return E_RTC_ERR_FCR_VALUE ;
    }

    u32Reg = RTC_WriteEnable();
    if (u32Reg != 0)
    {
        return E_RTC_ERR_EIO ;
    }
    outp32(RTC_FCR, (unsigned int)(g_u32Reg >>8 | i32RegFra));

    return E_RTC_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     RTC_WriteEnable                                                                           */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               void                                                                                      */
/* Returns:                                                                                                */
/*               E_SUCCESS		            Success.                                                       */
/*               E_RTC_ERR_FAILED       	FAILED                                                         */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*                                                                                                         */
/*               Access PW to AER to make access other register enable                                     */
/*---------------------------------------------------------------------------------------------------------*/
unsigned int RTC_WriteEnable (void)
{
    int i32i;

    outp32(AER, RTC_WRITE_KEY);


    for (i32i = 0 ; i32i < RTC_WAIT_COUNT ; i32i++)
    {
        /*-------------------------------------------------------------------------------------------------*/
        /* check RTC_AER[16] to find out RTC write enable                                                  */
        /*-------------------------------------------------------------------------------------------------*/
        if ( inp32(AER) & 0x10000 )
        {
            break;
        }
    }

    if (i32i == RTC_WAIT_COUNT)
    {
        RTCDEBUG ("\nRTC: RTC_WriteEnable, set write enable FAILED!\n");
        return E_RTC_ERR_EIO;
    }

    return E_RTC_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     RTC_Init        	                                                                       */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               void                                                                                      */
/* Returns:                                                                                                */
/*               E_SUCCESS            	Success.                                                           */
/*               E_RTC_ERR_EIO			Initial RTC FAILED.    	                                           */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*                                                                                                         */
/*               Initial RTC and install ISR                                                               */
/*---------------------------------------------------------------------------------------------------------*/


unsigned int RTC_Init (void)
{
    int i32i;

    /*-----------------------------------------------------------------------------------------------------*/
    /* Initial time data struct and some parameters.                                                       */
    /*-----------------------------------------------------------------------------------------------------*/
    g_pfnRTCCallBack_Alarm = NULL;
    g_pfnRTCCallBack_Tick = NULL;
	g_pfnRTCCallBack_PSWI = NULL;
    g_u32RTC_Count = 0;
    /*-----------------------------------------------------------------------------------------------------*/
    /* When RTC is power on, write 0xa5eb1357 to RTC_INIR to reset all logic.                              */
    /*-----------------------------------------------------------------------------------------------------*/
    outp32(INIR, RTC_INIT_KEY);

    for (i32i = 0 ; i32i < RTC_WAIT_COUNT ; i32i++)
    {
        if ( inp32(INIR) & 0x01 )
        {            /* Check RTC_INIR[0] to find out RTC reset signal */
            break;

        }
    }
        
    if (i32i == RTC_WAIT_COUNT)
    {
        RTCDEBUG("\nRTC: RTC_Init, initial RTC FAILED!\n");
        return E_RTC_ERR_EIO;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Install RTC ISR                                                              					   */
    /*-----------------------------------------------------------------------------------------------------*/
     outp32( RIIR, RTC_ALL_int );
     
	sysInstallISR(IRQ_LEVEL_1, IRQ_RTC, (void *)RTC_ISR);	
	sysSetLocalInterrupt(ENABLE_IRQ);
	sysEnableInterrupt(IRQ_RTC);	    
	
    return E_RTC_SUCCESS;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     RTC_Open                                                                                  */
/*                                                                                                         */
/* Parameter:    RTC_TIME_DATA_T *sPt            Just Set Current_Timer   		                           */
/*                                                                                                         */
/* Returns:                                                                                                */
/*               E_SUCCESS        		Success.                                                           */
/*               E_RTC_ERR_EIO		Initial RTC FAILED.                             	                   */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               Just Set Current_Timer .                                                                  */
//*--------------------------------------------------------------------------------------------------------*/

unsigned int RTC_Open (RTC_TIME_DATA_T *sPt)
{
    unsigned int u32Reg;
    
    /*-----------------------------------------------------------------------------------------------------*/
    /* DO BASIC JUDGEMENT TO Check RTC time data value is reasonable or not.                               */
    /*-----------------------------------------------------------------------------------------------------*/
    if ( ((sPt->u32Year - RTC_YEAR2000) > 99)|
         ((sPt->u32cMonth == 0) || (sPt->u32cMonth > 12))|
         ((sPt->u32cDay   == 0) || (sPt->u32cDay   > 31)))
    {
        return E_RTC_ERR_CALENDAR_VALUE;
    }

    if (sPt->u8cClockDisplay == RTC_CLOCK_12)
    {
        if ( (sPt->u32cHour == 0) || (sPt->u32cHour > 12) )
        {
            return E_RTC_ERR_TIMESACLE_VALUE ;
        }
    }
    else if (sPt->u8cClockDisplay == RTC_CLOCK_24)
    {
        if (sPt->u32cHour > 23)
        {
            return E_RTC_ERR_TIMESACLE_VALUE ;
        }
    }
    else
    {
        return E_RTC_ERR_TIMESACLE_VALUE ;
    }

    if ((sPt->u32cMinute > 59) |
        (sPt->u32cSecond > 59) |
        (sPt->u32cSecond > 59))
    {
        return E_RTC_ERR_TIME_VALUE ;
    }
    if (sPt->u32cDayOfWeek > 6)
    {
        return E_RTC_ERR_DWR_VALUE ;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Important, call RTC_WriteEnable() before write data into any register.                              */
    /*-----------------------------------------------------------------------------------------------------*/
    g_u32Reg = RTC_WriteEnable();
    if (g_u32Reg != 0)
    {
        return E_RTC_ERR_EIO;
    }

    /*-----------------------------------------------------------------------------------------------------*/
    /* Second, set RTC time data.                                                                          */
    /*-----------------------------------------------------------------------------------------------------*/
    if (sPt->u8cClockDisplay == RTC_CLOCK_12)
    {
        g_chHourMode = RTC_CLOCK_12;
        RTC_WriteEnable();
        outp32(TSSR, RTC_CLOCK_12);

        /*-------------------------------------------------------------------------------------------------*/
        /* important, range of 12-hour PM mode is 21 upto 32                                               */
        /*-------------------------------------------------------------------------------------------------*/
        if (sPt->u8cAmPm == RTC_PM)
            sPt->u32cHour += 20;
    }
    else                                                                               /* RTC_CLOCK_24 */
    {
        g_chHourMode = RTC_CLOCK_24;
        RTC_WriteEnable();
        outp32(TSSR, RTC_CLOCK_24);
        RTCDEBUG ("RTC: 24-hour\n");
    }

    outp32(DWR, (unsigned int)sPt->u32cDayOfWeek);
    g_u32hiYear  = (sPt->u32Year - RTC_YEAR2000) / 10;
    g_u32loYear  = (sPt->u32Year - RTC_YEAR2000) % 10;
    g_u32hiMonth =  sPt->u32cMonth              / 10;
    g_u32loMonth =  sPt->u32cMonth              % 10;
    g_u32hiDay   =  sPt->u32cDay                / 10;
    g_u32loDay   =  sPt->u32cDay                % 10;
    u32Reg    = (g_u32hiYear << 20);
    u32Reg    |= (g_u32loYear << 16);
    u32Reg    |= (g_u32hiMonth << 12);
    u32Reg    |= (g_u32loMonth << 8);
    u32Reg    |= (g_u32hiDay << 4);
    u32Reg    |= g_u32loDay;
    g_u32Reg = u32Reg;
    outp32 (CLR, (unsigned int)g_u32Reg);

    g_u32hiHour  = sPt->u32cHour / 10;
    g_u32loHour  = sPt->u32cHour % 10;
    g_u32hiMin   = sPt->u32cMinute / 10;
    g_u32loMin   = sPt->u32cMinute % 10;
    g_u32hiSec   = sPt->u32cSecond / 10;
    g_u32loSec   = sPt->u32cSecond % 10;
    u32Reg     = (g_u32hiHour << 20);
    u32Reg    |= (g_u32loHour << 16);
    u32Reg    |= (g_u32hiMin << 12);
    u32Reg    |= (g_u32loMin << 8);
    u32Reg    |= (g_u32hiSec << 4);
    u32Reg    |= g_u32loSec;
    g_u32Reg = u32Reg;
    outp32(TLR, (unsigned int)g_u32Reg);
    
    RTC_WriteEnable();
   // u32Reg = inp32(TLR);
    while( inp32(TLR) != (unsigned int)g_u32Reg);
    

    return E_RTC_SUCCESS;



}


/*---------------------------------------------------------------------------------------------------------*/
/* Function:     RTC_Read          	                                                                       */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               eTime                    		Currnet_Timer/ Alarm_Time                                  */
/*               RTC_TIME_DATA_T *spt   		Time Data                                                  */
/* Returns:                                                                                                */
/*               E_SUCCESS               Success.                                                          */
/*               E_RTC_ERR_EIO			 Initial RTC FAILED.   		                                       */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               Read current date/time or alarm date/time from RTC                                        */
//*--------------------------------------------------------------------------------------------------------*/

unsigned int RTC_Read (E_RTC_TIME_SELECT eTime, RTC_TIME_DATA_T *sPt)
{
    unsigned int u32Tmp;

    g_u32Reg = RTC_WriteEnable();
    if (g_u32Reg != 0)
    {
        return E_RTC_ERR_EIO;
    }    
    
    sPt->u8cClockDisplay = inp32(TSSR);                               /* 12/24-hour */
    sPt->u32cDayOfWeek = inp32(DWR);                                   /* Day of week */

    switch (eTime)
    {
        case RTC_CURRENT_TIME:
        {
			g_u32Reg   = inp32(CLR);
            g_u32Reg1  = inp32(TLR);
            break;
		}
        case RTC_ALARM_TIME:
        {
			g_u32Reg   = inp32(CAR);
            g_u32Reg1  = inp32(TAR);
            break;
        }
        default:
        {
            return E_RTC_ERR_ENOTTY;
        }
    }


    g_u32hiYear  = (g_u32Reg & 0xF00000) >> 20;
    g_u32loYear  = (g_u32Reg & 0xF0000) >> 16;
    g_u32hiMonth = (g_u32Reg & 0x1000) >> 12;
    g_u32loMonth = (g_u32Reg & 0xF00) >> 8;
    g_u32hiDay   = (g_u32Reg & 0x30) >> 4;
    g_u32loDay   =  g_u32Reg & 0xF;

    u32Tmp = (g_u32hiYear * 10);
    u32Tmp+= g_u32loYear;
    sPt->u32Year   =   u32Tmp  + RTC_YEAR2000;
    
    u32Tmp = (g_u32hiMonth * 10);
    sPt->u32cMonth = u32Tmp + g_u32loMonth;
    
    u32Tmp = (g_u32hiDay * 10);
    sPt->u32cDay   =  u32Tmp  + g_u32loDay;


    g_u32hiHour = (g_u32Reg1 & 0x300000) >> 20;
    g_u32loHour = (g_u32Reg1 & 0xF0000) >> 16;
    g_u32hiMin  = (g_u32Reg1 & 0x7000) >> 12;
    g_u32loMin  = (g_u32Reg1 & 0xF00) >> 8;
    g_u32hiSec  = (g_u32Reg1 & 0x70) >> 4;
    g_u32loSec  =  g_u32Reg1 & 0xF;


    if (sPt->u8cClockDisplay == RTC_CLOCK_12)
    {
        u32Tmp = (g_u32hiHour * 10);
        u32Tmp+= g_u32loHour;
        sPt->u32cHour = u32Tmp;                                /* AM: 1~12. PM: 21~32. */

        if (sPt->u32cHour >= 21)
        {
            sPt->u8cAmPm = RTC_PM;
            sPt->u32cHour -= 20;
        }
        else
        {
            sPt->u8cAmPm = RTC_AM;
        }
        
        u32Tmp = (g_u32hiMin  * 10);
        u32Tmp+= g_u32loMin;
        sPt->u32cMinute = u32Tmp;
        
        u32Tmp = (g_u32hiSec  * 10);
        u32Tmp+= g_u32loSec;
        sPt->u32cSecond = u32Tmp;

    }
    else
    {   /* RTC_CLOCK_24 */
        u32Tmp = (g_u32hiHour * 10);
        u32Tmp+= g_u32loHour;
        sPt->u32cHour   = u32Tmp;
        
        u32Tmp = (g_u32hiMin  * 10);
        u32Tmp+= g_u32loMin;
        sPt->u32cMinute = u32Tmp;
        
        u32Tmp = (g_u32hiSec  * 10);
        u32Tmp+= g_u32loSec;
        sPt->u32cSecond = u32Tmp;
    }


    return E_RTC_SUCCESS;

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     RTC_Write                                             	                                   */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               eTime                    		Currnet_Timer/ Alarm_Time                                  */
/*               RTC_TIME_DATA_T *sPt     		Time Data                                                  */
/* Returns:                                                                                                */
/*               E_SUCCESS               Success.                                                          */
/*               E_RTC_ERR_EIO      	 Initial RTC FAILED.                                               */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               Read current date/time or alarm date/time from RTC                                        */
//*--------------------------------------------------------------------------------------------------------*/
unsigned int RTC_Write(E_RTC_TIME_SELECT eTime, RTC_TIME_DATA_T *sPt)
{
    unsigned int u32Reg;
    
    /*-----------------------------------------------------------------------------------------------------*/
    /* Check RTC time data value is reasonable or not.                                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    if ( ((sPt->u32Year - RTC_YEAR2000) > 99)|
         ((sPt->u32cMonth == 0) || (sPt->u32cMonth > 12))|
         ((sPt->u32cDay   == 0) || (sPt->u32cDay   > 31)))
    {
        RTCDEBUG ("\nRTC: Year value is incorrect\n");
        return E_RTC_ERR_CALENDAR_VALUE;
    }

    if ( (sPt->u32Year - RTC_YEAR2000) > 99 )
    {
        RTCDEBUG ("\nRTC: Year value is incorrect\n");
        return E_RTC_ERR_CALENDAR_VALUE;
    }

    if ( (sPt->u32cMonth == 0) || (sPt->u32cMonth > 12) )
    {
        RTCDEBUG ("\nRTC: Month value is incorrect\n");
        return E_RTC_ERR_CALENDAR_VALUE;
    }

    if ( (sPt->u32cDay == 0) || (sPt->u32cDay > 31) )
    {
        RTCDEBUG ("\nRTC: Day value is incorrect\n");
        return E_RTC_ERR_CALENDAR_VALUE;
    }

    if (sPt->u8cClockDisplay == RTC_CLOCK_12)
    {
        if ( (sPt->u32cHour == 0) || (sPt->u32cHour > 12) )
        {
            RTCDEBUG ("\nRTC: Hour value is incorrect\n");
            return E_RTC_ERR_TIME_VALUE;
        }
    }
    else if (sPt->u8cClockDisplay == RTC_CLOCK_24)
    {
        if (sPt->u32cHour > 23)
        {
            RTCDEBUG ("\nRTC: Hour value is incorrect\n");
            return E_RTC_ERR_TIME_VALUE;
        }
    }
    else
    {
        RTCDEBUG ("\nRTC: Clock mode is incorrect\n");
        return E_RTC_ERR_TIME_VALUE;
    }

    if (sPt->u32cMinute > 59)
    {
        RTCDEBUG ("\nRTC: Minute value is incorrect\n");
        return E_RTC_ERR_TIME_VALUE;
    }

    if (sPt->u32cSecond > 59)
    {
        RTCDEBUG ("\nRTC: Second value is incorrect\n");
        return E_RTC_ERR_TIME_VALUE;
    }

    if (sPt->u32cDayOfWeek > 6)
    {
        RTCDEBUG ("\nRTC: Day of week value is incorrect\n");
        return E_RTC_ERR_DWR_VALUE;
    }


    /*-----------------------------------------------------------------------------------------------------*/
    /* Important, call RTC_Open() before write data into any register.                                     */
    /*-----------------------------------------------------------------------------------------------------*/
    g_u32Reg = RTC_WriteEnable();
    if (g_u32Reg != 0)
    {
        return E_RTC_ERR_EIO;
    }

    switch (eTime)
    {

        case RTC_CURRENT_TIME:
            /*---------------------------------------------------------------------------------------------*/
            /* Second, set RTC time data.                                                                  */
            /*---------------------------------------------------------------------------------------------*/

            if (sPt->u8cClockDisplay == RTC_CLOCK_12)
            {
                g_chHourMode = RTC_CLOCK_12;
                outp32(TSSR, RTC_CLOCK_12);
                RTCDEBUG ("RTC: 12-hour\n");
                /*-----------------------------------------------------------------------------------------*/
                /* important, range of 12-hour PM mode is 21 upto 32                                       */
                /*-----------------------------------------------------------------------------------------*/
                if (sPt->u8cAmPm == RTC_PM)
                    sPt->u32cHour += 20;
            }
            else                                                                  /* RTC_CLOCK_24 */
            {
                g_chHourMode = RTC_CLOCK_24;
                outp32(TSSR, RTC_CLOCK_24);
                RTCDEBUG ("RTC: 24-hour\n");
            }

            outp32(DWR,(unsigned int) sPt->u32cDayOfWeek);

            g_u32hiYear  = (sPt->u32Year - RTC_YEAR2000) / 10;
            g_u32loYear  = (sPt->u32Year - RTC_YEAR2000) % 10;
            g_u32hiMonth = sPt->u32cMonth / 10;
            g_u32loMonth = sPt->u32cMonth % 10;
            g_u32hiDay   = sPt->u32cDay / 10;
            g_u32loDay   = sPt->u32cDay % 10;
            
            u32Reg = (g_u32hiYear << 20);
            u32Reg|= (g_u32loYear << 16);
            u32Reg|= (g_u32hiMonth << 12);
            u32Reg|= (g_u32loMonth << 8);
            u32Reg|= (g_u32hiDay << 4);
            u32Reg|= g_u32loDay;
            g_u32Reg = u32Reg;
            RTCDEBUG ("RTC: REG_RTC_CLR[0x%08x]\n", inp32(CLR));
            RTC_WriteEnable();
            outp32 (CLR, (unsigned int)g_u32Reg);

            g_u32hiHour  = sPt->u32cHour / 10;
            g_u32loHour  = sPt->u32cHour % 10;
            g_u32hiMin   = sPt->u32cMinute / 10;
            g_u32loMin   = sPt->u32cMinute % 10;
            g_u32hiSec   = sPt->u32cSecond / 10;
            g_u32loSec   = sPt->u32cSecond % 10;
            
            u32Reg = (g_u32hiHour << 20);
            u32Reg|= (g_u32loHour << 16);
            u32Reg|= (g_u32hiMin << 12);
            u32Reg|= (g_u32loMin << 8);
            u32Reg|= (g_u32hiSec << 4);
            u32Reg|= g_u32loSec;
            g_u32Reg = u32Reg;
            RTCDEBUG ("RTC: REG_RTC_TLR[0x%08x]\n", inp32(TLR));
			RTC_WriteEnable();
            outp32(TLR, (unsigned int)g_u32Reg);
            
//            u32Reg = inp32(TLR);
            RTC_WriteEnable();
            while(inp32(TLR) != (unsigned int)g_u32Reg);

            return E_RTC_SUCCESS;


         case RTC_ALARM_TIME:

            g_pfnRTCCallBack_Alarm = NULL;                                         /* Initial call back function.*/
            /*---------------------------------------------------------------------------------------------*/
            /* Second, set alarm time data.                                                                */
            /*---------------------------------------------------------------------------------------------*/
            g_u32hiYear = (sPt->u32Year - RTC_YEAR2000) / 10;
            g_u32loYear = (sPt->u32Year - RTC_YEAR2000) % 10;
            g_u32hiMonth = sPt->u32cMonth / 10;
            g_u32loMonth = sPt->u32cMonth % 10;
            g_u32hiDay = sPt->u32cDay / 10;
            g_u32loDay = sPt->u32cDay % 10;
            
            u32Reg = (g_u32hiYear << 20);
            u32Reg|= (g_u32loYear << 16);
            u32Reg|= (g_u32hiMonth << 12);
            u32Reg|= (g_u32loMonth << 8);
            u32Reg|= (g_u32hiDay << 4);
            u32Reg|= g_u32loDay;
            g_u32Reg = u32Reg;
            outp32(CAR, (unsigned int)g_u32Reg);

            if (g_chHourMode == RTC_CLOCK_12)
            {
                if (sPt->u8cAmPm == RTC_PM)       /* important, range of 12-hour PM mode is 21 upto 32 */
                    sPt->u32cHour += 20;
            }
            g_u32hiHour   = sPt->u32cHour / 10;
            g_u32loHour   = sPt->u32cHour % 10;
            g_u32hiMin  = sPt->u32cMinute / 10;
            g_u32loMin  = sPt->u32cMinute % 10;
            g_u32hiSec  = sPt->u32cSecond / 10;
            g_u32loSec  = sPt->u32cSecond % 10;
            
            u32Reg = (g_u32hiHour << 20);
            u32Reg|= (g_u32loHour << 16);
            u32Reg|= (g_u32hiMin << 12);
            u32Reg|= (g_u32loMin << 8);
            u32Reg|= (g_u32hiSec << 4);
            u32Reg|= g_u32loSec;
            
            g_u32Reg = u32Reg;
            outp32(TAR, (unsigned int)g_u32Reg);          
            
            /*---------------------------------------------------------------------------------------------*/
            /* Third, install alarm callback function.                                                     */
            /*---------------------------------------------------------------------------------------------*/
            if (sPt->pfnAlarmCallBack != NULL)
                g_pfnRTCCallBack_Alarm = sPt->pfnAlarmCallBack;
            /*---------------------------------------------------------------------------------------------*/
            /* Finally, enable alarm interrupt.                                                            */
            /*---------------------------------------------------------------------------------------------*/

            RTC_Ioctl(0,RTC_IOC_ENABLE_int,RTC_ALARM_int,0);
            
            u32Reg = inp32(TAR);
            RTC_WriteEnable();
            while(inp32(TAR) != (unsigned int)g_u32Reg);
            
            return E_RTC_SUCCESS;


       default:
        {
            return E_RTC_ERR_ENOTTY;
        }
    }

}



/*---------------------------------------------------------------------------------------------------------*/
/* Function:     RTC_Ioctl                   	                                                           */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               i32Num             Interface number.   always set 0                                       */
/*               eCmd           	Command.                                                               */
/*               u32Arg0            Arguments for the command.                                             */
/*               u32Arg1            Arguments for the command.                                             */
/* Returns:                                                                                                */
/*               E_SUCCESS        		 Success.                          		                           */
/*               E_RTC_ERR_ENOTTY        Command not support, or parameter incorrect.          		       */
/*               E_RTC_ERR_ENODEV        Interface number incorrect.                           		       */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               Support some commands for application.                                                    */
//*--------------------------------------------------------------------------------------------------------*/

unsigned int RTC_Ioctl (int i32Num, E_RTC_CMD eCmd, unsigned int u32Arg0, FLOAT fArg1)
{
    int i32Ret;
    unsigned int u32Reg;
    RTC_TICK_T *ptick;

    if (i32Num != 0)
        return E_RTC_ERR_ENODEV;

    switch (eCmd)
    {

        case RTC_IOC_IDENTIFY_LEAP_YEAR:
        {
            u32Reg = inp32(LIR);
            if (u32Reg & 0x01)
            {
                *(unsigned int *)u32Arg0 = RTC_LEAP_YEAR;
                RTCDEBUG("\nRTC: It's a leap year\n");
            }
            else
            {
                *(unsigned int *)u32Arg0 = 0;
                RTCDEBUG("\nRTC: It's not a leap year\n");
            }
            break;
        }
        case RTC_IOC_SET_TICK_MODE:
        {
            ptick = (RTC_TICK_T *) u32Arg0;
            
            if (g_bIsEnableTickInt== TRUE)
            {            
            	RTC_Ioctl(0,RTC_IOC_DISABLE_int,RTC_TICK_int,0);
            	g_bIsEnableTickInt = TRUE;
	        }    
            g_u32RTC_Count = 0;

            u32Reg = RTC_WriteEnable();
            if (u32Reg != 0)
            {
                return E_RTC_ERR_EIO ;
            }
            if (ptick->ucMode > RTC_TICK_1_128_SEC)                            /*Tick mode 0 to 7 */
            {
                return E_RTC_ERR_ENOTTY ;
            }
            
            outp32(TTR, ptick->ucMode);           
           
            
            if (ptick->pfnTickCallBack != NULL)
            {
                g_pfnRTCCallBack_Tick = ptick->pfnTickCallBack;
            }
            else
            {
                g_pfnRTCCallBack_Tick = NULL;
            }
            
            RTC_WriteEnable();
            while(inp32(TTR) != ptick->ucMode);
            /*---------------------------------------------------------------------------------------------*/
            /* Reset tick interrupt status if program enable tick interrupt before.                        */
            /*---------------------------------------------------------------------------------------------*/
            if (g_bIsEnableTickInt== TRUE)
            {

                RTC_Ioctl(0,RTC_IOC_ENABLE_int,RTC_TICK_int,0);
                return E_RTC_SUCCESS;
            }
            break;
        }

        case RTC_IOC_GET_TICK:
        {
            (*(unsigned int *)u32Arg0) = g_u32RTC_Count;
            break;
        }

        case RTC_IOC_RESTORE_TICK:
        {
            g_u32RTC_Count = 0;
            break;
        }

        case RTC_IOC_ENABLE_int:
        {
            int i32Ret;

            i32Ret = RTC_WriteEnable();
            if (i32Ret != 0)
    		{	
                return E_RTC_ERR_EIO;
            }
            switch ((RTC_int_SOURCE)u32Arg0)
            {

                case RTC_TICK_int:
                {
                    g_bIsEnableTickInt   = TRUE;
                    outp32(RIER, inp32(RIER) | RTC_TICK_int)  ;
                    break;
                }
                case RTC_ALARM_int:
                {
                    g_bIsEnableAlarmInt  = TRUE;
                    outp32(RIER, inp32(RIER) | RTC_ALARM_int) ;
                    break;
                }
                case RTC_PSWI_int:
                {
                    g_bIsEnableAlarmInt  = TRUE;
                    outp32(RIER, inp32(RIER) | RTC_PSWI_int) ;
                    break;
                }              
                default:
                {
                    return E_RTC_ERR_ENOTTY;

                }
            }
            break;
        }
        case RTC_IOC_DISABLE_int:
        {
            int i32Ret;

            i32Ret = RTC_WriteEnable();
            if (i32Ret != 0)
            {
                return E_RTC_ERR_EIO;
            }            
			switch ((RTC_int_SOURCE)u32Arg0)
            {
                case RTC_TICK_int:
                {
                    g_bIsEnableTickInt   = FALSE;
                    outp32( RIER, inp32(RIER) & (~RTC_TICK_int) );
                    outp32( RIIR, inp32(RIIR) & (RTC_TICK_int) );
                    break;
                }
                case RTC_ALARM_int:
                {
                    g_bIsEnableAlarmInt  = FALSE;
                    outp32( RIER, inp32(RIER) & (~RTC_ALARM_int) );
                    outp32( RIIR, inp32(RIIR) & (RTC_ALARM_int) );
                    break;
                }
                case RTC_PSWI_int:
                {
                    g_bIsEnableAlarmInt  = FALSE;
                    outp32( RIER, inp32(RIER) & (~RTC_PSWI_int) );
                    outp32( RIIR, inp32(RIIR) & (RTC_PSWI_int) );
                    break;
                }                

                case RTC_ALL_int:
                {
                    g_bIsEnableTickInt   = FALSE;
                    g_bIsEnableAlarmInt  = FALSE;
                    outp32( RIER, 0 );
                    outp32( RIIR, RTC_ALL_int );

                    break;
                }
                default:
                {
                    return E_RTC_ERR_ENOTTY;
                }
            }


            break;
        }

        case RTC_IOC_SET_FREQUENCY:
        {
            i32Ret= RTC_SetFrequencyCompensation(u32Arg0) ;
            if (i32Ret != 0)
            {
                return E_RTC_ERR_ENOTTY;
            }
            break;
        }
		case RTC_IOC_SET_POWER_ON:
		{
		    g_u32Reg = RTC_WriteEnable();
		    if (g_u32Reg != 0)
	    	{
	        	return E_RTC_ERR_EIO;
		    }
		    outp32(PWRON, inp32(PWRON) | 0x01);

		    break;
		}
		case RTC_IOC_SET_POWER_OFF:
		{
		    g_u32Reg = RTC_WriteEnable();
		    if (g_u32Reg != 0)
	    	{
	        	return E_RTC_ERR_EIO;
		    }
		    outp32(PWRON, (inp32(PWRON) & ~0x05) | 2);
			//outp32(REG_APBCLK, inp32(REG_APBCLK) & ~RTC_CKE);
			outp32(REG_AHBCLK,0);
			while(1);
			
		  //  break;
		}
		case RTC_IOC_SET_POWER_OFF_PERIOD:
		{
		    g_u32Reg = RTC_WriteEnable();
		    if (g_u32Reg != 0)
	    	{
	        	return E_RTC_ERR_EIO;
		    }
		    
			outp32(PWRON, (inp32(PWRON) & ~0xF0000) | ((u32Arg0 & 0xF) << 16));
			
			while(((inp32(PWRON) & 0xF0000)) != ((u32Arg0 & 0xF) << 16) );
		    break;
		}
		case RTC_IOC_ENABLE_HW_POWEROFF:
		{
		    g_u32Reg = RTC_WriteEnable();
		    if (g_u32Reg != 0)
	    	{
	        	return E_RTC_ERR_EIO;
		    }           		    
			outp32(PWRON, (inp32(PWRON) | 0x04));
			while(!(inp32(PWRON) & 0x04));
		    break;
		}
		case RTC_IOC_DISABLE_HW_POWEROFF:
		{
		    g_u32Reg = RTC_WriteEnable();
		    if (g_u32Reg != 0)
	    	{
	        	return E_RTC_ERR_EIO;
		    }           		    
			outp32(PWRON, (inp32(PWRON) & ~0x04));
			while((inp32(PWRON) & 0x04));
		    break;
		}		
		case RTC_IOC_SET_PSWI_CALLBACK:
		{     
		    g_u32Reg = RTC_WriteEnable();
		    if (g_u32Reg != 0)
	    	{
	        	return E_RTC_ERR_EIO;
		    }		       
		
			RTC_Ioctl(0, RTC_IOC_ENABLE_int, RTC_PSWI_int, 0);
			
            if (((PFN_RTC_CALLBACK  *) u32Arg0) != NULL)
            {
                g_pfnRTCCallBack_PSWI = (PFN_RTC_CALLBACK  *) u32Arg0;
            }
            else
            {
                g_pfnRTCCallBack_PSWI = NULL;
            }
           	break;
        }            				
		case RTC_IOC_GET_POWERKEY_STATUS:
		{
		    g_u32Reg = RTC_WriteEnable();
		    if (g_u32Reg != 0)
	    	{
	        	return E_RTC_ERR_EIO;
		    }
		    
		    g_u32Reg = RTC_WriteEnable();
		    if (g_u32Reg != 0)
	    	{
	        	return E_RTC_ERR_EIO;
		    }
		    
			if(inp32(PWRON) & 0x80)
				 *(unsigned int *)u32Arg0 = 1;
			else
				 *(unsigned int *)u32Arg0 = 0;	

		    break;
		}
        default:
        {
            return E_RTC_ERR_ENOTTY;
        }
    }

    return E_RTC_SUCCESS;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function:     RTC_Close      	                                                                        */
/*                                                                                                         */
/* Parameter:                                                                                              */
/*               void            							                                               */
/* Returns:                                                                                                */
/*               E_SUCCESS                Success.                                                         */
/*               E_RTC_ERR_ENODEV 	      Interface number incorrect.                                      */
/* Side effects:                                                                                           */
/*                                                                                                         */
/* DESCRIPTION                                                                                             */
/*               Disable AIC channel of RTC and both tick and alarm interrupt..                             */
/*---------------------------------------------------------------------------------------------------------*/

unsigned int RTC_Close (void)
{

    g_bIsEnableTickInt = FALSE;
    
   	sysDisableInterrupt(IRQ_RTC);
   

    RTC_Ioctl(0,RTC_IOC_DISABLE_int,RTC_ALL_int,0);


    return E_RTC_SUCCESS;
}
int CheckTime(E_RTC_TIME_SELECT eTime, RTC_TIME_DATA_T * t)
{
	RTC_TIME_DATA_T sCurTime;

	if ( RTC_Read(eTime, &sCurTime) == E_RTC_SUCCESS )
	{				//24*60
		if( (sCurTime.u32cHour * 60 + sCurTime.u32cMinute) < 1440)
		{
			t->u32cDay = sCurTime.u32cDay;
			t->u8cAmPm = sCurTime.u8cAmPm;
			t->u32cMonth = sCurTime.u32cMonth;
			t->u32cDayOfWeek = sCurTime.u32cDayOfWeek;
			t->u32Year = sCurTime.u32Year;
			t->u8cClockDisplay = sCurTime.u8cClockDisplay;
			t->u32cSecond = sCurTime.u32cSecond;
			t->u32cHour = sCurTime.u32cHour;
			t->u32cMinute = sCurTime.u32cMinute;
			return 0;
		}
	}
	return 0;
}
extern unsigned int RTC_Init(void);
extern unsigned int RTC_Open(RTC_TIME_DATA_T *sPt);
extern unsigned int RTC_Ioctl(int nNum, E_RTC_CMD uCmd, unsigned int uArg0, FLOAT fArg1);
extern int CheckTime(E_RTC_TIME_SELECT eTime, RTC_TIME_DATA_T * t);
extern unsigned int RTC_Read(E_RTC_TIME_SELECT eTime, RTC_TIME_DATA_T *sPt);
extern unsigned int RTC_Write(E_RTC_TIME_SELECT eTime, RTC_TIME_DATA_T *sPt);
extern unsigned int RTC_Close(void);



