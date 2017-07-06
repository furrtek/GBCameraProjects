/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.2
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/

#include "main.h"
#include "sdcard.h"

const char file_header[4] = { 'G', 'B', 'C', 'C' };

const uint8_t bmp_header[54] = {
	'B', 'M',					// Magic
	0x36, 0xA8, 0x00, 0x00,		// Size
	0x00, 0x00, 0x00, 0x00,		// Reserved
	0x36, 0x00, 0x00, 0x00,		// Offset to image data

	0x28, 0x00, 0x00, 0x00,		// Header size
	0x80, 0x00, 0x00, 0x00,		// Image width
	0x70, 0x00, 0x00, 0x00,		// Image height
	0x01, 0x00,					// Planes
	0x18, 0x00,					// Bits per pixel
	0x00, 0x00, 0x00, 0x00,		// Compression
	0x00, 0xA8, 0x00, 0x00,		// Image data size
	0x00, 0x00, 0x00, 0x00,		// X resolution
	0x00, 0x00, 0x00, 0x00,		// Y resolution
	0x00, 0x00, 0x00, 0x00,		// Colors count
	0x00, 0x00, 0x00, 0x00		// Important colors
};

void spi_init() {
	uint8_t i, dummy;

    LPC_SYSCON->PRESETCTRL &= ~0x01;
	LPC_SYSCON->PRESETCTRL |= 0x01;	// SSP0 out of reset state
	LPC_SYSCON->SSPCLKDIV = 0x20;
	LPC_SSP->CPSR = 0x2;
	LPC_SSP->CR0 = 0x0407;
	LPC_SSP->CR1 = 0x2;				// SSP0 on

	for (i = 0; i < 8; i++)
		dummy = LPC_SSP->DR;		// Clear RXFIFO

	(void)dummy;
}

const uint8_t bmp_colors[12] = {
	0x00, 0x00, 0x00,
	0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA,
	0xFF, 0xFF, 0xFF
};

uint8_t save_bmp() {
	uint16_t br;
	uint8_t data_l, data_h, pixel;
	uint8_t xt, yt, xp;
	uint16_t addr, yto;
	char * file_name = file_list[0].file_name;

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
    f_write(&file, &bmp_header, 4, &br);

    // Write image data
	for (yt = 0; yt < 112; yt++) {

		yto = (yt & 7) + ((yt & 0x78) * 16);

		for (xt = 0; xt < 128; xt++) {
			// See draft, complete mess: array of tiles to linear LCD format (x inverted)
			// (xt & 0x78): tile # x
			// + (yt & 7): pixel y in tile
			// + (yt & 0x78) * 16: tile # y
			// * 2: 2 bytes per pixel

			addr = ((xt & 0x78) + yto) * 2;

			data_l = picture_buffer[addr];
			data_h = picture_buffer[addr + 1];

			xp = 7 - (xt & 7);			// Invert x

			// Planar to linear
			// xp = 0				xp = 1				...
			// 76543210 76543210	76543210 76543210
			// 6543210- 76543210	7654321- -7654321
			// ------0- -------0	------1- -------1
			pixel = (((data_h >> xp) << 1) & 2) | ((data_l >> xp) & 1);

		    f_write(&file, &bmp_colors[pixel * 3], 3, &br);
		}
	}

	f_close(&file);

    return 0;
}

// File format:
// "GBCC"
// Vx + 3584 bytes image in GB format (x = frames skipped since last one)
// Ax + x*512 bytes of 8192Hz 8-bit unsigned audio (x = number of audio frames to follow)
uint8_t new_file() {
	uint16_t br;
	char * file_name = file_list[0].file_name;

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

    return 0;
}
