//updating code for PCB version RA1.2 - main changes in pin definitions

#include "System.h"


extern volatile uint8_t i2c_cmd_rx;
extern volatile uint8_t i2c_nack_rx;


//test pio1_5 test point as input
#define T4_LOC (1<<5)	//P1_5


//MAIN********
int main (void)
{
	uint32_t del = 0;
	uint8_t temp = 0;
	uint8_t t1 = 0;

	InitGPIO();				//initialize the GPIO pins
	SysTick_Config(SystemCoreClock/1000);	//setup timer tick - Interrupt found in system.c to handle system flags etc.  tasks should be called from while loop.

	LED0_ON;

	//SETUP I2C PERIPHS - NEEDS SETUP BEFORE INITUSB_MSC - CAUSED A LOCKUP WHEN PLACED AFTER.
	Init_i2c_buf();			//Debug I2c.  clear the buffer
	I2CSlaveInit();			//initialize the i2c drivers - all i2c transaction happen in i2cslave.c interrupt.  There is a state machine there.

	InitTimers();			//SETUP SYSTEM TIMERS
	InitButtonDetectAll();	//SETUP BUTTON DEBOUNCE structures

	//THESE FUNCTIONS ARE FOR THE MARK-1 USB, FLASH, LOAD CONFIG.BIT
	InitFLASH();				//init flash memory.
	InitUSB_MSC();				//init usb mass storage device drivers
	StartDel();					//init startup delay
	//SETUP FILE SYSTEM
	f_mount(0,&MyFileSystem);	//mount file system for reading config.bit file form MSD


#ifdef USE_DRAG_DROP_BISTREAM
	//THESE FUNCTIONS ARE FOR THE MARK-1 USB, FLASH, LOAD CONFIG.BIT
	InitFLASH();				//init flash memory.
	InitUSB_MSC();				//init usb mass storage device drivers
	StartDel();					//init startup delay
	//SETUP FILE SYSTEM
	f_mount(0,&MyFileSystem);	//mount file system for reading config.bit file form MSD
	FPGA_Config("config.bit");  //check to see if there is a "config.bit" file if so load it into FPGA.  Note this needs to go into while loop
								// to check for new config file loaded and reload to FPGA.  This is why there currently needs to be a power cycle
								// in order for new file to be loaded into fpga.
#endif

	LED1_OFF;
	LED0_OFF;

	//test pb and sense pin(had to drop resistance to 27 ohms!?) still no sure why.  internal pullup/pulldown confilict of some sort!!
	//setup pb pin





	while(1)
	{

		// WILL ADD IN A BASIC TASK HANDLER THAT IS NOT RUN IN THE SYSTEM IRQ
		Basic_Tasks();


		//MAIN FRONT END TASKS
		if(i2c_lpc_mode){	//received command from i2c go into default lpc mode
			//set pins to default lpc mode
			InitGPIO_LPC_Mode();
			LED0_TOGGLE;
			i2c_lpc_mode=0;
		}
		if(i2c_passive_mode){	//received command from i2c go into passive mode
			//set pins to default lpc mode
			InitGPIO_Passive_Mode();
			LED1_TOGGLE;
			i2c_passive_mode=0;
		}
		if(i2c_lpc_configure){	//received command from i2c to reconfigure
			InitGPIO_LPC_Mode();	//make sure we are not setup for passive
			FPGA_Config("config.bit");
			i2c_lpc_configure=0;
		}



	}//while
}//main


void Basic_Tasks(){

	//TimerTest();
	//PROCESS NON TIMING CRITICAL TASKS
	if( !TimerIsOn(timer_pb) )
	{
		ButtonProcessAll();	//process the push button state machines
		TimerStart(timer_pb, TIME_PB0);	//restart the timer
	}

    //use timers to keep track of when to process.

}




// *****************************************************************************
void debug_master_sense( void){
	uint8_t temp = 0;

	//setup the pin manually
	LPC_GPIO2->DIR &= ~(1<<10); 	//INPUT
	//LPC_IOCON->PIO2_10 = 0x00;				//disable pullup/down hyst, open drain

	temp = LPC_GPIO2->DATA & (1<<10);
	if(  temp == 1  ){
		LED0_ON;
	}
	else if( temp == 0 ){
		LED0_OFF;
	}

}


//MAIN STATE MACHINE.  NOT NEEDED YET
/*
		switch (SystemState) {
			case  STATE_SYS_STARTUP:
				// ANY TASKS TO RUN AT STARTUP
				TransitionToState(STATE_LPC_LOADER);	//GO TO LPC LOADER STATE
				break;
			case  STATE_LPC_LOADER:
				// running with LPC as loader
				// * Add code , if push button then reload config.bit
				// * Check I2C commands - if change states as necessary

				/*if(MASTER_SENSE_STATE){ //NOT WORKING!!!
					TransitionToState(STATE_MASTER_LOADER);
				}*/
				//CHECK FOR I2C COMMANDS
				/*
				if(i2c_cmd_rx){
					I2C_process_cmd();
					i2c_cmd_rx = 0;
				}
				if(i2c_nack_rx){
					i2c_nack_rx = 0;
				}

				break;
			case STATE_MASTER_LOADER:
				// running in slave mode, with a master loading the fpga
				//CHECK FOR I2C COMMANDS
				/*
				if(i2c_cmd_rx){
					I2C_process_cmd();
					i2c_cmd_rx = 0;
				}
				if(i2c_nack_rx){
					i2c_nack_rx = 0;
				}


				/*if(!MASTER_SENSE_STATE){ //NOT WORKING!!!
								TransitionToState(STATE_MASTER_LOADER);
							}
				break;
			default:
				TransitionToState(STATE_SYS_STARTUP);  //go startup
				break;
		}
		*/



//EOF







