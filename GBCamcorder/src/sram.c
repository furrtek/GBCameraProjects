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
#include "colors.h"
#include "views.h"
#include "capture.h"
#include "lcd.h"
#include "io.h"
#include "sdcard.h"
#include "gbcam.h"

void sram_view() {
    lcd_clear();

	lcd_print(64, 64, "<  /30>", COLOR_GREY, 1);

	lcd_print(32, 220, "Save", COLOR_RED, 1);
	lcd_print(32, 220+24, "Erase", COLOR_GREEN, 1);
	lcd_print(32, 220+24+24, "Palette", COLOR_YELLOW, 1);
	lcd_print(32, 220+24+24+24, "Exit", COLOR_BLUE, 1);

	set_filename("SDUMP000.BMP");

	// 2bpp gradient values
    prev_palette_number = 1;		// Force palette update
    palette_number = 0;
	set_palette();

	prev_picture_number = 1;		// Force update
    picture_number = 0;

    bank = 1;
    bank_offset = 0xA000;
    cursor_prev = 1;
    cursor = 0;

	fade_in();

	loop_func = sram_loop;
}

void sram_loop() {
	uint32_t c;

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
		// Save
		if (cursor == 0) {
			VALIDATE_BEEP
			FCLK_FAST();
			fr = new_file();
			if (!fr) {
				lcd_print(56, 300, file_list[0].file_name, COLOR_WHITE, 0);

				LPC_GPIO1->DATA &= ~(1<<8);		// Yellow LED on

				FCLK_FAST();
				save_bmp();

				LPC_GPIO1->DATA |= (1<<8);		// Yellow LED off
			} else
				print_error(0, 0, fr);
		} else if (cursor == 1) {
			// Erase
			VALIDATE_BEEP
			cart_put(0x4000, bank);		// SRAM bank
			delay_us(2);
			cart_put(0x0000, 0x0A);		// Enable SRAM writes

		    for (c = 0; c < 0x1000; c++)
		    	cart_put(bank_offset + c, 0xFF);

		    prev_picture_number = picture_number + 1;	// Force refresh
		} else if (cursor == 3) {
			// Exit
			VALIDATE_BEEP
			fade_out(menu_view);
			return;
		}
	}

	if (cursor == 2) {
		// Palette
		if (inputs_active & BTN_LEFT) {
			if (palette_number)
				palette_number--;
			set_palette();
		} else if (inputs_active & BTN_RIGHT) {
			if (palette_number < MAX_PALETTES - 1)
				palette_number++;
			set_palette();
		}
	} else {
		if (inputs_active & BTN_LEFT) {
			if (picture_number)
				picture_number--;
		} else if (inputs_active & BTN_RIGHT) {
			if (picture_number < 29)
				picture_number++;
		}
	}

	if (picture_number != prev_picture_number) {
		MENU_BEEP
		lcd_fill(16, 64, 32, 16, COLOR_BLACK);		// Erase previous picture number

		c = picture_number + 1;
		str_buffer[0] = '0' + (c / 10);		// Draw new picture number
		str_buffer[1] = '0' + (c % 10);
		str_buffer[2] = 0;
		lcd_print(80, 64, str_buffer, COLOR_WHITE, 1);

		// Read SRAM
		bank = (picture_number >> 1) + 1;	// SRAM bank
		cart_put(0x4000, bank);
		delay_us(2);

		bank_offset = (picture_number & 1) ? 0xB000 : 0xA000;	// There are 2 pictures per bank

		for (c = 0; c < FRAME_SIZE; c++)
			picture_buffer[c] = cart_get_ram(bank_offset + c) ^ 0xFF;

		lcd_preview(56, 96);

		prev_picture_number = picture_number;
	}

	if (cursor != cursor_prev) {
		MENU_BEEP
		lcd_fill(16, 220 + (cursor_prev * 24), 16, 16, COLOR_BLACK);
		lcd_print(16, 220 + (cursor * 24), "#", COLOR_WHITE, 1);
		cursor_prev = cursor;
	}

	if (palette_number != prev_palette_number) {
		MENU_BEEP
		lcd_print(32+128, 220+24+24, palettes_list[palette_number]->name, COLOR_YELLOW, 1);
		prev_palette_number = palette_number;
		lcd_preview(56, 96);
	}

}
