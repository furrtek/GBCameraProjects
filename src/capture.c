#include <string.h>
#include "main.h"
#include "views.h"
#include "icons.h"
#include "lcd.h"
#include "io.h"
#include "sdcard.h"
#include "gbcam.h"

const uint16_t lut_2bpp_grey[4] = {
	COLOR565(0x00, 0x00, 0x00),
	COLOR565(0x55, 0x55, 0x55),
	COLOR565(0xAA, 0xAA, 0xAA),
	COLOR565(0xFF, 0xFF, 0xFF)
};
const uint16_t lut_2bpp_dmg[4] = {
	COLOR565(0x00, 0x2B, 0x16),
	COLOR565(0x0A, 0x64, 0x33),
	COLOR565(0x56, 0xAA, 0x2B),
	COLOR565(0xB1, 0xC1, 0x00)
};
const uint16_t lut_2bpp_cgb[4] = {
	COLOR565(0x00, 0x00, 0x00),
	COLOR565(0x00, 0x63, 0xC5),
	COLOR565(0x7B, 0xFF, 0x31),
	COLOR565(0xFF, 0xFF, 0xFF)
};
const uint16_t * lut_2bpp_list[3] = {
	lut_2bpp_grey,
	lut_2bpp_dmg,
	lut_2bpp_cgb
};

const uint16_t bar_colors[8] = {
	COLOR565(0xFF, 0x1F, 0x1F),
	COLOR565(0xFF, 0x7F, 0x1F),
	COLOR565(0x7F, 0xFF, 0x1F),
	COLOR565(0x1F, 0xFF, 0x1F),
	COLOR565(0x1F, 0xFF, 0x1F),
	COLOR565(0xFF, 0xFF, 0x1F),
	COLOR565(0xFF, 0x3F, 0x1F),
	COLOR565(0xFF, 0x1F, 0x1F)
};

void load_lut(uint8_t index) {
	uint8_t c;

	for (c = 0; c < 4; c++)
		lut_2bpp[c] = *(lut_2bpp_list[index] + c);
}

void update_gain() {
	uint16_t c;

	gbcam_set(0x4000, 0x10);		// ASIC registers
	delay_us(10);

	gbcam_set(0xA001, gain >> 4);	// Sensor gain

	// Display gain bar
	c = 76 + (gain >> 1);
	if (c != prev_gain_bar) {
		lcd_fill(prev_gain_bar, 228+12, 4, 6, COLOR_BLACK);
		lcd_fill(c, 228+12, 4, 6, COLOR_WHITE);
	}
	prev_gain_bar = c;
}

void capture_view() {
	uint16_t c;

	memcpy(file_list[0].file_name, "GBCAM000.BIN", 13);

    // ADC setup
    LPC_ADC->CR = (16<<8) | (4<<24) | 1;	// 72/16=4.5MHz, start conversion on rising edge of CT32B0_MAT0
    NVIC->ISER[1] |= (1<<17);				// Enable ADC interrupt
    LPC_ADC->INTEN = 1;						// Interrupt on conversion done

    // Timer
	LPC_TMR32B0->MCR = 2;			// Reset on match 0

	// Initial matrix threshold values (medium-high contrast) taken from real GB Cam
	/*qlevels[0] = 0x8C;
	qlevels[1] = 0x98;
	qlevels[2] = 0xAC;
	qlevels[3] = 0xEB;*/
	// Better blacks:
	qlevels[0] = 0x8C + 5;
	qlevels[1] = 0x98 + 5;
	qlevels[2] = 0xAC + 5;
	qlevels[3] = 0xEB + 5;

	gain = 8 << 4;

	// 2bpp gradient values
	load_lut(0);

    lcd_clear();

	// Grey backdrop around preview
    lcd_fill(0, 48, 240, 144, COLOR_GREY);

	// Draw reference lines
	lcd_hline(56-17, 64+56, 16, COLOR_WHITE);
	lcd_hline(56+129, 64+56, 16, COLOR_WHITE);
	lcd_vline(56+64, 64-17, 16, COLOR_WHITE);
	lcd_vline(56+64, 64+113, 16, COLOR_WHITE);

	// Draw menu
	if (mode == MODE_PHOTO)
		lcd_print(32, 256, "Snap", COLOR_GREEN, 1);
	else
		lcd_print(32, 256, "Record", COLOR_GREEN, 1);
	lcd_print(32, 256+24, "Palette", COLOR_YELLOW, 1);
	lcd_print(32, 256+24+24, "Exit", COLOR_BLUE, 1);

	lcd_print(36, 228, "Exp.:", COLOR_WHITE, 0);
	lcd_print(36, 228+12, "Gain:", COLOR_WHITE, 0);
	for (c = 0; c < 8; c++) {
		lcd_fill(76 + (c << 4), 228+6, 16, 2, bar_colors[c]);
		lcd_fill(76 + (c << 4), 228+12+6, 16, 2, bar_colors[c]);
	}

    gbcam_setmatrix();

    exposure = MAX_EXPOSURE / 2;
    gbcam_setexposure(exposure);

    // Clear GB Cam scratchpad RAM (bank 0, A000~AFFF)
    gbcam_set(0x4000, 0x00);		// SRAM bank 0
    gbcam_set(0x0000, 0x0A);		// Enable SRAM writes
    for (c = 0xA000; c < 0xB000; c++)
    	gbcam_set(c, 0x00);

    update_gain();
	gbcam_set(0xA004, 0x46);		// Edge enhance 200%
	gbcam_set(0xA005, 0x9F);		// Dark level calibration

	prev_expo_status = EXPO_INRANGE;
    cursor_prev = 1;
    cursor = 0;
    //picture_number = 15;			// Used as contrast value here
    prev_picture_number = 1;
    picture_number = 0;
	state = STATE_IDLE;

	fade_in();

	loop_func = capture_loop;
}

