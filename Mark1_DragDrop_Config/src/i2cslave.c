/******************************************************************************
From device to device, the I2C communication protocol may vary,
in the example below, the protocol uses repeated start to read data from or
write to the device:
For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO
for master write: the sequence is: STA,Addr(W),offset,RE-STA,Addr(w),data...STO
Thus, in state 8, the address is always WRITE. in state 10, the address could
be READ or WRITE depending on the I2C command.
*********************************************************************************/


#include "System.h"
#include "LPC13xx.h"
#include "type.h"
#include "i2cslave.h"

//I2C EXTERNAL STATUS FLAGS
//I2C TASKS flags that need to occur outside of interrupt
volatile uint8_t f_i2c_task = 0;	//flag to process i2c task

volatile uint8_t i2c_cmd_rx = 0;
volatile uint8_t i2c_nack_rx = 0;
volatile uint8_t i2c_lpc_mode = 0;
volatile uint8_t i2c_passive_mode = 0;
volatile uint8_t i2c_lpc_configure = 0;	//flag - configure  fpga with lpc


//I2C INTERRUPT STATE MACHINE VAR
volatile uint32_t I2CSlaveState = I2C_IDLE;	//interrupt state varialbe
//I2C READ/WR BUFFERS
volatile uint8_t I2CWrBuffer[I2C_WR_BUF_SIZE];	//data to write to i2c bus
volatile uint8_t I2CRdBuffer[I2C_RD_BUF_SIZE];	//data read from i2c bus
//volatile uint32_t I2CSlaveState;					//state machine for i2c slave interrupt
volatile uint32_t I2CReadLength, I2CWriteLength;	//lengh of the read write buffers
//INDEX VALUES FOR THE READ/WR BUFFERS
volatile uint8_t RdIndex = 0;
volatile uint8_t WrIndex = 0;


/* *****************************************************************************************
* DESCRIPTION: Process commands I2C commands that are not processed in the interrupt.
* 		- Slow porcessing commands will be processed here so that the interrupt will not block communications

******************************************************************************************/
void I2C_process_cmd( void){
	uint8_t cmd = 0;

	//NEED TO HANDLE THE BITSTREAM START SEQUENCE


	/*	//ALL OF THIS IS NOW IN THE INTERRUPT
	cmd = I2CRdBuffer[0];
	switch (cmd){
		case I2C_CMD_PASSIVE_MODE:
			//PUT DEVICE INTO PASSIVE MODE
			//InitGPIO_PASSIVE();
			TransitionToState(STATE_MASTER_LOADER);
			LED0_TOGGLE;
			break;
		case I2C_CMD_LPC_CFG_MODE:
			//PUT DEVICE INTO LPC CFG MODE
			//InitGPIO();
			TransitionToState(STATE_LPC_LOADER);
			LED1_TOGGLE;
			break;
		case I2C_CMD_DONE:
			//send the status of done pin.
			I2CWrBuffer[0] = CFG_DONE_STATE; //may have to add this to state machine
			LED0_TOGGLE;
			LED1_TOGGLE;
			break;
		default:
			break;
	}	//switch
	*/

}//function




