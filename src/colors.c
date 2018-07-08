/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.3
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/

#include "colors.h"
#include "lcd.h"

const palette_t palette_grey = {
	"Grey ",
	{
		COLOR565(0x00, 0x00, 0x00),
		COLOR565(0x55, 0x55, 0x55),
		COLOR565(0xAA, 0xAA, 0xAA),
		COLOR565(0xFF, 0xFF, 0xFF)
	}
};
const palette_t palette_dmg = {
	"DMG  ",
	{
		COLOR565(0x00, 0x2B, 0x16),
		COLOR565(0x0A, 0x64, 0x33),
		COLOR565(0x56, 0xAA, 0x2B),
		COLOR565(0xB1, 0xC1, 0x00)
	}
};
const palette_t palette_cgb = {
	"CGB  ",
	{
		COLOR565(0x00, 0x00, 0x00),
		COLOR565(0x00, 0x63, 0xC5),
		COLOR565(0x7B, 0xFF, 0x31),
		COLOR565(0xFF, 0xFF, 0xFF)
	}
};
const palette_t palette_sepia = {
	"Sepia",
	{
		COLOR565(100, 64, 34),
		COLOR565(154, 105, 43),
		COLOR565(206, 146, 55),
		COLOR565(255, 190, 65)
	}
};
const palette_t palette_candy = {
	"Candy",
	{
		COLOR565(64, 0, 128),
		COLOR565(188, 0, 155),
		COLOR565(255, 80, 120),
		COLOR565(255, 150, 70)
	}
};
const palette_t palette_aqua = {
	"Aqua ",
	{
		COLOR565(22, 52, 162),
		COLOR565(40, 128, 201),
		COLOR565(69, 209, 255),
		COLOR565(169, 255, 245)
	}
};
const palette_t palette_cute = {
	"Cute ",
	{
		COLOR565(240, 68, 121),
		COLOR565(245, 115, 140),
		COLOR565(250, 175, 155),
		COLOR565(255, 246, 200)
	}
};

const palette_t * palettes_list[MAX_PALETTES] = {
	&palette_grey,
	&palette_dmg,
	&palette_cgb,
	&palette_sepia,
	&palette_candy,
	&palette_aqua,
	&palette_cute
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
