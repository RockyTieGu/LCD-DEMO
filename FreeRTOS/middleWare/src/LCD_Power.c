#include "LCD_Power.h"

void BackLightAdj(unsigned char ulight)
{
	PWM_TIME_DATA_T sPt;				//Define PWM Time Data Struct
	PWM_Enable(PWM_TIMER0,FALSE);						
	/* PWM Timer property */ 
	sPt.u8Mode = PWM_TOGGLE_MODE;
	sPt.fFrequency = 10000;
	sPt.u8HighPulseRatio = ulight ; 	// ulight   /* High Pulse peroid : Total Pulse peroid = 1 : 100 */ 
	
	sPt.bInverter = FALSE;								
	/* Set PWM Timer 0 Configuration */
	PWM_SetTimerClk(PWM_TIMER0,&sPt);							

	/* Enable Output for PWM Timer 0 */
	PWM_SetTimerIO(PWM_TIMER0,TRUE);				
	PWM_Enable(PWM_TIMER0,TRUE);
}
