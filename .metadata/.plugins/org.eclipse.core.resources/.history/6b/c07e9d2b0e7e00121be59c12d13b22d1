/*
 * i2c_mark1.c
 *
 *  Created on: Dec 23, 2012
 *      Author: MJ
 */
#include "System.h"


void BB_conifg_start( void){
	uint32_t i = 0;

	//First Bring the PROG_B low
   CFG_PROG_B_ACTIVE;
	for(i=0;i<20000;i++)	//delay @48Mhz = 400uS
	{
	}
	CFG_PROG_B_INACTIVE;	//Bring high

	i = 0;
	while(CFG_INIT_B_STATE == 0)	//wait for INIT_B TO go high
	{
		i++;
		if(i>800000)
		{
		 SetLED_BlinkRate(1000);	//set blink rate to fail!
			return CONFIG_ERROR_INIT_NOT_HIGH;
		}
	}


}//func



int BB_cofig_done_stat( void){

	return(CFG_DONE_STATE);
}


