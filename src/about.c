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
#include "views.h"
#include "colors.h"
#include "io.h"
#include "lcd.h"

const struct {
	uint16_t x;
	uint16_t y;
	char * s;
	uint16_t c;
} credits[6] = {
	{ 32, 64, "Hardware", COLOR_GREEN },
	{ 32, 64+16, "Firmware", COLOR_GREEN },
	{ 96, 64+16+16+8, "Furrtek", COLOR_WHITE },
	{ 32, 64+96, "Thanks:", COLOR_GREEN },
	{ 64, 64+96+16+8, "AntonioND", COLOR_WHITE },
	{ 64+96, 64+96+16+8+16, "cLx", COLOR_WHITE }
};

void about_view() {
    lcd_clear();

    for (uint32_t c = 0; c < 6; c++)
    	lcd_print(credits[c].x, credits[c].y, credits[c].s, credits[c].c, 1);

	lcd_print(56, 304, "Press any button", COLOR_WHITE, 0);

	fade_in();

	loop_func = about_loop;
}

void about_loop() {
	systick_wait(2);	// 20ms

	read_inputs();

	if (inputs_active)
		fade_out(menu_view);
}
