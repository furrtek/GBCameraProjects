/*
 * main.h
 *
 *  Created on: 20 sept. 2015
 *      Author: furrtek
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "LPC13xx.h"
#include "ff.h"

#define COLOR_RED 		0b1111100000000000
#define COLOR_GREEN 	0b0000011111100000
#define COLOR_BLUE		0b0000000000011111
#define COLOR_YELLOW	0b1111111111100000
#define COLOR_WHITE 	0b1111111111111111
#define COLOR_GREY		0b0111101111101111

#define FRAME_SIZE 	3584			// 128*112 * 2bpp / 8 bits

#define EXPO_INRANGE	0
#define EXPO_DARK		1
#define EXPO_BRIGHT		2

static FATFS FatFs;
static FIL fil;
uint8_t picture_buffer[FRAME_SIZE];
char rbf[32];

#endif /* MAIN_H_ */
