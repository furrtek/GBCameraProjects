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
	lcd_print(32, 220+24+24, "Exit", COLOR_BLUE, 1);

	set_filename("SDUMP000.BMP");

    palette_number = 0;
	set_palette();

	prev_picture_number = 1;
    picture_number = 0;
    bank = 1;
    bank_offset = 0xA000;
    cursor_prev = 1;
    cursor = 0;

	fade_in();

	loop_func = sram_loop;
}

void sram_loop() {
	uint16_t c;
	uint16_t br;

	systick = 0;
	while (systick < 2);	// 20ms

	read_inputs();

	if (inputs_active & BTN_DOWN) {
		if (cursor == 2)
			cursor = 0;
		else
			cursor++;
	} else if (inputs_active & BTN_UP) {
		if (cursor == 0)
			cursor = 2;
		else
			cursor--;
	}
	if (inputs_active & BTN_LEFT) {
		if (picture_number)
			picture_number--;
	} else if (inputs_active & BTN_RIGHT) {
		if (picture_number < 29)
			picture_number++;
	}
	if (inputs_active & BTN_A) {
		if (cursor == 0) {
			// Save
			uint8_t fr = save_bmp();
			print_error(0, 0, fr);	// DEBUG

			/*if (!new_file()) {
				lcd_print(56, 300, file_list[0].file_name, COLOR_WHITE, 0);

				LPC_GPIO1->DATA &= ~(1<<8);		// Yellow LED on

				f_write(&file, &bmp_header, sizeof(bmp_header), &br);
				for (c = 0; c < 7; c++)			// Write image data (FATFS doesn't like writing more than 512 bytes at a time)
					f_write(&file, &picture_buffer[512 * c], 512, &br);

				f_close(&file);

				LPC_GPIO1->DATA |= (1<<8);		// Yellow LED off
			}*/
		} else if (cursor == 1) {
			// Erase
			gbcam_put(0x4000, bank);		// SRAM bank
			delay_us(2);
			gbcam_put(0x0000, 0x0A);		// Enable SRAM writes

		    for (c = 0; c < 0x1000; c++)
		    	gbcam_put(bank_offset + c, 0xFF);

		    prev_picture_number = picture_number + 1;	// Force refresh
		} else if (cursor == 2) {
			fade_out(menu_view);
		}
		return;
	}

	if (picture_number != prev_picture_number) {
		lcd_fill(16, 64, 32, 16, COLOR_BLACK);		// Erase previous picture number

		str_buffer[0] = '0' + (picture_number / 10);		// Draw new picture number
		str_buffer[1] = '0' + (picture_number % 10);
		str_buffer[2] = 0;
		lcd_print(80, 64, str_buffer, COLOR_WHITE, 1);

		// Read SRAM
		bank = (picture_number >> 1) + 1;	// SRAM bank
		gbcam_put(0x4000, bank);
		delay_us(2);

		if (picture_number & 1)				// There are 2 pictures per bank
			bank_offset = 0xB000;
		else
			bank_offset = 0xA000;

		for (c = 0; c < FRAME_SIZE; c++)
			picture_buffer[c] = gbcam_get_ram(bank_offset + c) ^ 0xFF;

		lcd_preview(56, 96);

		prev_picture_number = picture_number;
	}

	if (cursor != cursor_prev) {
		lcd_fill(16, 220 + (cursor_prev * 24), 16, 16, COLOR_BLACK);
		lcd_print(16, 220 + (cursor * 24), "#", COLOR_WHITE, 1);
		cursor_prev = cursor;
	}

}
