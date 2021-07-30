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
#include "tool.h"
#include "colors.h"
#include "capture.h"
#include "views.h"
#include "icons.h"
#include "lcd.h"
#include "io.h"
#include "sdcard.h"
#include "gbcam.h"

void update_infos() {
	uint32_t c, x;
	uint8_t checksum = 0;
	const uint8_t n_logo[48] = {
		0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
		0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
		0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E
	};
	const struct {
		uint8_t code;
		uint8_t flags;
	} type_codes[23] = {
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
		{ 0x19, 0b10100000 },
		{ 0x1A, 0b10110000 },
		{ 0x1B, 0b10111000 },
		{ 0x1C, 0b10100001 },
		{ 0x1D, 0b10110001 },
		{ 0x1E, 0b10111001 },
		{ 0x20, 0b11000000 },
		{ 0x22, 0b11111001 }
	};
	const char type_ram[] = "+RAM";
	const char type_bat[] = "+BAT";
	const char type_timer[] = "+TIMER";
	const char type_mmm[] = "+MMM01";
	const char type_rumble[] = "+RUMBLE";
	const char * type_str[5] = {
		type_ram,
		type_bat,
		type_timer,
		type_mmm,
		type_rumble
	};

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
	memcpy(file_list[0].file_name, str_buffer, 6);

	// Check logo
	for (c = 0; c < 48; c++)
		if (cart_get_rom(c + 0x0104) != n_logo[c]) break;
	if (c == 48)
		lcd_print(16+40, 40+8, "GOOD", COLOR_GREEN, 0);
	else
		lcd_print(16+40, 40+8, "BAD", COLOR_RED, 0);

    // Read type and decode it
	mapper_type = 0;
	c = cart_get_rom(0x0147);
	if (c == 0x00) {
		lcd_print(16+40, 40+8+8, "ROM ONLY", COLOR_WHITE, 0);
	} else if (c == 0xFC) {
		lcd_print(16+40, 40+8+8, "CAMERA", COLOR_WHITE, 0);
	} else {
		// Search
		for (x = 0; x < 23; c++)
			if (c == type_codes[x].code) break;
		if (x != 23) {
			// Found, decode flags
			c = type_codes[x].flags;
			x = 16 + 40;
			if (c & 0b11100000) {
				// Has MBC
				mapper_type = c >> 5;
				str_buffer[0] = 'M';
				str_buffer[1] = 'B';
				str_buffer[2] = 'C';
				str_buffer[3] = '0' + mapper_type;
				str_buffer[4] = 0;
				lcd_print(x, 40+8+8, str_buffer, COLOR_WHITE, 0);
				x += 32;
			}
			for (uint32_t b = 0; b < 5; b++) {
				if (c & 0b00010000) {
					lcd_print(x, 40+8+8, (char*)type_str[b], COLOR_WHITE, 0);
					x += 32;
				}
				c <<= 1;
			}
		} else {
			// Not found, print hex code
			str_buffer[0] = '?';
			str_buffer[1] = ' ';
			hex_insert(2, c);
			lcd_print(16+40, 40+8+8, str_buffer, COLOR_WHITE, 0);
		}
	}

    // Read ROM size
	c = cart_get_rom(0x0148);
	hex_insert(0, c);
	lcd_print(16+32, 40+8+8+8, str_buffer, COLOR_WHITE, 0);
	if (c <= 7)
		rom_banks = 1 << (c + 1);
	else
		rom_banks = 0;

    // Read RAM size
	c = cart_get_rom(0x0149);
	hex_insert(0, c);
	lcd_print(16+32, 40+8+8+8+8, str_buffer, COLOR_WHITE, 0);
	if (c == 1)
		sram_banks = 1;
	else if (c == 2)
		sram_banks = 4;
	else if (c == 3)
		sram_banks = 16;
	else
		sram_banks = 0;

	// Checksum
	for (c = 0; c < 25; c++)
		checksum -= (cart_get_rom(c + 0x0134) + 1);
	if (checksum == cart_get_rom(0x014D))
		lcd_print(16+72, 40+8+8+8+8+8, "GOOD", COLOR_GREEN, 0);
	else
		lcd_print(16+72, 40+8+8+8+8+8, "BAD", COLOR_RED, 0);
}

void dump_rom_bank(const uint32_t i) {
	uint16_t br;

	if (i == 0) {
		for (uint32_t b = 0; b < 32; b++) {
			for (uint32_t c = 0; c < 512; c++)
				picture_buffer[c] = cart_get_rom((b << 9) + c);
			FCLK_FAST();
			f_write(&file, &picture_buffer, 512, &br);
		}
	} else {
		for (uint32_t b = 0; b < 32; b++) {
			for (uint32_t c = 0; c < 512; c++)
				picture_buffer[c] = cart_get_rom(0x4000 + (b << 9) + c);
			FCLK_FAST();
			f_write(&file, &picture_buffer, 512, &br);
		}
	}
}

