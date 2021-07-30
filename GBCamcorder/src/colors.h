/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.3
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/

#ifndef COLORS_H_
#define COLORS_H_

#include "main.h"

#define COLOR_RED 		0b1111100000000000
#define COLOR_GREEN 	0b0000011111100000
#define COLOR_BLUE		0b0000000000011111
#define COLOR_YELLOW	0b1111111111100000
#define COLOR_ORANGE	0b1111101111100000
#define COLOR_CYAN		0b0000011111111111
#define COLOR_WHITE 	0b1111111111111111
#define COLOR_GREY		0b0111101111101111
#define COLOR_BLACK		0b0000000000000000

#define MAX_PALETTES 7

typedef struct {
	char * name;
	uint16_t colors[4];
} palette_t;

const palette_t * palettes_list[MAX_PALETTES];

const uint16_t bar_colors[8];

#endif /* COLORS_H_ */
