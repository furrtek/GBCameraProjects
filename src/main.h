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

#define FW_STRING "FW 02072017"

#define COLOR_RED 		0b1111100000000000
#define COLOR_GREEN 	0b0000011111100000
#define COLOR_BLUE		0b0000000000011111
#define COLOR_YELLOW	0b1111111111100000
#define COLOR_ORANGE	0b1111101111100000
#define COLOR_CYAN		0b0000011111111111
#define COLOR_WHITE 	0b1111111111111111
#define COLOR_GREY		0b0111101111101111
#define COLOR_BLACK		0b0000000000000000

#define BTN_UP 1
#define BTN_DOWN 8
#define BTN_LEFT 2
#define BTN_RIGHT 4
#define BTN_A 16

#define MODE_VIDEO 0
#define MODE_PHOTO 1
#define MODE_SETTINGS 2

volatile enum state_t {
	STATE_IDLE = 0,
	STATE_START,
	STATE_REC,
	STATE_PLAY,
	STATE_STOP
} state;

#define FRAME_SIZE 	3584			// 128 * 112 * 2bpp / 8 bits

#define EXPO_INRANGE	0
#define EXPO_DARK		1
#define EXPO_BRIGHT		2

#define	MAX_EXPOSURE 1000		// ((acqtime/953.6)-32446)/16
#define	MIN_EXPOSURE 40

FATFS FatFs;
FIL file;
FILINFO file_info;
DIR dir;
FRESULT fr;
void (*loop_func)(void);			// Loop function pointer for each view
uint8_t mode;						// Capture mode
uint8_t cursor, cursor_prev, cursor_max;	// For menus
uint8_t picture_buffer[FRAME_SIZE];
char str_buffer[32];				// For lcd_print with dynamic data
uint16_t exposure;
uint8_t gbcam_matrix[48];			// Dithering/ADC matrix
extern const uint8_t matrix_layout[16];
uint8_t seconds, minutes, hours;	// Recording duration counter
uint8_t prev_expo_status;
uint8_t picture_number, picture_number_prev;
char file_buffer[4];
struct {
	char file_name[13];
	uint32_t duration;
} file_list[8];
uint8_t bank;						// For SRAM dump
uint16_t bank_offset;
uint16_t backlight;					// For fade in/out
uint8_t sd_ok, gbcam_ok;			// Detection flags
uint32_t video_frame_count;
uint32_t audio_frame_count;

// Audio ring FIFO
volatile uint8_t audio_fifo[6][512];
volatile uint8_t audio_fifo_ready;
volatile uint8_t audio_fifo_get;
volatile uint8_t audio_fifo_put;
volatile uint16_t audio_fifo_ptr;

// Timing/inputs
volatile uint8_t systick;			// Used for 10ms timing
uint8_t rec_timer;					// Used for 1s timing
uint16_t slots_timer;				// For SD and GB Cam detection
volatile uint8_t skipped;
uint8_t inputs_prev, inputs_current, inputs_active;
volatile uint8_t frame_tick;

#endif /* MAIN_H_ */
