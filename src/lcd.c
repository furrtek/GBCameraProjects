/*
 * lcd.c
 *
 *  Created on: 12 oct. 2015
 *      Author: furrtek
 */

#include "main.h"
#include "font.h"
#include "sdcard.h"
#include "io.h"

void lcd_spifast(uint8_t v) {
	uint8_t dummy;

	while (!(LPC_SSP->SR & 0x02));
	LPC_SSP->DR = v;
	while ((LPC_SSP->SR & 0x10));
	dummy = LPC_SSP->DR;
}

void lcd_writecommand(uint8_t c) {
	LPC_GPIO0->DATA &= ~(1<<7);		// D/C low
	LPC_GPIO0->DATA &= ~(1<<10);	// CLK low
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	xmit_spi(c);

	LPC_GPIO0->DATA |= (1<<5);		// LCDCS high
}

void lcd_writedata(uint8_t c) {
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<10);	// CLK low
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	xmit_spi(c);

	LPC_GPIO0->DATA |= (1<<5);		// LCDCS high
}

void lcd_init(void) {
	delay_us(60000);				// Useless ?

	LPC_GPIO0->DATA |= (1<<2);		// RST high
	delay_us(500);
	LPC_GPIO0->DATA &= ~(1<<2);		// RST low
	delay_us(500);
	LPC_GPIO0->DATA |= (1<<2);		// RST high
	delay_us(500);

	delay_us(60000);				// ILI9340 datasheet says 120ms after reset ?
	delay_us(60000);

	lcd_writecommand(0x28);			// Display off

	lcd_writecommand(0x11);    		// Exit sleep mode
	lcd_writedata(0x00);
	delay_us(5000);

	lcd_writecommand(0xC0);    		// Voltage control
	lcd_writedata(0x26);
	lcd_writedata(0x04);
	lcd_writecommand(0xC1);    		// Voltage control
	lcd_writedata(0x04);
	lcd_writecommand(0xC2);    		// Step-up control (power/display quality)
	lcd_writedata(0xD2);
	lcd_writecommand(0xC5);    		// VCOM control
	lcd_writedata(0x34);
	lcd_writedata(0x40);
	lcd_writecommand(0x36);			// Memory access
	lcd_writedata(0x88);
	lcd_writecommand(0xB1);			// Frame rate (70Hz)
	lcd_writedata(0x00);
	lcd_writedata(0x1B);
	lcd_writecommand(0xB6);    		// TFT panel driver configuration
	lcd_writedata(0x0A);
	lcd_writedata(0xE2);
	lcd_writedata(0x27);
	lcd_writedata(0x00);
	lcd_writecommand(0xC7);    		// VCOM control
	lcd_writedata(0xC0);
	lcd_writecommand(0x3A);			// Pixel format (16bit)
	lcd_writedata(0x55);
	lcd_writecommand(0xE0);    		// Positive gamma
	lcd_writedata(0x1F);
	lcd_writedata(0x1B);
	lcd_writedata(0x18);
	lcd_writedata(0x0B);
	lcd_writedata(0x0F);
	lcd_writedata(0x09);
	lcd_writedata(0x46);
	lcd_writedata(0xB5);
	lcd_writedata(0x37);
	lcd_writedata(0x0A);
	lcd_writedata(0x0C);
	lcd_writedata(0x07);
	lcd_writedata(0x07);
	lcd_writedata(0x05);
	lcd_writedata(0x00);
	lcd_writecommand(0xE1);    		// Negative gamma
	lcd_writedata(0x24);
	lcd_writedata(0x27);
	lcd_writedata(0x04);
	lcd_writedata(0x10);
	lcd_writedata(0x06);
	lcd_writedata(0x39);
	lcd_writedata(0x74);
	lcd_writedata(0x48);
	lcd_writedata(0x05);
	lcd_writedata(0x13);
	lcd_writedata(0x38);
	lcd_writedata(0x38);
	lcd_writedata(0x3A);
	lcd_writedata(0x1F);

	//delay_us(20000);

	lcd_writecommand(0x29);    		// Display on
}

