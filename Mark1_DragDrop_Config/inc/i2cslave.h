
#ifndef __I2C_H 
#define __I2C_H

extern volatile uint8_t i2c_cmd_rx;
extern volatile uint8_t i2c_nack_rx;
extern volatile uint8_t i2c_lpc_configure;	//flag - configure  fpga with lpc
extern volatile uint8_t i2c_lpc_mode;
extern volatile uint8_t i2c_passive_mode;


#define I2C_RD_BUF_SIZE 8
#define I2C_WR_BUF_SIZE 8


//#define I2C_SLAVE_ADDR	0X70	//bits 7:1 are the 7 bit slave address.  bit 0 is a control bit.
#define I2C_SLAVE_ADDR	0X70	//7 BIT SLAVE ADDRESS


#define FAST_MODE_PLUS	0

#define I2CMASTER		0

#define BUFSIZE			6
#define MAX_TIMEOUT		0x00FFFFFF


#define READ_WRITE		0x01

#define RD_BIT			0x01

#define I2C_IDLE			0
#define I2C_STARTED			1
#define I2C_RESTARTED		2
#define I2C_REPEATED_START	3
#define DATA_ACK			4
#define DATA_NACK			5
#define I2C_WR_STARTED		6
#define I2C_RD_STARTED		7

#define I2CONSET_I2EN		0x00000040  /* I2C Control Set Register */
#define I2CONSET_AA			0x00000004
#define I2CONSET_SI			0x00000008
#define I2CONSET_STO		0x00000010
#define I2CONSET_STA		0x00000020

#define I2CONCLR_AAC		0x00000004  /* I2C Control clear Register */
#define I2CONCLR_SIC		0x00000008
#define I2CONCLR_STAC		0x00000020
#define I2CONCLR_I2ENC		0x00000040

#define I2DAT_I2C			0x00000000  /* I2C Data Reg */
#define I2ADR_I2C			0x00000000  /* I2C Slave Address Reg */

//IT SEEMS DUTY VALUES ARE ONLY USED IN MASTER MODE.  ABLE SEND AND RECEIVE COMMANDS AT DIFFERENT RATE IN SLAVE MODE.
#define I2SCLH_SCLH			0x00000180  /* I2C SCL Duty Cycle High Reg */	//VALUE FOR 92.2KHZ
#define I2SCLL_SCLL			0x00000180  /* I2C SCL Duty Cycle Low Reg */	//VALUE FOR 92.2KHZ

#define I2SCLH_HS_SCLH		0x00000020  /* Fast Plus I2C SCL Duty Cycle High Reg */ //VALUE FOR 92.2KHZ
#define I2SCLL_HS_SCLL		0x00000020  /* Fast Plus I2C SCL Duty Cycle Low Reg */ //VALUE FOR 92.2KHZ


extern void I2C_IRQHandler( void );
extern void I2CSlaveInit( void );
void Init_i2c_buf( void);
void I2C_process_task( void);

#endif /* end __I2C_H */
/****************************************************************************
**                            End Of File
*****************************************************************************/
