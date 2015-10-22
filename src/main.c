/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.1
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camera camcorder firmware
===============================================================================
*/

#include "LPC13xx.h"
#include "main.h"
#include "gbcam.h"
#include "sdcard.h"
#include "integer.h"
#include "diskio.h"
#include "ff.h"
#include "io.h"
#include "lcd.h"
#include "icons.h"
#include <string.h>
#include <stdlib.h>

// TODO: Playback of recorded files
// TODO: Brightness/contrast adjustment
// TODO: File naming ?
// TODO: Gain setting
// TODO: More pretty icons, logo ?

#define	MAX_EXPOSURE 1000		// ((acqtime/953.6)-32446)/16

uint16_t exposure;
uint8_t gbcam_matrix[48];		// Dithering/ADC matrix
uint8_t qlevels[4] = {	0x8F,	// Initial matrix computation values (medium-high contrast)
						0x98,
						0xA6,
						0xB9};

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

// Audio ring FIFO
static volatile uint8_t afifo[6][512];
static volatile uint8_t afifo_ready = 0;
static volatile uint8_t afifo_get = 0;
static volatile uint8_t afifo_put = 0;
static volatile uint16_t afifo_idx = 0;
static volatile uint8_t write_frame = 0;

// Timing
static volatile uint8_t debounce = 0;
static volatile uint8_t ref_frame_cnt;
static volatile uint8_t skipped = 0;

// Flags
static volatile uint8_t can_record = 0;
static volatile uint8_t recording = 0;

static FRESULT fr;

void ADC_IRQHandler(void) {
	volatile uint32_t adstat;

	adstat = LPC_ADC->STAT;

	afifo[afifo_put][afifo_idx] = (LPC_ADC->DR0>>8) & 0xFF;

	if (afifo_idx < 511) {
		afifo_idx++;
	} else {
		afifo_idx = 0;
		ref_frame_cnt++;
		if (write_frame) skipped++;
		write_frame = 1;
		afifo_ready++;			// This should NEVER go over 5 (SD card too slow or stalled ?)
		if (afifo_put == 5)
			afifo_put = 0;
		else
			afifo_put++;
	}

    NVIC->ICPR[1] = (1<<17);	// Ack interrupt
}

void PIOINT2_IRQHandler(void) {
	if (!debounce){
		debounce = 10;
		if (can_record) {
			if (recording == 0) recording = 1;	// IDLE to REC_REQUEST
			if (recording == 2) recording = 3;	// RECORDING to REC_STOP
		}
	}

	LPC_GPIO2->IC = (1<<11);	// Ack interrupt
}

uint8_t hexify(uint8_t d) {
	if (d > 9) d += 7;
	return d;
}

// File format:
// "GBCC"
// Vx + 3584 bytes image in GB format (x = frames skipped since last one)
// Ax + x*512 bytes of 8192Hz 8-bit unsigned audio (x = number of audio frames to follow)

void newfile(void) {
	char filename[13] = "GBCAM000.BIN";
	const char fhead[4] = {'G','B','C','C'};
	uint16_t br;

	FCLK_FAST();
	// Find filename and create file
	fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_NEW);
    while (fr == FR_EXIST) {
    	if (filename[7] < '9') {
    		filename[7]++;
    	} else {
    		filename[7] = '0';
        	if (filename[6] < '9') {
        		filename[6]++;
        	} else {
        		filename[6] = '0';
            	if (filename[5] < '9') {
            		filename[5]++;
            	} else {
            		// TODO: No file available for creation (all 999 already exist)
            	}
        	}
    	}
    	fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_NEW);
    }

    // Write header
    fr = f_write(&fil, &fhead, 4, &br);
    FCLK_LCD();
}

