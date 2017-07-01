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

#define FW_STRING "FW 01072017"

#define COLOR_RED 		0b1111100000000000
#define COLOR_GREEN 	0b0000011111100000
#define COLOR_BLUE		0b0000000000011111
#define COLOR_YELLOW	0b1111111111100000
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

volatile enum recording_t {
	REC_IDLE = 0,
	REC_START,
	REC_WORK,
	REC_STOP
} recording;

#define FRAME_SIZE 	3584			// 128 * 112 * 2bpp / 8 bits

#define EXPO_INRANGE	0
#define EXPO_DARK		1
#define EXPO_BRIGHT		2

#define	MAX_EXPOSURE 1000		// ((acqtime/953.6)-32446)/16
#define	MIN_EXPOSURE 40

volatile uint8_t systick;
uint8_t mode;
uint8_t cursor, cursor_prev;
void (*loop_func)(void);
FATFS FatFs;
FIL file;
uint8_t picture_buffer[FRAME_SIZE];
uint8_t inputs_prev, inputs_current, inputs_active;
char rbf[32];
uint16_t exposure;
uint8_t gbcam_matrix[48];		// Dithering/ADC matrix
FRESULT fr;
uint8_t rec_timer;
uint8_t seconds, minutes, hours;
uint8_t prev_expo_status;
uint8_t picture_number, picture_number_prev;
extern char file_name[13];
uint8_t bank;
uint16_t bank_offset;
uint16_t backlight;

extern const uint8_t matrix_layout[16];

// Audio ring FIFO
volatile uint8_t audio_fifo[6][512];
volatile uint8_t audio_fifo_ready;
volatile uint8_t audio_fifo_get;
volatile uint8_t audio_fifo_put;
volatile uint16_t audio_fifo_ptr;
volatile uint8_t write_frame_request;

// Timing/inputs
volatile uint8_t skipped;

// Flags
volatile uint8_t can_record;

#endif /* MAIN_H_ */
