#include "main.h"
#include "views.h"
#include "io.h"
#include "icons.h"
#include "lcd.h"

void menu_view() {
    lcd_clear();

	lcd_print(32, 96, "Photo", COLOR_RED, 1);
	lcd_print(32, 96+24, "Video", COLOR_GREEN, 1);
	lcd_print(32, 96+24+24, "Settings", COLOR_BLUE, 1);
	lcd_print(32, 96+24+24+24, "SRAM dump", COLOR_YELLOW, 1);
	lcd_print(32, 96+24+24+24+24, "About", COLOR_WHITE, 1);

    lcd_paint(75, 268, logo_fe, 0);
	lcd_print(76, 304, FW_STRING, COLOR_GREY, 0);

	cursor = 0;
	cursor_prev = 1;

	fade_in();

	loop_func = menu_loop;
}

void menu_loop() {
	systick = 0;
	while (systick < 2);	// 20ms

	read_inputs();

	if (inputs_active & BTN_DOWN) {
		if (cursor == 4)
			cursor = 0;
		else
			cursor++;
	} else if (inputs_active & BTN_UP) {
		if (cursor == 0)
			cursor = 4;
		else
			cursor--;
	} else if (inputs_active & BTN_A) {
		if (cursor == 0) {
			mode = MODE_PHOTO;
			fade_out(capture_view);
		} else if (cursor == 1) {
			mode = MODE_VIDEO;
			fade_out(capture_view);
		} else if (cursor == 2) {
			mode = MODE_SETTINGS;
			fade_out(capture_view);
		} else if (cursor == 3) {
			fade_out(sram_view);
		} else if (cursor == 4) {
			fade_out(about_view);
		}
		return;
	}

	if (cursor != cursor_prev) {
		lcd_fill(16, 96 + (cursor_prev * 24), 16, 16, COLOR_BLACK);
		lcd_print(16, 96 + (cursor * 24), "#", COLOR_WHITE, 1);
		cursor_prev = cursor;
	}
}
