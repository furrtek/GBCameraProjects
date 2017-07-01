/*
 * sdcard.c
 *
 *  Created on: 26 sept. 2015
 *      Author: furrtek
 */

#include "main.h"
#include "sdcard.h"

void spi_init() {
	uint8_t i, dummy;

    LPC_SYSCON->PRESETCTRL &= ~0x01;
	LPC_SYSCON->PRESETCTRL |= 0x01;	//SSP0 out of reset state
	LPC_SYSCON->SSPCLKDIV = 0x20;
	LPC_SSP->CPSR = 0x2;
	LPC_SSP->CR0 = 0x0407;
	LPC_SSP->CR1 = 0x2;				//SSP0 on

	for (i = 0; i < 8; i++)
		dummy = LPC_SSP->DR;		//Clear RXFIFO

	(void)dummy;
}

// File format:
// "GBCC"
// Vx + 3584 bytes image in GB format (x = frames skipped since last one)
// Ax + x*512 bytes of 8192Hz 8-bit unsigned audio (x = number of audio frames to follow)

uint8_t new_file() {
	const char header[4] = {'G','B','C','C'};
	uint16_t br;

	FCLK_FAST();

	// Find filename and create file
	fr = f_open(&file, file_name, FA_WRITE | FA_CREATE_NEW);
    while (fr == FR_EXIST) {
    	if (file_name[7] < '9') {
    		file_name[7]++;
    	} else {
    		file_name[7] = '0';
        	if (file_name[6] < '9') {
        		file_name[6]++;
        	} else {
        		file_name[6] = '0';
            	if (file_name[5] < '9') {
            		file_name[5]++;
            	} else {
            		// TODO: No file available for creation (all 999 already exist)
            		return 1;
            	}
        	}
    	}
    	fr = f_open(&file, file_name, FA_WRITE | FA_CREATE_NEW);
    }

    if (fr != FR_OK)
    	return 1;

    // Write header
    fr = f_write(&file, &header, 4, &br);

    FCLK_LCD();

    return 0;
}
