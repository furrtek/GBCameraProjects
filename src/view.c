/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.3
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/
#include <string.h>
#include "main.h"
#include "colors.h"
#include "views.h"
#include "capture.h"
#include "icons.h"
#include "lcd.h"
#include "io.h"
#include "sdcard.h"
#include "gbcam.h"

// List 8 files, returns 1 if no more files found
uint8_t list_files(uint16_t page) {
	uint8_t c, s;
	uint16_t br, ff;
	uint32_t duration;

    c = 0;
    ff = page * 8;
	FCLK_FAST();
    fr = f_opendir(&dir, "/");

    if (fr == FR_OK) {
        for (;;) {
    		FCLK_FAST();
        	fr = f_readdir(&dir, &file_info);
            if (file_info.fname[0] == 0)
            	break;
            //if (fr != FR_OK || file_info.fname[0] == 0)
            //	break;
            //if (!ff) {
				if (!(file_info.fattrib & AM_DIR)) {
			        lcd_print(32, 216 + (c << 3), file_info.fname, COLOR_WHITE, 0);

		    		FCLK_FAST();
					fr = f_open(&file, file_info.fname, FA_READ);
					if (fr == FR_OK) {
						// Check file header
						fr = f_read(&file, file_buffer, 4, &br);
						// fr == FR_OK
						for (s = 0; s < 4; s++)
							if (file_buffer[c] != file_header[c]) break;
						if (s == 4) {
							f_read(&file, &file_list[c].duration, 4, &br);
							if (file_list[c].duration == 1) {
								file_list[c].duration = 0;			// Picture marker
							} else {
								file_list[c].duration *= 625;		// Frame duration (1/16s) to seconds
								file_list[c].duration /= 10000;
							}
							memcpy(&file_list[c].file_name, file_info.fname, 13);
							//c++;
						}
						c++;
						f_close(&file);
						if (c == 8)
							break;
					}
				}
            //} else {
            //	ff--;
            //}
        }
        f_closedir(&dir);

        if (!c)
        	return 1;

        // Fill if needed
        for (; c < 8; c++)
        	file_list[c].file_name[0] = 0;
    } else
    	return 1;

    cursor_max = 0;

    // Clear shown file list
    lcd_fill(32, 216, 208, 64, COLOR_BLACK);

    // Display file list
    for (c = 0; c < 8; c++) {
    	if (!file_list[c].file_name[0])
    		break;
        lcd_print(32, 216 + (c * 8), file_list[c].file_name, COLOR_WHITE, 0);
        duration = file_list[c].duration;
        if (!duration) {
            lcd_print(144, 216 + (c * 8), "PICTURE", COLOR_WHITE, 0);
        } else {
            str_buffer[7] = 0x30 + (duration % 10);
            str_buffer[6] = 0x30 + (duration / 10);
            duration /= 60;
            str_buffer[5] = ':';
            str_buffer[4] = 0x30 + (duration % 10);
            str_buffer[3] = 0x30 + (duration / 10);
            duration /= 60;
            str_buffer[2] = ':';
            str_buffer[1] = 0x30 + (duration % 10);
            str_buffer[0] = 0x30 + (duration / 10);
            str_buffer[8] = 0;
            lcd_print(144, 216 + (c * 8), str_buffer, COLOR_WHITE, 0);
        }
        cursor_max++;
    }

    if (cursor_max)		// Should always be the case here
    	cursor_max--;

	cursor = 0;

    return 0;
}

void view_view() {
    lcd_clear();

    // TMR32B0 is used directly here for playback timing (no ADC trigger)
	LPC_TMR32B0->MCR = 3;			// Reset AND interrupt on match 0
    NVIC->ISER[1] |= (1<<11);		// Enable timer interrupt

    lcd_print(32, 208, "FILE NAME     DURATION", COLOR_GREEN, 0);
    lcd_print(28, 296, "Press RIGHT+A to delete", COLOR_GREY, 0);
    lcd_print(48, 304, "Press LEFT to exit", COLOR_GREY, 0);

    picture_number = 0;				// Used as page number for the file list

    list_files(picture_number);

    state = STATE_IDLE;
    cursor_prev = 1;
    cursor = 0;

	fade_in();

	loop_func = view_loop;
}

