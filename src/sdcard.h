/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.3
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/

#include <string.h>
#include "LPC13xx.h"
#include "diskio.h"

#ifndef SDCARD_H_
#define SDCARD_H_

extern const uint8_t bmp_header[54];
extern const char file_header[4];

void FCLK_LCD();
void FCLK_FAST();
void FCLK_SLOW();
void xmit_spi(BYTE dat);

void spi_init(void);
void set_filename(const char * filename);
uint8_t save_bmp();
uint8_t new_file();

#endif /* SDCARD_H_ */
