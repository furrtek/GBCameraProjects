/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.2
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camera camcorder firmware
===============================================================================
*/

#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "views.h"
#include "gbcam.h"
#include "sdcard.h"
#include "diskio.h"
#include "ff.h"
#include "io.h"
#include "lcd.h"
#include "icons.h"

// TODO: Playback of recorded files ?
// TODO: Brightness/contrast adjustment
// TODO: File naming ?
// TODO: Gain setting

// Dithering layout taken from real GB Cam, 0000yyxx (could use nibbles instead)
//     0   1   2   3
//   ---------------
// 0 | A   M   D   P
// 1 | I   E   L   H
// 2 | C   O   B   N
// 3 | K   G   J   F
const uint8_t matrix_layout[16] = { 0x00, 0x0A, 0x08, 0x02,
									0x05, 0x0F, 0x0D, 0x07,
									0x04, 0x0E, 0x0C, 0x06,
									0x01, 0x0B, 0x09, 0x03 };

void SysTick_Handler(void) {
	if (systick != 255)
		systick++;

	if (slots_timer)
		slots_timer--;

	rec_timer++;
}

void ADC_IRQHandler(void) {
	volatile uint32_t adstat;

	adstat = LPC_ADC->STAT;
	(void)adstat;

	audio_fifo[audio_fifo_put][audio_fifo_ptr] = (LPC_ADC->DR0 >> 8) & 0xFF;

	if (audio_fifo_ptr == 511) {
		audio_fifo_ptr = 0;

		if (write_frame_request)
			skipped++;

		write_frame_request = 1;	// This sets the framerate (8192/512 = 16 fps)

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

    init_io();

	// TMR16B0 is used for delays (1us tick @ 72MHz, do NOT modify !)
	LPC_TMR16B0->PR = 72;
	LPC_TMR16B0->TCR = 1;

	// TMR16B1 is used for Phi generation (GB CPU frequency / 4)
	LPC_TMR16B1->PR = 0;
    LPC_TMR16B1->MCR = 0x0400;		// Reset on match register 3
    LPC_TMR16B1->MR0 = 17;			// Not used ?
	LPC_TMR16B1->MR3 = 34;			// ~1048576Hz (can go faster if needed) !!!!!!!
	LPC_TMR16B1->EMR = 0x30;
	LPC_TMR16B1->TCR = 1;

	backlight = 0;

	// TMR32B1 is used for LCD backlight PWM
    LPC_TMR32B1->PR = 10;			// Prescaler
    LPC_TMR32B1->MCR = 0x0400;		// Reset on match register 3
    LPC_TMR32B1->MR3 = 7200;
    LPC_TMR32B1->MR0 = 7200 - backlight;	// Inverted brightness (72-50)/72
	LPC_TMR32B1->EMR = 0x30;
	LPC_TMR32B1->PWMC = 1;
    LPC_TMR32B1->TCR = 1;

    LPC_SYSCON->PDRUNCFG &= ~(1<<4);		// Power to ADC

    delay_us(1000);

    // ADC setup
    LPC_ADC->CR = (16<<8) | (4<<24) | 1;	// 72/16=4.5MHz, start conversion on rising edge of CT32B0_MAT0
    NVIC->ISER[1] = (1<<17);				// Enable ADC interrupt
    LPC_ADC->INTEN = 1;						// Interrupt on conversion done

	spi_init();
    lcd_init();

    FCLK_LCD();
    lcd_clear();

    //delay_us(10000);				// Software fixing hardware: 3.3V drops hard after backlight is switched on, dirty...

    lcd_fill(0, 0, 240, 32, 0b0110011100000000);
    lcd_paint(1, 1, logo, 0);
	lcd_hline(0, 32, 240, 0b0100111100000000);
	lcd_hline(0, 33, 240, 0b0011010101000000);
	lcd_hline(0, 34, 240, 0b0001001110000000);

    delay_us(50000);

    gbcam_reset();

    slots_timer = 100;
    sd_ok = 0;
    gbcam_ok = 0;

	menu_view();
	fade_in();

	while (1) {
		loop_func();

		// Init SD FAT
		if (!slots_timer) {
			slots_timer = 100;

			if (!sd_ok) {
				FCLK_SLOW();
				if (f_mount(&FatFs, "", 1) == FR_OK) {
					sd_ok = 1;
					FCLK_LCD();
					lcd_paint(218, 0, icon_sdok, 1);
				} else {
					sd_ok = 0;
					FCLK_LCD();
					lcd_paint(218, 0, icon_sdnok, 1);
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
