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
#include "main.h"
#include "colors.h"
#include "views.h"
#include "capture.h"
#include "icons.h"
#include "lcd.h"
#include "io.h"
#include "sdcard.h"
#include "gbcam.h"

void update_infos() {
	const uint8_t logo[48] = {
		0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
	};
	const struct {
		uint8_t code;
		uint8_t flags;
	} type_codes[24] = {
		//        MBCABTMR
		{ 0x01, 0b00100000 },
		{ 0x02, 0b00110000 },
		{ 0x03, 0b00111000 },
		{ 0x05, 0b01000000 },
		{ 0x06, 0b01001000 },
		{ 0x08, 0b00010000 },
		{ 0x09, 0b00011000 },
		{ 0x0B, 0b00000010 },
		{ 0x0C, 0b00010010 },
		{ 0x0D, 0b00011010 },
		{ 0x0F, 0b01101100 },
		{ 0x10, 0b01111100 },
		{ 0x11, 0b01100000 },
		{ 0x12, 0b01110000 },
		{ 0x13, 0b01111000 },
		{ 0x15, 0b10000000 },
		{ 0x16, 0b10010000 },
		{ 0x17, 0b10011000 },
		{ 0x19, 0b10100000 },
		{ 0x1A, 0b10110000 },
		{ 0x1B, 0b10111000 },
		{ 0x1C, 0b10100001 },
		{ 0x1D, 0b10110001 },
		{ 0x1E, 0b10111001 }
	};
	uint32_t c, x;
	uint8_t checksum = 0;

    lcd_clear();

    lcd_print(16, 40, "Title:", COLOR_GREY, 0);
    lcd_print(16, 40+8, "Logo:", COLOR_GREY, 0);
    lcd_print(16, 40+8+8, "Type:", COLOR_GREY, 0);
    lcd_print(16, 40+8+8+8, "ROM:", COLOR_GREY, 0);
    lcd_print(16, 40+8+8+8+8, "RAM:", COLOR_GREY, 0);
    lcd_print(16, 40+8+8+8+8+8, "Checksum:", COLOR_GREY, 0);

    // Read title
	for (c = 0; c < 13; c++)
		str_buffer[c] = cart_get_rom(c + 0x0134);
	str_buffer[13] = 0;
	lcd_print(16+48, 40, str_buffer, COLOR_WHITE, 0);

	// Check logo
	for (c = 0; c < 48; c++)
		if (cart_get_rom(c + 0x0104) != logo[c]) break;
	if (c == 48)
		lcd_print(16+40, 40+8, "GOOD", COLOR_GREEN, 0);
	else
		lcd_print(16+40, 40+8, "BAD", COLOR_RED, 0);

    // Read type and decode it
	c = cart_get_rom(0x0147);
	if (c == 0x00) {
		lcd_print(16+40, 40+8+8, "ROM ONLY", COLOR_WHITE, 0);
	} else if (c == 0xFC) {
		lcd_print(16+40, 40+8+8, "CAMERA", COLOR_WHITE, 0);
	} else {
		// Search
		for (x = 0; x < 24; c++)
			if (c == type_codes[x].code) break;
		if (x != 24) {
			// Found, decode flags
			c = type_codes[x].flags;
			x = 16 + 40;
			if (c & 0b11100000) {
				// Has MBC
				str_buffer[0] = 'M';
				str_buffer[1] = 'B';
				str_buffer[2] = 'C';
				str_buffer[3] = '0' + (c >> 5);
				str_buffer[4] = 0;
				lcd_print(x, 40+8+8, str_buffer, COLOR_WHITE, 0);
				x += 32;
			}
			if (c & 0b00010000) {
				// Has RAM
				lcd_print(x, 40+8+8, "+RAM", COLOR_WHITE, 0);
				x += 32;
			}
			if (c & 0b00001000) {
				// Has battery
				lcd_print(x, 40+8+8, "+BAT", COLOR_WHITE, 0);
				x += 32;
			}
			if (c & 0b00000100) {
				// Has timer
				lcd_print(x, 40+8+8, "+TIMER", COLOR_WHITE, 0);
				x += 48;
			}
			if (c & 0b00000010) {
				// Has MMM01
				lcd_print(x, 40+8+8, "+MMM01", COLOR_WHITE, 0);
				x += 40;
			}
			if (c & 0b00000001) {
				// Has rumble
				lcd_print(x, 40+8+8, "+RUMBLE", COLOR_WHITE, 0);
			}
		} else {
			// Not found, print hex code
			str_buffer[0] = '?';
			str_buffer[1] = ' ';
			str_buffer[2] = hexify(c >> 4);
			str_buffer[3] = hexify(c & 15);
			str_buffer[4] = 0;
			lcd_print(16+40, 40+8+8, str_buffer, COLOR_WHITE, 0);
		}
	}

    // Read ROM size
	c = cart_get_rom(0x0148);
	str_buffer[0] = hexify(c >> 4);
	str_buffer[1] = hexify(c & 15);
	str_buffer[2] = 0;
	lcd_print(16+32, 40+8+8+8, str_buffer, COLOR_WHITE, 0);

    // Read RAM size
	c = cart_get_rom(0x0149);
	str_buffer[0] = hexify(c >> 4);
	str_buffer[1] = hexify(c & 15);
	str_buffer[2] = 0;
	lcd_print(16+32, 40+8+8+8+8, str_buffer, COLOR_WHITE, 0);

	// Checksum
	for (c = 0; c < 25; c++)
		checksum -= (cart_get_rom(c + 0x0134) + 1);
	if (checksum == cart_get_rom(0x014D))
		lcd_print(16+72, 40+8+8+8+8+8, "GOOD", COLOR_GREEN, 0);
	else
		lcd_print(16+72, 40+8+8+8+8+8, "BAD", COLOR_RED, 0);
}

void tool_view() {
	update_infos();

	fade_in();

	loop_func = tool_loop;
}

void tool_loop() {
	systick_wait(2);	// 20ms

	read_inputs();

	if (inputs_active & BTN_LEFT) {
		fade_out(menu_view);
		return;
	} else if (inputs_active & BTN_A) {
		update_infos();
	}
}
