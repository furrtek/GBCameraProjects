/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.2
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/
#ifndef CAPTURE_H_
#define CAPTURE_H_

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

void set_palette();

uint8_t mode;							// Capture mode
uint8_t picture_buffer[FRAME_SIZE];
uint16_t exposure;
extern const uint8_t matrix_layout[16];
extern uint8_t qlevels[4];
uint8_t seconds, minutes, hours;		// Recording duration counter
uint8_t gain;
uint16_t luma_acc;
uint16_t histogram[4];
uint32_t video_frame_count;
uint32_t audio_frame_count;

const uint16_t * lut_2bpp;
uint8_t palette_number, prev_palette_number;
uint16_t prev_gain_bar, prev_exp_bar;
uint8_t prev_expo_status;

// Audio stuff
volatile uint8_t audio_fifo[MAX_AUDIO_BUFFERS][512];
volatile uint8_t audio_fifo_ready;
volatile uint8_t audio_fifo_get;
volatile uint8_t audio_fifo_put;
volatile uint16_t audio_fifo_ptr;
volatile uint8_t audio_max;

// Timing/inputs
uint8_t rec_timer;					// Used for 1s timing
volatile uint8_t skipped;
volatile uint8_t frame_tick;

#endif /* CAPTURE_H_ */
