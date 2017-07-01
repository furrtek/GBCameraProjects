#include <string.h>
#include "main.h"
#include "views.h"
#include "icons.h"
#include "lcd.h"
#include "io.h"
#include "sdcard.h"
#include "gbcam.h"

void capture_view() {
	uint16_t c, v, a;
	uint8_t lev, m;
	uint8_t qlevels[4] = {	0x8F,	// Initial matrix computation values (medium-high contrast)
							0x98,
							0xA6,
							0xB9};

	memcpy(file_name, "GBCAM000.BIN", 13);

    lcd_clear();

	// Grey backdrop around preview
    lcd_fill(0, 48, 240, 144, COLOR_GREY);

	// Draw reference lines
	lcd_hline(56-17, 64+56, 16, COLOR_WHITE);
	lcd_hline(56+129, 64+56, 16, COLOR_WHITE);
	lcd_vline(56+64, 64-17, 16, COLOR_WHITE);
	lcd_vline(56+64, 64+113, 16, COLOR_WHITE);

	if (mode == MODE_PHOTO)
		lcd_print(32, 244, "Snap !", COLOR_GREEN, 1);
	else
		lcd_print(32, 244, "Record", COLOR_GREEN, 1);
	lcd_print(32, 244+24, "Exit", COLOR_BLUE, 1);

    // Compute GB Cam dithering matrix values (see GB Cam ROM)
    for (lev = 0; lev < 3; lev++) {
    	v = qlevels[lev];
    	a = (qlevels[lev + 1] - v) << 4;
    	v = v << 8;
		for (m = 0; m < 16; m++) {
			gbcam_matrix[lev + (matrix_layout[m] * 3)] = v >> 8;
			v += a;
		}
    }
    gbcam_setmatrix(gbcam_matrix);

    exposure = MAX_EXPOSURE / 2;
    gbcam_setexposure(exposure);	// Set image sensor exposure value

	//TMR32B0 is used for recording timing (audio/video)
	//72MHz/8192Hz = 8789 with match at 4 (why not ?) and /2 = 1099
	LPC_TMR32B0->PR = 1099;
	LPC_TMR32B0->MCR = 2;		// Reset on match 0
	LPC_TMR32B0->MR0 = 3;		// Count 0~3 (/4)
	LPC_TMR32B0->EMR = (3<<4);
	LPC_TMR32B0->TCR = 0;

    // Clear GB Cam scratchpad RAM (bank 0, A000~AFFF)
    gbcam_set(0x4000, 0x00);		// SRAM bank 0
    gbcam_set(0x0000, 0x0A);		// Enable SRAM writes
    for (c = 0xA000; c < 0xB000; c++)
    	gbcam_set(c, 0x00);

	gbcam_set(0x4000, 0x10);		// ASIC registers
	delay_us(2);
	gbcam_set(0xA001, 0x08);		// Sensor gain
	gbcam_set(0xA004, 0x46);		// Edge enhance 200%
	gbcam_set(0xA005, 0x9F);		// Dark level calibration

	prev_expo_status = EXPO_INRANGE;
    cursor_prev = 1;
    cursor = 0;
	recording = REC_IDLE;

	fade_in();

	loop_func = capture_loop;
}

uint8_t hexify(uint8_t d) {
	if (d > 9)
		d += 7;

	return 0x30 + d;
}

