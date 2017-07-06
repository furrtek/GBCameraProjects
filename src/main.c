/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.2
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "colors.h"
#include "views.h"
#include "gbcam.h"
#include "sdcard.h"
#include "diskio.h"
#include "ff.h"
#include "io.h"
#include "lcd.h"
#include "icons.h"

// TO CHECK: SDCS and LCDCS pins are open-drain, check why they don't go down to GND when data reg set to 1
//				SDCS drops to ~2V, which is probably too high to be registered as low -> init bug
// TODO: merge image conversion code between lcd_preview() and save_bmp()
// TODO: lcd_preview doesn't work before going into capture mode
// TODO: File list stops where it shouldn't, see view.c
// TODO: File naming ?

void SysTick_Handler(void) {
	if (systick < 255)
		systick++;

	if (check_timer)
		check_timer--;

	rec_timer++;
}

void TIMER32_0_IRQHandler(void) {
	// Simulates recording timing for playback
	if (audio_fifo_ptr == 511) {
		audio_fifo_ptr = 0;

		frame_tick = 1;				// This sets the framerate (8192/512 = 16 fps)
	} else {
		audio_fifo_ptr++;
	}

	LPC_TMR32B0->IR = 1;			// Ack interrupt
    NVIC->ICPR[1] = (1<<11);		// Ack interrupt
}

void ADC_IRQHandler(void) {
	uint32_t ad_stat;
	uint32_t ad_data;

	ad_stat = LPC_ADC->STAT;
	(void)ad_stat;

	ad_data = (LPC_ADC->DR0 >> 8) & 0xFF;
	audio_fifo[audio_fifo_put][audio_fifo_ptr] = ad_data;
	if (ad_data > audio_max)
		audio_max = ad_data;		// Store peak level

	if (audio_fifo_ptr == 511) {
		audio_fifo_ptr = 0;
		audio_max = 0;

		if (frame_tick)
			skipped++;

		frame_tick = 1;				// This sets the framerate (8192/512 = 16 fps)

		if (audio_fifo_ready < 5)
			audio_fifo_ready++;		// This should NEVER go over 5 (SD card too slow or stalled ?)

		if (audio_fifo_put == 5)	// Roll
			audio_fifo_put = 0;
		else
			audio_fifo_put++;
	} else {
		audio_fifo_ptr++;
	}

    NVIC->ICPR[1] = (1<<17);		// Ack interrupt
}

int main(void) {
	// Should already be set correctly in SystemInit()...
    LPC_SYSCON->SYSAHBCLKCTRL = (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<6) |
    							(1<<7) | (1<<8) | (1<<9) | (1<<10) | (1<<11) | (1<<13) | (1<<16);

    SysTick->LOAD = 0x000AFC7F;		// 10ms tick
    SysTick->VAL = 0;
    SysTick->CTRL = 7;

	backlight = 0;
    check_timer = 0;
    sd_ok = 0;
    gbcam_ok = 0;

	systick = 0;
	while (systick < 10);	// 200ms

    init_io();

	// TMR16B0 is used for delays (1us tick @ 72MHz, do NOT modify !)
	LPC_TMR16B0->PR = 72;
	LPC_TMR16B0->TCR = 1;

	// TMR16B1 is used for Phi generation (GB CPU frequency / 4)
	LPC_TMR16B1->PR = 0;
    LPC_TMR16B1->MCR = 0x0400;		// Reset on match register 3
    LPC_TMR16B1->MR0 = 17;			// Not used ?
	LPC_TMR16B1->MR3 = 34;			// ~1048576Hz (can go faster if needed)
	LPC_TMR16B1->EMR = 0x30;
	LPC_TMR16B1->TCR = 1;

	// TMR32B0 is used for recording timing (audio/video)
	// 72MHz/8192Hz = 8789 with match at 4 (why not ?) and /2 = 1099
	LPC_TMR32B0->PR = 1099;
	LPC_TMR32B0->MCR = 2;			// Reset on match 0
	LPC_TMR32B0->MR0 = 3;			// Count 0~3 (/4)
	LPC_TMR32B0->EMR = (3<<4);
	LPC_TMR32B0->TCR = 0;

	// TMR32B1 is used for LCD backlight PWM
    LPC_TMR32B1->PR = 10;			// Prescaler
    LPC_TMR32B1->MCR = 0x0400;		// Reset on match register 3
    LPC_TMR32B1->MR3 = 7200;
    LPC_TMR32B1->MR0 = 7200;		// Inverted brightness (72-x)/72
	LPC_TMR32B1->EMR = 0x30;
	LPC_TMR32B1->PWMC = 1;
    LPC_TMR32B1->TCR = 1;

    LPC_SYSCON->PDRUNCFG &= ~(1<<4);		// Power to ADC

	spi_init();

	// DEBUG
    /*delay_us(50000);
	for (;;) {
		//f_mount(&FatFs, "", 1);
		//delay_us(50000);
	}*/

    lcd_init();
	FCLK_FAST();
    lcd_clear();

    lcd_fill(0, 0, 240, 32, 0b0110011100000000);
    lcd_paint(1, 1, logo, 0);
	lcd_hline(0, 32, 240, 0b0100111100000000);	// Useless gradient
	lcd_hline(0, 33, 240, 0b0011010101000000);
	lcd_hline(0, 34, 240, 0b0001001110000000);

    gbcam_reset();

	menu_view();
	fade_in();

	while (1) {
		loop_func();

		if (!check_timer) {
			check_timer = 100;		// 1s

			if (!sd_ok) {
				FCLK_SLOW();
				if ((fr = f_mount(&FatFs, "", 1)) == FR_OK) {
					sd_ok = 1;
					FCLK_FAST();
					lcd_paint(218, 0, icon_sdok, 1);
				} else {
					sd_ok = 0;
					FCLK_FAST();
					lcd_paint(218, 0, icon_sdnok, 1);
					str_buffer[0] = hexify((fr >> 4) & 15);
					str_buffer[1] = hexify(fr & 15);
					str_buffer[2] = 0;
					lcd_print(0, 0, str_buffer, COLOR_WHITE, 1);
				}
			}

			if (!gbcam_ok) {
			    if (!gbcam_detect()) {
			    	gbcam_ok = 1;
			    	lcd_paint(184, 0, icon_camok, 1);
			    } else {
			    	gbcam_ok = 0;
			    	lcd_paint(184, 0, icon_camnok, 1);
			    }
			}
		}
	}

    return 0 ;
}
