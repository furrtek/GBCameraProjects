/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.2
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/

#include "colors.h"
#include "lcd.h"

const uint16_t lut_2bpp_grey[4] = {
	COLOR565(0x00, 0x00, 0x00),
	COLOR565(0x55, 0x55, 0x55),
	COLOR565(0xAA, 0xAA, 0xAA),
	COLOR565(0xFF, 0xFF, 0xFF)
};
const uint16_t lut_2bpp_dmg[4] = {
	COLOR565(0x00, 0x2B, 0x16),
	COLOR565(0x0A, 0x64, 0x33),
	COLOR565(0x56, 0xAA, 0x2B),
	COLOR565(0xB1, 0xC1, 0x00)
};
const uint16_t lut_2bpp_cgb[4] = {
	COLOR565(0x00, 0x00, 0x00),
	COLOR565(0x00, 0x63, 0xC5),
	COLOR565(0x7B, 0xFF, 0x31),
	COLOR565(0xFF, 0xFF, 0xFF)
};
const uint16_t * lut_2bpp_list[3] = {
	lut_2bpp_grey,
	lut_2bpp_dmg,
	lut_2bpp_cgb
};

const uint16_t bar_colors[8] = {
	COLOR565(0xFF, 0x1F, 0x1F),
	COLOR565(0xFF, 0x7F, 0x1F),
	COLOR565(0x7F, 0xFF, 0x1F),
	COLOR565(0x1F, 0xFF, 0x1F),
	COLOR565(0x1F, 0xFF, 0x1F),
	COLOR565(0xFF, 0xFF, 0x1F),
	COLOR565(0xFF, 0x3F, 0x1F),
	COLOR565(0xFF, 0x1F, 0x1F)
};
