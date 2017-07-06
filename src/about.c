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

void about_view() {
    lcd_clear();

	lcd_print(32, 64, "Hardware", COLOR_GREEN, 1);
	lcd_print(32, 64+16, "Firmware:", COLOR_GREEN, 1);
	lcd_print(96, 64+16+16+8, "Furrtek", COLOR_WHITE, 1);

	lcd_print(32, 64+96, "Thanks:", COLOR_GREEN, 1);
	lcd_print(64, 64+96+16+8, "AntonioND", COLOR_WHITE, 1);
	lcd_print(64+96, 64+96+16+8+16, "cLx", COLOR_WHITE, 1);

	lcd_print(56, 304, "Press any button", COLOR_WHITE, 0);

	cursor = 0;
	cursor_prev = 1;

	fade_in();

	loop_func = about_loop;
}

void about_loop() {
	systick = 0;
	while (systick < 2);	// 20ms

	read_inputs();

	if (inputs_active)
		fade_out(menu_view);
}
