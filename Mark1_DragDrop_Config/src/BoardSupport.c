/*
 * BoardSupport.c
 */

#include "System.h"



/*************************************************************************
 *DESCRIPTION: 	Setup GIO for the LPC
 *************************************************************************/
void InitGPIO()
{
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<6);  	//Enable AHB clock to the GPIO domain.

	//SETUP PIN TRIS OUTPUTS
	LPC_GPIO1->DIR |= CFG_PROG_B_LOC;
	LPC_GPIO2->DIR |= LED0_LOC;
	LPC_GPIO3->DIR |= LED1_LOC;
	LPC_GPIO3->DIR |= CFG_CCLK_LOC;
	LPC_GPIO3->DIR |= CFG_DIN_LOC;
	LPC_GPIO3->DIR |= FPGA_SUSPEND_LOC;

	LPC_GPIO0->DIR &= ~PB_LOC;			//0 = input 0.1
	LPC_IOCON->PIO0_1 = 0;				//disable pullup/down hyst, open drain
		
	LPC_GPIO1->DIR &= ~CFG_INIT_B_LOC;
	LPC_GPIO1->DIR &= ~CFG_DONE_LOC;

	LPC_GPIO2->DIR |= CFG_MUX_SEL_LOC;		//OUTPUT
	LPC_GPIO2->DIR |= CFG_MODE1_LOC;		//OUTPUT
	LPC_GPIO0->DIR |= FPGA_CS_LOC;			//OUTPUT

	LPC_GPIO0->DIR &= ~(1<<1);			//0 = INPUT
	LPC_IOCON->PIO0_1= 0x00;			//clear all io options

	//SETUP MASTER SENSE PIN
	LPC_GPIO2->DIR &= ~MASTER_SENSE_LOC; 	//INPUT
	//LPC_IOCON->PIO2_10 &= ~(1<<4);		//disable pullup/down hyst, open drain
	//LPC_IOCON->PIO2_10 = 0;					//disable pullup/down hyst, open drain
	LPC_IOCON->PIO2_10 = 1<<3;					//pull down enabled,  stop from floating up when no host is connected


	// I2C PINS SETUP IN I2CSLAVE INIT
	
	//SETUP LOGIC LEVEL ON PINS
	FPGA_SUSPEND_INACTIVE;
	CFG_PROG_B_INACTIVE;
	CFG_CCLK_INACTIVE;
	CFG_DIN_INACTIVE;
	CFG_MODE1_SLAVE;		//configure in serial slave mode
	CFG_MUX_LPC_CTL;			//select LPC as serial configuration device

}//INITGPIO


/*************************************************************************
 *DESCRIPTION: 	Setup GIO for the LPC
 *	- Setup shared pins to be HIZ to not conflict with other system components
 *************************************************************************/
void InitGPIO_Passive_Mode( void)
{
	//SETUP PIN AS HIZ INPUTS TO NOT CONFLICT WITH ANYTHING ELSE IS SYSTEM
	LPC_GPIO2->DIR &= ~MASTER_SENSE_LOC; 	//INPUT
	LPC_GPIO1->DIR &= ~CFG_PROG_B_LOC;
	LPC_GPIO3->DIR &= ~CFG_CCLK_LOC;
	LPC_GPIO3->DIR &= ~CFG_DIN_LOC;
	LPC_GPIO3->DIR &= ~FPGA_SUSPEND_LOC;
	LPC_GPIO1->DIR &= ~CFG_INIT_B_LOC;
	LPC_GPIO1->DIR &= ~CFG_DONE_LOC;
	LPC_GPIO0->DIR &= ~FPGA_CS_LOC;
	LPC_GPIO2->DIR &= ~CFG_MUX_SEL_LOC;		//this pin is pulled to proper state without being driven

	LPC_GPIO2->DIR |= CFG_MODE1_LOC;		//OUTPUT

	//SETUP LOGIC LEVEL ON PINS
	CFG_MODE1_SLAVE;		//configure in serial slave mode
	CFG_MUX_MASTER_CTL;			//select Master as serial config device for selection on mux

	//CFG_MODE1_MASTER; //SET MODE1 LOW TO SETUP FOR SELF CONFIG RATHER THAN SLAVE CONFIG BY DEFAULT

}


/*************************************************************************
 *DESCRIPTION: 	Setup GPIO FOR LPC MODE - This will not have startup gpio setup
 *************************************************************************/
void InitGPIO_LPC_Mode()
{
	//SETUP PIN TRIS OUTPUTS
	LPC_GPIO1->DIR |= CFG_PROG_B_LOC;	//output
	LPC_GPIO3->DIR |= CFG_CCLK_LOC;		//output
	LPC_GPIO3->DIR |= CFG_DIN_LOC;		//output
	LPC_GPIO3->DIR |= FPGA_SUSPEND_LOC;	//output
	LPC_GPIO2->DIR |= CFG_MUX_SEL_LOC;		//OUTPUT
	LPC_GPIO2->DIR |= CFG_MODE1_LOC;		//OUTPUT
	LPC_GPIO0->DIR |= FPGA_CS_LOC;			//OUTPUT

	//SETUP PIN TRIS INPUTS
	LPC_GPIO1->DIR &= ~CFG_INIT_B_LOC;
	LPC_GPIO1->DIR &= ~CFG_DONE_LOC;
	LPC_GPIO2->DIR &= ~MASTER_SENSE_LOC; 	//INPUT
	LPC_IOCON->PIO2_10 = 0x00;				//disable pullup/down hyst, open drain


	//SETUP LOGIC LEVEL ON PINS
	FPGA_SUSPEND_INACTIVE;
	CFG_PROG_B_INACTIVE;
	CFG_CCLK_INACTIVE;
	CFG_DIN_INACTIVE;
	CFG_MODE1_SLAVE;		//configure in serial slave mode
	CFG_MUX_LPC_CTL;			//select LPC as serial configuration device


}//INITGPIO





/*************************************************************************
 *DESCRIPTION: 	pins can conflict with normal operation - set as hiz until needed
 *************************************************************************/
void InitMiscCFG_State( void){

	//SETUP LOGIC LEVEL ON PINS
	LPC_GPIO2->DIR &= ~CFG_FLASH_CS_LOC;		//HIZ
	LPC_GPIO1->DIR &= ~CFG_FLASH_RST_LOC;		//HIZ
	LPC_GPIO2->DIR &= ~FTDI_RST_LOC;			//HIZ

}


/*************************************************************************
 *DESCRIPTION: 	set the default state of pins
 *************************************************************************/
void InitGPIO_State( void){

	//SETUP LOGIC LEVEL ON PINS
	FPGA_SUSPEND_INACTIVE;
	CFG_PROG_B_INACTIVE;
	CFG_CCLK_INACTIVE;
	CFG_DIN_INACTIVE;
	CFG_MODE1_SLAVE;		//configure in serial slave mode
	CFG_MUX_LPC_CTL;			//select LPC as serial configuration device

}



//EOF

