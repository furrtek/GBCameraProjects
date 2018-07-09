/*
===============================================================================
 Name        : GBCamcorder

 Author      : furrtek
 Version     : 0.3
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/

#ifndef MAIN_H_
#define MAIN_H_

#include "LPC13xx.h"
#include "ff.h"

#define FW_STRING "FW 08072018"

#define MAX_AUDIO_BUFFERS 5

#define MENU_BEEP beep(400, 2, 40);
#define VALIDATE_BEEP beep(800, 2, 40);

// Button bit masks
#define BTN_UP 1
#define BTN_DOWN 8
#define BTN_LEFT 2
#define BTN_RIGHT 4
#define BTN_A 16

// For FATFS
FATFS FatFs;
FIL file;
FILINFO file_info;
DIR dir;
FRESULT fr;

// File stuff
char file_buffer[4];
struct {
	char file_name[13];
	uint32_t duration;
} file_list[8];

void hex_insert(uint32_t pos, uint8_t d);
void beep(const uint32_t frequency, const uint32_t duration, const uint32_t volume);
void print_error(uint8_t x, uint8_t y, uint8_t fr);
void systick_wait(const uint32_t duration);

void (*loop_func)(void);		// Loop function pointer for each view
void (*slot_func)(void);		// SD card or GB cam state change function pointer for each view

uint8_t gbcam_matrix[48];		// Dithering/ADC matrix
uint8_t bank;					// For SRAM dump
uint16_t bank_offset;

// UI stuff
char str_buffer[32];				// For lcd_print with dynamic data
uint16_t backlight;					// For fade in/out
int8_t cursor, cursor_prev, cursor_max;	// For menus
uint8_t refresh_req;
uint32_t picture_number, prev_picture_number;

// Timing/inputs
volatile uint32_t systick;			// Used for 10ms timing
uint16_t check_timer;				// For SD and GB Cam detection
uint8_t sd_ok, sd_ok_prev, gbcam_ok, gbcam_ok_prev;			// Detection flags

#endif /* MAIN_H_ */