void view_loop() {
	uint16_t br;

	systick_wait(2);	// 20ms

	read_inputs();

	if (inputs_active & BTN_DOWN) {
		if (cursor < cursor_max) {
			cursor++;
		} else {
			// Try next page
			if (!list_files(picture_number + 1))
				picture_number++;
		}
	} else if (inputs_active & BTN_UP) {
		if (cursor > 0) {
			cursor--;
		} else {
			if (picture_number) {
				// Try previous page
				if (!list_files(picture_number - 1))
					picture_number--;
			}
		}
	}
	if (inputs_active & BTN_A) {
		if (inputs_current & BTN_RIGHT) {
			// Delete file
			fr = f_unlink(file_list[cursor].file_name);
			if (fr == FR_OK)
				list_files(picture_number);	// Refresh on success
		} else {
			if (state == STATE_IDLE)
				state = STATE_START;
			if (state == STATE_PLAY)
				state = STATE_STOP;
			VALIDATE_BEEP
		}
	}
	if ((inputs_active & BTN_LEFT) && (state == STATE_IDLE)) {
	    NVIC->ICER[1] |= (1<<11);		// Disable timer interrupt
		VALIDATE_BEEP
		fade_out(menu_view);
		return;
	}

	if (cursor != cursor_prev) {
		// Show first frame of file
		fr = f_open(&file, file_list[cursor].file_name, FA_READ);
		if (fr == FR_OK) {
			f_lseek(&file, 16);
			fr = f_read(&file, file_buffer, 2, &br);		// Read block ID
			if (file_buffer[0] == 'V') {
				f_read(&file, picture_buffer, FRAME_SIZE, &br);
				lcd_preview(56, 64);
			}
			f_close(&file);
		}

		lcd_fill(24, 216 + (cursor_prev * 8), 8, 8, COLOR_BLACK);
		lcd_print(24, 216 + (cursor * 8), "#", COLOR_WHITE, 0);
		cursor_prev = cursor;
		MENU_BEEP
	}

	if (state == STATE_START) {
		// Play start request
		fr = f_open(&file, file_list[cursor].file_name, FA_READ);
		if (fr == FR_OK) {
			f_lseek(&file, 16);
			audio_fifo_ptr = 0;		// Used here for timing
			frame_tick = 0;
			skipped = 0;
			rec_timer = 0;
			seconds = 0;
			minutes = 0;
			hours = 0;
			state = STATE_PLAY;
			LPC_TMR32B0->TCR = 1;
		}
	}

	if (state == STATE_PLAY) {
		// Playing
		if (frame_tick) {
			frame_tick = 0;

			if (skipped) {
				skipped--;
			} else {
				f_read(&file, file_buffer, 2, &br);		// Read block ID
				if (!br)
					state = STATE_STOP;					// End of file
				skipped = file_buffer[1];

				if (file_buffer[0] == 'V') {
					f_read(&file, picture_buffer, FRAME_SIZE, &br);
					lcd_preview(56, 64);
				} else if (file_buffer[0] == 'A') {
					f_lseek(&file, f_tell(&file) + (skipped * 512));	// Skip audio
				} else {
					state = STATE_STOP;					// Bad block ID
				}
			}
		}

		// Update playing time every second
		if (rec_timer >= 100) {
			rec_timer -= 100;
			if (seconds < 59) {
				seconds++;
			} else {
				seconds = 0;
				if (minutes < 59) {
					minutes++;
				} else {
					minutes = 0;
					if (hours < 99)
						hours++;
				}
			}

			lcd_print_time(88, 180);
		}
	}

	if (state == STATE_STOP) {
		LPC_TMR32B0->TCR = 0;
		f_close(&file);
		state = STATE_IDLE;
	}
}