void lcd_locate(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	lcd_writecommand(0x2A); 		// Column address
	lcd_writedata(x >> 8);			// X start
	lcd_writedata(x & 0xFF);
	lcd_writedata((x+w-1) >> 8);	// X end
	lcd_writedata((x+w-1) & 0xFF);
	lcd_writecommand(0x2B);			// Row address
	lcd_writedata(y >> 8);			// Y start
	lcd_writedata(y & 0xFF);
	lcd_writedata((y+h-1) >> 8);	// Y end
	lcd_writedata((y+h-1) & 0xFF);
}

void lcd_print(uint16_t x, uint16_t y, const char * str, uint16_t color) {
	uint8_t c, v, data;

	while (*str) {
		lcd_locate(x, y, 8, 8);
		lcd_writecommand(0x2C); 	// Write to RAM
		LPC_GPIO0->DATA |= (1<<7);	// D/C high
		LPC_GPIO0->DATA &= ~(1<<5);	// LCDCS low
		for (c=0; c<8; c++) {
			data = font[((*str)-32)*8+c];
			for (v=0; v<8; v++) {
				if (data & (1 << v)) {
					xmit_spi(color >> 8);
					xmit_spi(color & 0xFF);
				} else {
					xmit_spi(0x00);
					xmit_spi(0x00);
				}
			}
		}
		LPC_GPIO0->DATA |= (1<<5);	// LCDCS high
		x += 8;
		str++;
	}
}

void lcd_hline(uint16_t x, uint16_t y, uint16_t l, uint16_t color) {
	uint16_t c;

	lcd_locate(x, y, l, 1);
	lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low
	for (c=0; c<l; c++) {
		xmit_spi(color >> 8);
		xmit_spi(color & 0xFF);
	}
}

void lcd_vline(uint16_t x, uint16_t y, uint16_t l, uint16_t color) {
	uint16_t c;

	lcd_locate(x, y, 1, l);
	lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low
	for (c=0; c<l; c++) {
		lcd_spifast(color >> 8);
		lcd_spifast(color & 0xFF);
	}
}

void lcd_paint(uint16_t x, uint16_t y, const uint8_t * icon) {
	uint16_t c, cc, color;
	uint16_t pixs;

	pixs = (icon[0] * icon[1]) + 2;

	lcd_locate(x, y, icon[0], icon[1]);
	lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low
	for (c=2; c<pixs; c++) {
		cc = (uint16_t)icon[c];
		//00rrggbb to rrrrrggggggbbbbb
		color = ((cc & 0x30)<<10) + ((cc & 0x0C)<<7) + ((cc & 0x03)<<3);
		lcd_spifast(color >> 8);
		lcd_spifast(color & 0xFF);
	}
}

void lcd_preview(uint16_t x, uint16_t y) {
	uint8_t data_l, data_h, pixel;
	uint8_t xt, yt, xp;
	uint16_t addr, yto;

	lcd_locate(x, y, 128, 112);
	lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	//lcd_locate(x, y, 192, 168);

	for (yt=0; yt<112; yt++) {
		yto = (yt & 7) + ((yt & 0x78)*16);
		for (xt=0; xt<128; xt++) {
			// See draft, complete mess: array of tiles to linear LCD format (x inverted)
			// (xt & 0x78): tile # x
			// + (yt & 7): pixel y in tile
			// + (yt & 0x78) * 16: tile # y
			// * 2: 2 bytes per pixel

			addr = ((xt & 0x78) + yto) * 2;

			data_l = picture_buffer[addr];
			data_h = picture_buffer[addr+1];

			xp = (7-(xt & 7));			// Invert x

			// Planar to linear
			pixel = (((data_h>>xp)<<1)&2) | ((data_l>>xp)&1);

			// GameBoy 2bpp to RGB
			if (pixel == 3) {
				lcd_spifast(0xFF);		// 11111111 11111111
				lcd_spifast(0xFF);
			} else if (pixel == 2) {
				lcd_spifast(0xBD);		// 10111101 11110111
				lcd_spifast(0xF7);
			} else if (pixel == 1) {
				lcd_spifast(0x39);		// 00111001 11100111
				lcd_spifast(0xE7);
			} else {
				lcd_spifast(0x18);		// 00011000 11100011
				lcd_spifast(0xE3);
			}
		}
	}

	LPC_GPIO0->DATA |= (1<<5);		// LCDCS high
}
