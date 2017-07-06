/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.2
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/
#ifndef MAIN_H_
#define MAIN_H_

#include "LPC13xx.h"
#include "ff.h"

#define FW_STRING "FW 05072017"

// Button bit masks
#define BTN_UP 1
#define BTN_DOWN 8
#define BTN_LEFT 2
#define BTN_RIGHT 4
#define BTN_A 16

// Capture modes
#define MODE_VIDEO 0
#define MODE_PHOTO 1
#define MODE_SETTINGS 2

// Capture states
volatile enum state_t {
	STATE_IDLE = 0,
	STATE_START,
	STATE_REC,
	STATE_PLAY,
	STATE_STOP
} state;

#define FRAME_SIZE 	3584		// 128 * 112 * 2bpp / 8 bits

#define EXPO_INRANGE	0
#define EXPO_DARK		1
#define EXPO_BRIGHT		2

#define	MAX_EXPOSURE 1000		// ((acqtime/953.6)-32446)/16
#define	MIN_EXPOSURE 40

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

void (*loop_func)(void);		// Loop function pointer for each view
uint8_t mode;							// Capture mode
uint8_t picture_buffer[FRAME_SIZE];
uint16_t exposure;
uint8_t gbcam_matrix[48];				// Dithering/ADC matrix
extern const uint8_t matrix_layout[16];
extern uint8_t qlevels[4];
uint8_t seconds, minutes, hours;		// Recording duration counter
uint8_t gain;
uint16_t luma_acc;
uint16_t histogram[4];
uint32_t video_frame_count;
uint32_t audio_frame_count;
uint8_t bank;						// For SRAM dump
uint16_t bank_offset;

// UI stuff
char str_buffer[32];				// For lcd_print with dynamic data
uint16_t backlight;					// For fade in/out
uint8_t cursor, cursor_prev, cursor_max;	// For menus
uint16_t lut_2bpp[4];
uint8_t picture_number, prev_picture_number;
uint16_t prev_gain_bar, prev_exp_bar;
uint8_t prev_expo_status;

// Audio stuff
volatile uint8_t audio_fifo[6][512];
volatile uint8_t audio_fifo_ready;
volatile uint8_t audio_fifo_get;
volatile uint8_t audio_fifo_put;
volatile uint16_t audio_fifo_ptr;
volatile uint8_t audio_max;

// Timing/inputs
volatile uint8_t systick;			// Used for 10ms timing
uint8_t rec_timer;					// Used for 1s timing
uint16_t check_timer;				// For SD and GB Cam detection
volatile uint8_t skipped;
uint8_t inputs_prev, inputs_current, inputs_active;
volatile uint8_t frame_tick;
uint8_t sd_ok, gbcam_ok;			// Detection flags

#endif /* MAIN_H_ */
