/*
 * FPGA_Config.h
 *
 */

#ifndef FPGA_CONFIG_H_
#define FPGA_CONFIG_H_

//ERRORS FOR THE FPGA CONFIGURATION
#define CONFIG_ERROR_FILE_NOT_FOUND	-1
#define CONFIG_ERROR_INIT_NOT_HIGH	-2
#define CONFIG_ERROR_DONE_NOT_HIGH	-3
#define CONFIG_OK					1

//PROTOTYPES
int8_t FPGA_Config(char * Filename);	//main function call for complete lcp fpga config
int8_t InitFPGA_Config( void);			// start fpga init sequence

#endif /* FPGA_CONFIG_H_ */

//EOF