void capture_loop() {
	uint16_t c;
	char sn_marker[2] = {'A', 0};			// Audio (# of blocks)
	char im_marker[2] = {'V', 0};			// Video (# of skipped frames since last one)
	uint8_t expo_status, p;
	int16_t histogram_shape;
	int32_t luma_delta;
	uint16_t br;

	read_inputs();

	if (state == STATE_IDLE) {
		// Menu can only be used in REC_IDLE state
		if (inputs_active & BTN_DOWN) {
			if (cursor < 2)
				cursor++;
		} else if (inputs_active & BTN_UP) {
			if (cursor > 0)
				cursor--;
		} else if (inputs_active & BTN_LEFT) {
			if (picture_number)
				picture_number--;
			/*gbcam_setcontrast(picture_number);
		    gbcam_setmatrix();*/
			load_lut(picture_number);	// This should only work when cursor == 1
		} else if (inputs_active & BTN_RIGHT) {
			if (picture_number < 2)
				picture_number++;
			/*if (picture_number < 31)
				picture_number++;
			gbcam_setcontrast(picture_number);
		    gbcam_setmatrix();*/
			load_lut(picture_number);	// This should only work when cursor == 1
		} else if (inputs_active & BTN_A) {
			if (cursor == 0) {
				if (gbcam_ok && sd_ok)
					state = STATE_START;
			} else if (cursor == 2) {
				// Turn off ADC
			    LPC_ADC->CR = 0;
			    NVIC->ICER[1] |= (1<<17);
			    LPC_ADC->INTEN = 0;

				fade_out(menu_view);
				return;
			}
		}
	} else if (state == STATE_REC) {
		if (inputs_active & BTN_A)
			state = STATE_STOP;
	}

	if (cursor != cursor_prev) {
		lcd_fill(16, 256 + (cursor_prev * 24), 16, 16, COLOR_BLACK);
		lcd_print(16, 256 + (cursor * 24), "#", COLOR_WHITE, 1);
		cursor_prev = cursor;
	}

	if (picture_number != prev_picture_number) {
		str_buffer[0] = 'A' + picture_number;
		str_buffer[1] = 0;
		lcd_print(32+128, 256+24, str_buffer, COLOR_YELLOW, 1);
		prev_picture_number = picture_number;
	}

	// Read scratchpad to picture buffer
	gbcam_set(0x4000, 0x00);	// SRAM bank 0
	delay_us(2);
	for (c = 0; c < FRAME_SIZE; c++)
		picture_buffer[c] = gbcam_get(0xA100 + c) ^ 0xFF;

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

	// Display exposure bar
	c = 76 + ((exposure - MIN_EXPOSURE) >> 3);
	if (c != prev_exp_bar) {
		lcd_fill(prev_exp_bar, 228, 4, 6, COLOR_BLACK);
		lcd_fill(c, 228, 4, 6, COLOR_WHITE);
	}
	prev_exp_bar = c;

	// Display histogram
	lcd_fill(208, 148, 14, 28, COLOR_GREY);
	for (p = 0; p < 4; p++) {
		c = histogram[p] >> 9;
		lcd_fill(208 + (p << 2), 148 + (28 - c), 2, c + 1, lut_2bpp[p]);
	}

	// Display audio level
	c = (ad_max >= 127) ? (ad_max - 127) >> 1 : 0;
	lcd_fill(8, 64, 2, 128 - c, COLOR_GREY);
	lcd_fill(8, 64 + 128 - c, 2, c, COLOR_WHITE);

	// Adjust gain according to histogram shape for best contrast
	// Ideally max-min should be == 0, meaning there's an even distribution of all pixel values
	histogram_shape = (histogram[0] + histogram[3]) - (histogram[1] + histogram[2]);	// (Blacks + whites) - (greys)
	if (histogram_shape > 2000) {
		if (gain > (3 << 4)) {
			gain--;
			update_gain();
		}
	} else if (histogram_shape < -2000) {
		if (gain < (11 << 4)) {
			gain++;
			update_gain();
		}
	}/* else {
		gain += (8 - (gain >> 4));
	}*/

	if (state == STATE_START) {
		// Recording start request
		if (!new_file()) {
			lcd_print(24, 216, file_list[0].file_name, COLOR_WHITE, 0);
			if (mode == MODE_VIDEO)
				lcd_paint(192, 36, icon_rec, 1);	// Display "REC" icon
			LPC_GPIO1->DATA &= ~(1<<5);		// Red LED on
			frame_tick = 0;
			video_frame_count = 0;
			audio_frame_count = 0;
			audio_fifo_put = 0;
			audio_fifo_get = 0;
			audio_fifo_ptr = 0;
			rec_timer = 0;
			seconds = 0;
			minutes = 0;
			hours = 0;
			audio_fifo_ready = 0;
			state = STATE_REC;
			LPC_TMR32B0->TCR = 1;
		}
	}

	if (state == STATE_REC) {
		// Recording
		if (frame_tick) {
			frame_tick = 0;

			FCLK_FAST();

			im_marker[1] = skipped;
			video_frame_count += (skipped + 1);
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
					audio_frame_count++;

					if (audio_fifo_get == 5)	// Roll
						audio_fifo_get = 0;
					else
						audio_fifo_get++;

					sn_marker[1]--;
				} while (sn_marker[1]);
			} else if (mode == MODE_PHOTO) {
				state = STATE_STOP;
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
						state = STATE_STOP;
					}
				}
			}

			lcd_print_time(128, 216);
		}
	}

	if (state == STATE_STOP) {
		// Recording stop request
		LPC_TMR32B0->TCR = 0;
		if (mode == MODE_VIDEO)
			lcd_fill(192, 36, 48, 48, COLOR_BLACK);	// Hide "REC" icon
		LPC_GPIO1->DATA |= (1<<5);		// Red LED off
		state = STATE_IDLE;
		f_lseek(&file, 4);
		f_write(&file, &video_frame_count, 4, &br);	// WARNING: This takes skipped frames into account
		f_write(&file, &audio_frame_count, 4, &br);
		f_close(&file);
	}

	// Exposure alerts
	if (expo_status != prev_expo_status) {
		if (expo_status == EXPO_INRANGE)
			lcd_print(56, 200, "In range  ", COLOR_GREEN, 1);
		if (expo_status == EXPO_DARK)
			lcd_print(56, 200, 	"Too dark  ", COLOR_RED, 1);
		if (expo_status == EXPO_BRIGHT)
			lcd_print(56, 200, 	"Too bright", COLOR_RED, 1);
	}

	prev_expo_status = expo_status;

	// Display debug values
	/*str_buffer[0] = hexify((ad_max >> 12) & 0xF);
	str_buffer[1] = hexify((ad_max >> 8) & 0xF);
	str_buffer[2] = hexify((ad_max >> 4) & 0xF);
	str_buffer[3] = hexify(ad_max & 0xF);
	str_buffer[4] = 0;
	lcd_print(208, 180, str_buffer, COLOR_GREY, 0);*/

	// Show last captured image and compute adjustment values
	lcd_preview(56, 64);

	// Wait for capture to end
	gbcam_wait_idle();

	gbcam_setexposure(exposure);
}
