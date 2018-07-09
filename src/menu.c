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
#include "capture.h"
#include "tool.h"
#include "colors.h"
#include "io.h"
#include "icons.h"
#include "lcd.h"

#define MENU_MAX 5

typedef struct {
	uint16_t y;
	char * string;
	uint16_t color;
	void (*func)(void);
} menu_item_t;

void func_photo(void) {
	mode = MODE_PHOTO;
	fade_out(capture_view);
}
void func_video(void) {
	mode = MODE_VIDEO;
	fade_out(capture_view);
}
void func_view(void) {
	fade_out(view_view);
}
void func_sram(void) {
	fade_out(sram_view);
}
void func_tool(void) {
	fade_out(tool_view);
}
void func_about(void) {
	fade_out(about_view);
}

static const menu_item_t menu_items[6] = {
	{ 96+0*24, "Photo",	COLOR_RED, func_photo },
	{ 96+1*24, "Video",	COLOR_ORANGE, func_video },
	{ 96+2*24, "View",	COLOR_GREEN, func_view },
	{ 96+3*24, "Dump",	COLOR_YELLOW, func_sram },
	{ 96+4*24, "Game tool",	COLOR_CYAN, func_tool },
	{ 96+5*24, "About",	COLOR_BLUE, func_about }
};

uint8_t menu_item_enabled[6] = { 0, 0, 0, 0, 0, 1 };

void menu_move_cursor(const int8_t direction) {
	do {
		cursor += direction;

		if (cursor > MENU_MAX)
			cursor = 0;
		else if (cursor < 0)
			cursor = MENU_MAX;

	} while (!menu_item_enabled[cursor]);

	MENU_BEEP
}

void menu_draw() {
	const menu_item_t * menu_item;
	uint16_t color;

	if (!menu_item_enabled[cursor])
		menu_move_cursor(1);

	for (uint32_t c = 0; c <= MENU_MAX; c++) {
		menu_item = &menu_items[c];
		if (menu_item_enabled[c])
			color = menu_item->color;
		else
			color = COLOR_GREY;
		lcd_print(48, menu_item->y, menu_item->string, color, 1);
	}
}

void menu_slot_func(void) {
	if (sd_ok) {
		menu_item_enabled[2] = 1;
		menu_item_enabled[4] = 1;
		if (gbcam_ok) {
			menu_item_enabled[0] = 1;
			menu_item_enabled[1] = 1;
			menu_item_enabled[3] = 1;
		}
	}

	refresh_req = 1;
}

void menu_view() {
    lcd_clear();

    menu_draw();

    lcd_paint(75, 268, logo_fe, 0);
	lcd_print(76, 304, FW_STRING, COLOR_GREY, 0);

	cursor = 0;
	cursor_prev = 1;		// Force cursor update

	fade_in();

	loop_func = menu_loop;
	slot_func = menu_slot_func;
}

void menu_loop() {
	systick_wait(2);		// 20ms (50Hz update rate)

	read_inputs();

	if (refresh_req)
		menu_draw();

	if (inputs_active & BTN_DOWN) {
		menu_move_cursor(1);
	} else if (inputs_active & BTN_UP) {
		menu_move_cursor(-1);
	} else if (inputs_active & BTN_A) {
		VALIDATE_BEEP
		menu_items[cursor].func();
		return;
	}

	// Refresh cursor if needed
	if (cursor != cursor_prev) {
		lcd_fill(32, 96 + (cursor_prev * 24), 16, 16, COLOR_BLACK);
		lcd_print(32, 96 + (cursor * 24), "#", COLOR_WHITE, 1);
		cursor_prev = cursor;
	}
}