void capture_loop() {
	uint16_t c, text_color;
	char sn_marker[2] = {'A', 0};			// Audio (# of blocks)
	char im_marker[2] = {'V', 0};			// Video (# of skipped frames since last one)
	uint8_t data, expo_status;
	uint16_t luma_acc = 0;
	int32_t luma_delta;
	uint16_t br;

	read_inputs();

	if (recording == REC_IDLE) {
		if (inputs_active & BTN_DOWN) {
			if (cursor < 1)
				cursor++;
		} else if (inputs_active & BTN_UP) {
			if (cursor > 0)
				cursor--;
		} else if (inputs_active & BTN_A) {
			if (cursor == 0) {
				if (can_record)
					recording = REC_START;
			} else if (cursor == 1) {
				mode = MODE_VIDEO;
				fade_out(menu_view);
				return;
			}
		}
	} else if (recording == REC_WORK) {
		recording = REC_STOP;
	}

	if (cursor != cursor_prev) {
		lcd_fill(16, 244 + (cursor_prev * 24), 16, 16, COLOR_BLACK);
		lcd_print(16, 244 + (cursor * 24), "#", COLOR_WHITE, 1);
		cursor_prev = cursor;
	}

	// Read scratchpad
	gbcam_set(0x4000, 0x00);	// SRAM bank 0
	delay_us(2);
	//LPC_GPIO1->DATA &= ~(1<<5);		// Red LED on DEBUG
	//LPC_GPIO1->DATA |= (1<<5);		// Red LED off DEBUG
	for (c = 0; c < FRAME_SIZE; c++) {
		data = gbcam_get(0xA100 + c) ^ 0xFF;
		picture_buffer[c] = data;
		luma_acc += ((data>>6) & 3) + ((data>>4) & 3) + ((data>>2) & 3) + (data & 3);
	}

	// Ask ASIC for capture
	gbcam_set(0x4000, 0x10);	// ASIC registers
	delay_us(2);
	gbcam_set(0xA000, 0x03);

	// Wait for capture to start
	gbcam_wait_busy();

	// Auto-exposure (progressive): 128 * 112 * 3 (all pixels white) / 2 = 21504
	luma_delta = (21504 - luma_acc) >> 9;

	// Cap exposure and alert
	if ((int32_t)exposure + luma_delta > MAX_EXPOSURE) {
		expo_status = EXPO_DARK;
		exposure = MAX_EXPOSURE;
	} else if ((int32_t)exposure + luma_delta < MIN_EXPOSURE) {
		expo_status = EXPO_BRIGHT;
		exposure = MIN_EXPOSURE;
	} else {
		expo_status = EXPO_INRANGE;
		exposure += luma_delta;
	}

	// Exposure value is yellow if there's an abrupt lighting change
	if ((luma_delta > 30) || (luma_delta < -30))
		text_color = COLOR_YELLOW;
	else
		text_color = COLOR_GREEN;

	// Display exposure value
	str_buffer[0] = hexify((exposure >> 12) & 0xF);
	str_buffer[1] = hexify((exposure >> 8) & 0xF);
	str_buffer[2] = hexify((exposure >> 4) & 0xF);
	str_buffer[3] = hexify(exposure & 0xF);
	str_buffer[4] = 0;
	lcd_print(56, 194, str_buffer, text_color, 0);

	if (recording == REC_START) {
		// Recording start request
		if (!new_file()) {
			lcd_print(56, 220, file_name, COLOR_WHITE, 0);
			if (mode == MODE_VIDEO)
				lcd_paint(192, 36, icon_rec, 1);	// Display "REC" icon
			LPC_GPIO1->DATA &= ~(1<<5);		// Red LED on
			write_frame_request = 0;
			audio_fifo_put = 0;
			audio_fifo_ptr = 0;
			rec_timer = 0;
			seconds = 0;
			minutes = 0;
			hours = 0;
			audio_fifo_ready = 0;
			recording = REC_WORK;
			LPC_TMR32B0->TCR = 1;
		}
	}

	if (recording == REC_WORK) {
		// Recording
		if (write_frame_request) {
			write_frame_request = 0;

			FCLK_FAST();

			im_marker[1] = skipped;
			skipped = 0;

			LPC_GPIO1->DATA &= ~(1<<8);		// Yellow LED on

			f_write(&file, &im_marker, 2, &br);		// Write image marker
			for (c = 0; c < 7; c++)					// Write image data (FATFS doesn't like writing more than 512 bytes at a time)
				f_write(&file, &picture_buffer[512 * c], 512, &br);

			if (audio_fifo_ready && (mode == MODE_VIDEO)) {
				sn_marker[1] = audio_fifo_ready;
				audio_fifo_ready = 0;

				f_write(&file, &sn_marker, 2, &br);	// Write audio marker
				do {								// Write audio data
					f_write(&file, &audio_fifo[audio_fifo_get], 512, &br);

					if (audio_fifo_get == 5)	// Roll
						audio_fifo_get = 0;
					else
						audio_fifo_get++;

					sn_marker[1]--;
				} while (sn_marker[1]);
			} else if (mode == MODE_PHOTO) {
				recording = REC_STOP;
			}

			LPC_GPIO1->DATA |= (1<<8);		// Yellow LED off

			FCLK_LCD();
		}

		// Update recording time every second
		if ((rec_timer >= 100) && (mode == MODE_VIDEO)) {
			rec_timer -= 100;
			if (seconds < 59) {
				seconds++;
			} else {
				seconds = 0;
				if (minutes < 59) {
					minutes++;
				} else {
					minutes = 0;
					if (hours < 99) {
						hours++;
					} else {
						// Recording time exceeded, forgot to turn it off ?
						recording = REC_STOP;
					}
				}
			}

			str_buffer[0] = 0x30 + (hours / 10);
			str_buffer[1] = 0x30 + (hours % 10);
			str_buffer[2] = ':';
			str_buffer[3] = 0x30 + (minutes / 10);
			str_buffer[4] = 0x30 + (minutes % 10);
			str_buffer[5] = ':';
			str_buffer[6] = 0x30 + (seconds / 10);
			str_buffer[7] = 0x30 + (seconds % 10);

			lcd_print(56, 220, str_buffer, COLOR_WHITE, 0);
		}
	}

	if (recording == REC_STOP) {
		// Recording stop request
		if (mode == MODE_VIDEO)
			lcd_fill(192, 36, 48, 48, COLOR_BLACK);	// Hide "REC" icon
		LPC_TMR32B0->TCR = 0;
		LPC_GPIO1->DATA |= (1<<5);		// Red LED off
		recording = REC_IDLE;
		f_close(&file);
	}

	// Exposure alerts
	if (expo_status != prev_expo_status) {
		if (expo_status == EXPO_INRANGE)
			lcd_print(56, 204, "In range  ", COLOR_GREEN, 1);
		if (expo_status == EXPO_DARK)
			lcd_print(56, 204, 	"Too dark  ", COLOR_RED, 1);
		if (expo_status == EXPO_BRIGHT)
			lcd_print(56, 204, 	"Too bright", COLOR_RED, 1);
	}

	prev_expo_status = expo_status;

	// Show last captured image
	lcd_preview(56, 64);

	// Wait for capture to end
	gbcam_wait_idle();

	gbcam_setexposure(exposure);
}