/**********************************************************************************
//  Table 237. Slave Receiver mode, SEE PAGE 236
//STATUS REGISTER BIT VALUES
//Bit 		7 		6 		5 		4 		3 		2 	1 	0
//Symbol 	- 		I2EN 	STA 	STO 	SI 		AA 	- 	-
//Value 	- 		1 		0 		0 		0 		1 	- 	-
***********************************************************************************
	//STATES USED IN THE STATE MACHINE
	case 0x60:								// Own SLA+W has been received; ACK has been returned.
	case 0x68: 0B 0110 1000					// Arbitration lost in SLA+R/W as master; Own SLA+W has been received, ACK returned.
	case 0x80: 1000 0000					// data receive
	case 0x90: 1001 0000


	case 0xA8: 1010 1000					// An own SLA_R has been received.  	//slave Read
	case 0xB0: 1011 0000

	case 0xB8: 1011 1000					// Data byte has been transmitted
	case 0xC8: 1100 1000

	case 0xC0: 1100 1000				  // Data byte has been transmitted, NACK
	case 0xA0: 1010 0000
/*****************************************************************************
** Function name:		I2C_IRQHandler
*****************************************************************************/
volatile uint8_t rec = 0, temp1=0;
void I2C_IRQHandler(void) 
{
  uint8_t StatValue;

  StatValue = LPC_I2C->STAT;
  switch ( StatValue )
  {
  //Bit 		7 	6 		5 		4 		3 		2 	1 	0
  //Symbol 		- 	I2EN 	STA 	STO 	SI 		AA 	- 	-
	case 0x60:							// An own SLA_W has been received. - ADDRESS WAS DETECTED
	case 0x68:		//START RECEIVED
		RdIndex = 0;	//restarte the read index
		LPC_I2C->CONSET = I2CONSET_AA;	// assert ACK after SLV_W is received
		LPC_I2C->CONCLR = I2CONCLR_SIC;	//clear the interrupt flag
		I2CSlaveState = I2C_WR_STARTED;
	break;
	
	//WRITE ALREADY STARTED - STORE THE DATA
	case 0x80:							// data receive
	case 0x90:
		if ( I2CSlaveState == I2C_WR_STARTED )
		{
			LPC_I2C->CONSET = I2CONSET_AA;	// assert ACK after data is received
			I2CRdBuffer[RdIndex++] = LPC_I2C->DAT;
		}//if
		else
		{
			LPC_I2C->CONCLR = I2CONCLR_AAC;	// assert NACK
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;
		
	//READ COMMAND RECEIVED
	case 0xA8:								// An own SLA_R has been received.  	//slave Read
	case 0xB0:
		RdIndex = 0;
		LPC_I2C->CONSET = I2CONSET_AA;		// assert ACK after SLV_R is received
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		I2CSlaveState = I2C_RD_STARTED;
		WrIndex = 0;//I2CRdBuffer[0];		//The 1st byte is the index. //restart the write index
	break;
	
					//SEND CONSECUTIVE DATA OUT ON I2C PORT
	case 0xB8:		// Data byte has been transmitted
	case 0xC8:
		if ( I2CSlaveState == I2C_RD_STARTED )
		{
		  LPC_I2C->DAT = I2CWrBuffer[WrIndex++];	// write the same data back to master
		  LPC_I2C->CONSET = I2CONSET_AA;			// assert ACK
		}
		else
		{
		  LPC_I2C->CONCLR = I2CONCLR_AAC;		// assert NACK
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
	break;

	//NACK WAS RECEIVED - ALL DATA WAS SENT
	case 0xC0:									// Data byte has been transmitted, NACK
		LPC_I2C->CONCLR = I2CONCLR_AAC;			// assert NACK - or nack was received??
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		//I2CSlaveState = DATA_NACK;			//demo used this
		LPC_I2C->CONSET = I2CONSET_AA; 			//THIS IS NEEDED
		I2CSlaveState = I2C_IDLE;				//restart state machine
		//LED1_TOGGLE;
		i2c_nack_rx = 1;	// set flag that nack was received
		WrIndex = 0;
	break;

	//STOP CONDITION - PROCESSED THE RECEIVED DATA
	case 0xA0:							// Stop condition or repeated start has
		LPC_I2C->CONSET = I2CONSET_AA;	// been received, assert ACK.
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		I2CSlaveState = I2C_IDLE;
		//LED0_TOGGLE;
		i2c_cmd_rx = 1;
		//PROCESS THE RECEIVED COMMANDS HERE
		rec = I2CRdBuffer[0];	//what is the first value received (command)
		switch (rec){
			case I2C_CMD_PASSIVE_MODE:
				//PUT DEVICE INTO PASSIVE MODE
				i2c_passive_mode = 1;	//set flag for main to process
				LED0_TOGGLE;
				break;
			case I2C_CMD_LPC_CFG_MODE:
				//PUT DEVICE INTO LPC CFG MODE
				i2c_lpc_mode = 1;	//set flag for main to process
				//LED1_TOGGLE;
				break;
			case I2C_CMD_DONE:
				//send the status of done pin.
				//I2CWrBuffer[0] = 0xAA;	//debug send something recognizable
				I2CWrBuffer[0] = CFG_DONE_STATE;
				LED0_TOGGLE;
				LED1_TOGGLE;
				break;
			case I2C_CMD_LPC_CONFIGURE:
				//LED0_TOGGLE;
				//LED1_TOGGLE;
				i2c_lpc_configure = 1;	//set flag to have lpc configure the fpga
				break;
			default:
				break;
		}//switch
		//reset to receive data
		RdIndex = 0;
	break;

	default:
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		LPC_I2C->CONSET = I2CONSET_I2EN | I2CONSET_SI;
	break;
  }
  return;
}

/*****************************************************************************
** Function name:		I2CSlaveInit
**
** Descriptions:		Initialize I2C controller
**
** parameters:			I2c mode is either MASTER or SLAVE
** Returned value:		true or false, return false if the I2C
**				interrupt handler was not installed correctly
** 
*****************************************************************************/
void I2CSlaveInit( void ) 
{
  /* It seems to be bit0 is for I2C, different from
  UM. To be retested along with SSP reset. SSP and I2C
  reset are overlapped, a known bug, for now, both SSP 
  and I2C use bit 0 for reset enable. Once the problem
  is fixed, change to "#if 1". */

#if 1
  LPC_SYSCON->PRESETCTRL |= (0x1<<1);	//This de-asserts the reset signal to the I2C block
#else
  LPC_SYSCON->PRESETCTRL |= (0x1<<0);
#endif
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
  LPC_IOCON->PIO0_4 &= ~0x3F;		//  I2C I/O config 	//this lines seems unnecessary.  bits7:3 are reserved.?  But read C= 1100
  LPC_IOCON->PIO0_4 |= 0x01;		// I2C SCL
  LPC_IOCON->PIO0_5 &= ~0x3F;	
  LPC_IOCON->PIO0_5 |= 0x01;		// I2C SDA

  //--- Clear flags ---
  LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;    

  //--- Reset registers ---
#if FAST_MODE_PLUS
  LPC_IOCON->PIO0_4 |= (0x1<<9);
  LPC_IOCON->PIO0_5 |= (0x1<<9);
  LPC_I2C->SCLL   = I2SCLL_HS_SCLL;
  LPC_I2C->SCLH   = I2SCLH_HS_SCLH;
#else
  LPC_I2C->SCLL   = I2SCLL_SCLL;
  LPC_I2C->SCLH   = I2SCLH_SCLH;
#endif

  //LPC_I2C->ADR0 = PCF8594_ADDR;
  LPC_I2C->ADR0 = I2C_SLAVE_ADDR<<1;	//upper 7 bits are the address, low bit is control
  I2CSlaveState = I2C_IDLE;
  
  // Enable the I2C Interrupt
  NVIC_EnableIRQ(I2C_IRQn);

  LPC_I2C->CONSET = I2CONSET_I2EN | I2CONSET_SI;
  return;
}//i2c slave init


// init i2c test buffer **************************************************
void Init_i2c_buf( void){
 uint8_t i = 0;

	  for ( i = 0; i < I2C_RD_BUF_SIZE ; i++ )
	  {
		I2CRdBuffer[i] = 0x00;
		I2CWrBuffer[i] = 0X00;
	  }

	  /*
	  for ( i = 0; i < I2C_WR_BUF_SIZE; i++ )
	  {
		I2CWrBuffer[i] = ;
	  }
	  */
}



/*******************************************************************************
**   Main Function  main()
*******************************************************************************/
int example_main (void)
{
	  /* Basic chip initialization is taken care of in SystemInit() called
	   * from the startup code. SystemInit() and chip settings are defined
	   * in the CMSIS system_<part family>.c file.
	   */

  uint32_t i;

  for ( i = 0; i < BUFSIZE; i++ )
  {
	I2CRdBuffer[i] = 0x00;
  }

  I2CSlaveInit();			/* initialize I2c */

  /* When the NACK occurs, the master has stopped the
  communication. Just check the content of I2CRd/WrBuffer. */
	//DEBUG I2C ONLY - WAITS FOR I2C TRANSACTION TO COMPLETE.
	//WAIT UNTIL A NACK IS RECEIVED.  (Master stopped a complete transaction)
	while ( I2CSlaveState != DATA_NACK );
	LED1_TOGGLE;
	LPC_I2C->CONSET = I2CONSET_AA;	/* been received, assert ACK.  */
	LPC_I2C->CONCLR = I2CONCLR_SIC;
	I2CSlaveState = I2C_IDLE;
	Init_i2c_buf();


}//example

/******************************************************************************
**                            End Of File
******************************************************************************/





