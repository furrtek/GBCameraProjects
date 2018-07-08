/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.3
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/

#include "main.h"
#include "sdcard.h"
#include "capture.h"

const char file_header[4] = { 'G', 'B', 'C', 'C' };

const uint8_t bmp_header[54] = {
	'B', 'M',					// Magic
	0x76, 0x1C, 0x00, 0x00,		// Size of file
	0x00, 0x00, 0x00, 0x00,		// Reserved
	0x76, 0x00, 0x00, 0x00,		// Offset to image data

	0x28, 0x00, 0x00, 0x00,		// Header size
	0x80, 0x00, 0x00, 0x00,		// Image width
	0x70, 0x00, 0x00, 0x00,		// Image height
	0x01, 0x00,					// Planes
	0x04, 0x00,					// Bits per pixel
	0x00, 0x00, 0x00, 0x00,		// Compression
	0x00, 0x1C, 0x00, 0x00,		// Image data size
	0x00, 0x00, 0x00, 0x00,		// X resolution
	0x00, 0x00, 0x00, 0x00,		// Y resolution
	0x00, 0x00, 0x00, 0x00,		// Colors count
	0x00, 0x00, 0x00, 0x00		// Important colors
};

void spi_init() {
	uint8_t i, dummy;

	LPC_SYSCON->PRESETCTRL |= 0x01;	// SSP0 out of reset state
	LPC_SYSCON->SSPCLKDIV = 20;		// 72/2/20/4 = 450kHz
	LPC_SSP->CPSR = 0x2;			// Prescaler
	LPC_SSP->CR0 = 0x0407;			// SPI 8 bit, /4
	LPC_SSP->CR1 = 0x2;				// SSP0 on

	for (i = 0; i < 8; i++)
		dummy = LPC_SSP->DR;		// Clear RXFIFO

	(void)dummy;
}

void set_filename(const char * filename) {
	memcpy(file_list[0].file_name, filename, 13);
}

uint8_t save_bmp() {
	uint32_t c;
	uint16_t br, color;
	uint8_t data_l, data_h, pixel;
	uint8_t xt, yt, yt_flip, xp, pixel_pair = 0;
	uint16_t addr, yto;

    // Write header
	f_write(&file, &bmp_header, 54, &br);

	// Write palettes
	for (c = 0; c < 4; c++) {
		color = lut_2bpp->colors[c];
		f_putc((color << 3) & 0xF8, &file);		// B
		f_putc((color >> 3) & 0xFC, &file);		// G
		f_putc((color >> 8) & 0xF8, &file);		// R
		f_putc(0, &file);
	}
	for (c = 0; c < (12 * 4); c++)
		f_putc(0, &file);

    // Write image data
	for (yt = 0; yt < 112; yt++) {

		yt_flip = 111 - yt;
		yto = (yt_flip & 7) + ((yt_flip & 0x78) * 16);

		for (xt = 0; xt < 128; xt++) {
			// (xt & 0x78): 0XXXX000 tile # x
			// + (yt & 7):  00000YYY pixel y in tile
			// + (yt & 0x78) * 16: 0YYYY000 0000 tile # y
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

			if ((xt & 1) == 1) {
				pixel_pair |= pixel;
				if (f_putc(pixel_pair, &file) == -1)	// Write image marker
					LPC_GPIO1->DATA &= ~(1<<8);		// DEBUG: Yellow LED shouldn't turn on
			} else {
				pixel_pair = pixel << 4;
			}
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
    	return fr;

    return 0;
}