void tool_view() {
	set_filename("------00.R-M");

	update_infos();

	lcd_print(32, 220, "Dump SRAM", COLOR_GREEN, 1);
	lcd_print(32, 220+24, "Dump ROM", COLOR_CYAN, 1);
	lcd_print(32, 220+24+24, "Write SRAM", COLOR_RED, 1);
	lcd_print(32, 220+24+24+24, "Exit", COLOR_ORANGE, 1);

    cursor_prev = 1;
    cursor = 0;

	fade_in();

	loop_func = tool_loop;
}

void tool_loop() {
	uint16_t br;

	systick_wait(2);	// 20ms

	read_inputs();

	if (inputs_active & BTN_DOWN) {
		if (cursor == 3)
			cursor = 0;
		else
			cursor++;
	} else if (inputs_active & BTN_UP) {
		if (cursor == 0)
			cursor = 3;
		else
			cursor--;
	}

	if (inputs_active & BTN_A) {
		if (cursor == 0) {
			// Dump SRAM
			if (sram_banks && (mapper_type < 6)) {
				VALIDATE_BEEP
				file_list[0].file_name[10] = 'A';	// .RAM
				FCLK_FAST();
				fr = new_file();
				if (!fr) {
					lcd_print(56, 200, file_list[0].file_name, COLOR_WHITE, 0);

					LPC_GPIO1->DATA &= ~(1<<8);		// Yellow LED on

					// Use picture buffer as dump buffer
					for (uint32_t bank = 0; bank < sram_banks; bank++) {
						cart_put(0x0000, 0x0A);	// Enable SRAM
						if (mapper_type == 1) {
							cart_put(0x6000, 1);	// RAM bank mode
							cart_put(0x4000, bank);
						} else if (mapper_type == 3) {
							cart_put(0x4000, bank & 3);
						} else if (mapper_type == 5) {
							cart_put(0x4000, bank);
						}

						print_error(48, 80, bank);
						if (mapper_type != 2) {
							for (uint32_t b = 0; b < 4; b++) {
								for (uint32_t c = 0; c < 512; c++)
									picture_buffer[c] = cart_get_ram(0xA000 + (b << 9) + c);
								FCLK_FAST();
								f_write(&file, &picture_buffer, 512, &br);
							}
						} else {
							// MBC2 special case
							for (uint32_t c = 0; c < 512; c++)
								picture_buffer[c] = cart_get_ram(0xA000 + c) & 0x0F;
							FCLK_FAST();
							f_write(&file, &picture_buffer, 512, &br);
						}
					}

					f_close(&file);

					LPC_GPIO1->DATA |= (1<<8);		// Yellow LED off
				} else
					print_error(0, 0, fr);
			}
		} else if (cursor == 1) {
			// Dump ROM
			if (rom_banks && (mapper_type < 6)) {
				VALIDATE_BEEP
				file_list[0].file_name[10] = 'O';	// .ROM
				FCLK_FAST();
				fr = new_file();
				if (!fr) {
					lcd_print(56, 200, file_list[0].file_name, COLOR_WHITE, 0);

					LPC_GPIO1->DATA &= ~(1<<8);		// Yellow LED on

					// Use picture buffer as dump buffer
					dump_rom_bank(0);
					rom_banks--;
					for (uint32_t bank = 0; bank < rom_banks; bank++) {
						if (mapper_type == 1) {
							cart_put(0x6000, 0);	// ROM bank mode
							cart_put(0x2000, bank & 0x1F);
							cart_put(0x4000, bank >> 5);
						} else if (mapper_type == 2) {
							cart_put(0x2100, bank);
						} else if (mapper_type == 3) {
							cart_put(0x2000, bank);
						} else if (mapper_type == 5) {
							cart_put(0x2000, (bank + 1) & 0xFF);
							cart_put(0x3000, (bank + 1) >> 8);
						}

						print_error(48, 80, bank);
						dump_rom_bank(bank);
					}

					f_close(&file);

					LPC_GPIO1->DATA |= (1<<8);		// Yellow LED off
				} else
					print_error(0, 0, fr);
			}
		} else if (cursor == 2) {
			// Write SRAM
			// TODO
		} else if (cursor == 3) {
			// Exit
			VALIDATE_BEEP
			fade_out(menu_view);
			return;
		}
	}

	if (cursor != cursor_prev) {
		lcd_fill(16, 220 + (cursor_prev * 24), 16, 16, COLOR_BLACK);
		lcd_print(16, 220 + (cursor * 24), "#", COLOR_WHITE, 1);
		cursor_prev = cursor;
		MENU_BEEP
	}
}