int main(void) {
	uint16_t c, v, a, text_color;
	uint8_t lev, m, b;
	uint8_t data, expo_status, prev_expo_status = EXPO_INRANGE;
	uint16_t mean;
	int32_t exposure_diff;
	uint16_t br;

	char sn_marker[2] = {'A', 0};			// Audio (# of blocks)
	char im_marker[2] = {'V', 0};			// Video (# of skipped frames since last one)

	uint8_t seconds, minutes, hours;

	char timestr[9] = "00:00:00";

	// Should already be set correctly in SystemInit()...
    LPC_SYSCON->SYSAHBCLKCTRL = (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<6) |
    							(1<<7) | (1<<8) | (1<<9) | (1<<10) | (1<<11) | (1<<13) | (1<<16);

	// TMR16B0 is used for delays (1us tick, do NOT modify !)
	LPC_TMR16B0->PR = 72;
	LPC_TMR16B0->TCR = 1;

    LPC_SYSCON->PDRUNCFG &= ~(1<<4);		// Power to ADC

    delay_us(20000);						// Required ! (at least for simulation)

    init_io();

    delay_us(1000);

    // ADC setup
    LPC_ADC->CR = (16<<8) | (4<<24) | 1;	// 72/16=4.5MHz, start conversison on rising edge of CT32B0_MAT0
    NVIC->ISER[1] = (1<<22) | (1<<17);		// Enable ADC and GPIO2 interrupts
    LPC_ADC->INTEN = 1;						// Interrupt on conversion done

	spi_init();

    lcd_init();

    FCLK_LCD();
    // Clear screen to black
    lcd_locate(0, 0, 240, 320);
    lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low
	for (c=0; c<38400; c++) {
		lcd_spifast(0x00);
		lcd_spifast(0x00);
	}
	for (c=0; c<38400; c++) {
		lcd_spifast(0x00);
		lcd_spifast(0x00);
	}
	LPC_GPIO0->DATA |= (1<<5);		// LCDCS high

	// TMR16B1 is used for Phi generation (GB CPU frequency / 4)
	LPC_TMR16B1->PR = 0;
    LPC_TMR16B1->MCR = 0x0400;		// Reset on match register 3
    LPC_TMR16B1->MR0 = 17;			// Not used ?
	LPC_TMR16B1->MR3 = 34;			// ~1048576Hz (can go faster if needed)
	LPC_TMR16B1->EMR = 0x30;
	LPC_TMR16B1->TCR = 1;

	// TMR32B1 is used for LCD backlight PWM
    LPC_TMR32B1->PR = 10;			// Prescaler
    LPC_TMR32B1->MCR = 0x0400;		// Reset on match register 3
    LPC_TMR32B1->MR3 = 7200;
    LPC_TMR32B1->MR0 = 5000;		// Inverted brightness (72-50)/72
	LPC_TMR32B1->EMR = 0x30;
	LPC_TMR32B1->PWMC = 1;
    LPC_TMR32B1->TCR = 1;

    delay_us(10000);				// Software fixing hardware: 3.3V drops hard after backlight is switched on, dirty...

	lcd_print(68, 16, "GBCamcorder !", COLOR_WHITE);
	lcd_print(76, 28, "FW 10122015", COLOR_GREY);
	lcd_print(24, 304, "Furrtek Engineering 2015", COLOR_WHITE);

	// Grey backdrop around preview
    lcd_locate(0, 48, 240, 144);
    lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low
	for (c=0; c<(240*144); c++) {
		lcd_spifast(COLOR_GREY >> 8);
		lcd_spifast(COLOR_GREY & 0xFF);
	}

	// Draw reference lines
	lcd_hline(56-9, 64+56, 8, COLOR_WHITE);
	lcd_hline(56+129, 64+56, 8, COLOR_WHITE);
	lcd_vline(56+64, 64-9, 8, COLOR_WHITE);
	lcd_vline(56+64, 64+113, 8, COLOR_WHITE);

	// Init SD FAT
	FCLK_SLOW();
    if (f_mount(&FatFs, "", 1) == FR_OK) {
    	can_record = 1;
    	FCLK_LCD();
    	lcd_paint(16, 10, icon_sdok);
    } else {
    	can_record = 0;
    	FCLK_LCD();
    	lcd_paint(16, 10, icon_sdnok);
    }

    delay_us(50000);

    gbcam_reset();

    if (gbcam_detect())
    	lcd_paint(40, 10, icon_camok);
    else
    	lcd_paint(40, 10, icon_camnok);

    // Compute GB Cam dithering matrix values (see GB Cam ROM)
    for (lev=0; lev<3; lev++) {
    	v = qlevels[lev];
    	a = (qlevels[lev+1] - v) << 4;
    	v = v << 8;
		for (m=0; m<16; m++) {
			gbcam_matrix[lev+(matrix_layout[m]*3)] = (v >> 8);
			v += a;
		}
    }

    gbcam_setmatrix(gbcam_matrix);	// Upload matrix to GB Cam

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
    for (c=0; c<0x1000; c++)
    	gbcam_set(0xA000 + c, 0x00);

	gbcam_set(0x4000, 0x10);		// ASIC registers
	delay_us(2);
	gbcam_set(0xA001, 0x08);		// Sensor gain
	gbcam_set(0xA004, 0x46);		// Edge enhance 200%
	gbcam_set(0xA005, 0x9F);		// Dark level calibration

	// Main loop
	while (1) {

		if (debounce) debounce--;	// Dirty

		mean = 0;

		// Read scratchpad
		gbcam_set(0x4000, 0x00);	// SRAM bank 0
		delay_us(2);
		for (c=0; c<FRAME_SIZE; c++) {
			data = gbcam_get(0xA100 + c) ^ 0xFF;
			picture_buffer[c] = data;
			// Sum up pixels
			mean += ((data>>6) & 3)+((data>>4) & 3)+((data>>2) & 3)+(data & 3);
		}

		// Ask ASIC for capture
		gbcam_set(0x4000, 0x10);	// ASIC registers
		delay_us(2);
		gbcam_set(0xA000, 0x03);

		// Wait for capture to start
		while(!(gbcam_get(0xA000) & 1));	// Could freeze !

		// Auto-exposure (progressive): 128*112*3 (all pixels white) /2 = 21504
		exposure_diff = ((21504 - mean) >> 7);

		// Cap and alert
		if ((int32_t)exposure + exposure_diff > MAX_EXPOSURE) {
			expo_status = EXPO_DARK;
			exposure = MAX_EXPOSURE;
		} else if ((int32_t)exposure + exposure_diff < 40) {
			expo_status = EXPO_BRIGHT;
			exposure = 40;
		} else {
			expo_status = EXPO_INRANGE;
			exposure += exposure_diff;
		}

		// Exposure value is yellow if there's an abrupt lighting change
		if ((exposure_diff > 60) || (exposure_diff < -60))
			text_color = COLOR_YELLOW;
		else
			text_color = COLOR_GREEN;

		// Display exposure value
		rbf[0] = 0x30 + hexify((exposure >> 12) & 0xF);
		rbf[1] = 0x30 + hexify((exposure >> 8) & 0xF);
		rbf[2] = 0x30 + hexify((exposure >> 4) & 0xF);
		rbf[3] = 0x30 + hexify(exposure & 0xF);
		rbf[4] = 0;
		lcd_print(56, 194, rbf, text_color);

		if (recording == 1) {
			// Recording start request
			newfile();
			lcd_paint(204, 12, icon_rec);	// Display "REC" icon
			LPC_GPIO1->DATA &= ~(1<<5);		// Red LED on
			ref_frame_cnt = 0;
			seconds = 0;
			minutes = 0;
			hours = 0;
			afifo_ready = 0;
			recording = 2;
			LPC_TMR32B0->TCR = 1;
		}
		if (recording == 2) {
			// Recording
			if (write_frame) {
				write_frame = 0;
				FCLK_FAST();
				im_marker[1] = skipped;
				// Write image marker
				f_write(&fil, &im_marker, 2, &br);
				// Write image data
				for (b=0; b<7; b++)
					f_write(&fil, &picture_buffer[512*b], 512, &br);
				skipped = 0;
				if (afifo_ready) {
					sn_marker[1] = afifo_ready;
					afifo_ready = 0;
					// Write audio marker
					f_write(&fil, &sn_marker, 2, &br);
					// Write audio data
					do {
						f_write(&fil, &afifo[afifo_get], 512, &br);
						if (afifo_get == 5)
							afifo_get = 0;
						else
							afifo_get++;
						sn_marker[1]--;
					} while (sn_marker[1]);
				}
				FCLK_LCD();
			}
			// Update recording time every second
			if (ref_frame_cnt >= 16) {
				ref_frame_cnt -= 16;
				if (seconds < 59) {
					seconds++;
				} else {
					seconds = 0;
					if (minutes < 59) {
						minutes++;
					} else {
						minutes = 0;
						if (hours < 98) {
							hours++;
						} else {
							// Recording time exceeded, forgot to turn it off ?
						}
					}
				}

				timestr[0] = 0x30 + (hours / 10);
				timestr[1] = 0x30 + (hours % 10);

				timestr[3] = 0x30 + (minutes / 10);
				timestr[4] = 0x30 + (minutes % 10);

				timestr[6] = 0x30 + (seconds / 10);
				timestr[7] = 0x30 + (seconds % 10);

				lcd_print(56, 220, timestr, COLOR_WHITE);
			}
		}
		if (recording == 3) {
			// Recording stop request
			// Todo: erase REC icon
			LPC_TMR32B0->TCR = 0;
			LPC_GPIO1->DATA |= (1<<5);		// Red LED off
			recording = 0;
			f_close(&fil);
		}

		// Exposure alerts
		if (expo_status != prev_expo_status) {
			if (expo_status == EXPO_INRANGE) lcd_print(56, 204, "In range  ", COLOR_GREEN);
			if (expo_status == EXPO_DARK) lcd_print(56, 204, 	"Too dark  ", COLOR_RED);
			if (expo_status == EXPO_BRIGHT) lcd_print(56, 204, 	"Too bright", COLOR_RED);
		}

		prev_expo_status = expo_status;

		//Show last captured image
		lcd_preview(56, 64);

		// Wait for capture to end
		while(gbcam_get(0xA000) & 1);		// Could freeze !
		LPC_GPIO1->DATA |= (1<<8);

		gbcam_setexposure(exposure);
	}

    return 0 ;
}
