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

	(void)dummy;
}

void lcd_spifast_word(uint16_t v) {
	uint8_t dummy;

	while (!(LPC_SSP->SR & 0x02));
	LPC_SSP->DR = (v >> 8);
	while ((LPC_SSP->SR & 0x10));
	dummy = LPC_SSP->DR;

	__asm("nop");

	while (!(LPC_SSP->SR & 0x02));
	LPC_SSP->DR = (v & 0xFF);
	while ((LPC_SSP->SR & 0x10));
	dummy = LPC_SSP->DR;

	(void)dummy;
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

const uint8_t lcd_init_data[] = {
	0xC0, 2, 0x26, 0x04,
	0xC1, 1, 0x04,
	0xC2, 1, 0xB2,
	0xC5, 2, 0x3E, 0x28,
	0x36, 1, 0x88,
	0xB1, 2, 0x00, 0x1B,
	0xB6, 4, 0x08, 0xE2, 0x27, 0x00,
	0xC7, 1, 0x86,
	0x3A, 1, 0x55,
	0xE0, 15, 0x1F, 0x1B, 0x18, 0x0B, 0x0F, 0x09, 0x46, 0xB5, 0x37, 0x0A, 0x0C, 0x07, 0x07, 0x05, 0x00,
	0xE1, 14, 0x24, 0x27, 0x04, 0x10, 0x06, 0x39, 0x74, 0x48, 0x05, 0x13, 0x38, 0x38, 0x3A, 0x1F,
	0x29, 0,
	0
};

void lcd_init(void) {
	uint16_t c;
	uint8_t cmd, a, args;

	LPC_GPIO0->DATA |= (1<<2);		// RST high
	delay_us(100);
	LPC_GPIO0->DATA &= ~(1<<2);		// RST low (min. 10us)
	delay_us(100);
	LPC_GPIO0->DATA |= (1<<2);		// RST high

	delay_us(60000);				// 120ms worst case
	delay_us(60000);

	lcd_writecommand(0x28);			// Display off

	lcd_writecommand(0x11);    		// Exit sleep mode
	lcd_writedata(0x00);
	delay_us(60000);				// 120ms worst case
	delay_us(60000);

	c = 0;
	while ((cmd = lcd_init_data[c++])) {
		lcd_writecommand(cmd);
		args = lcd_init_data[c++];
		if (args) {
			for (a = 0; a < args; a++)
				lcd_writedata(lcd_init_data[c++]);
		}
	}
}

void lcd_locate(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	uint16_t x_end = x + w - 1;
	uint16_t y_end = y + h - 1;

	lcd_writecommand(0x2A); 		// Column address
	lcd_writedata(x >> 8);			// X start
	lcd_writedata(x & 0xFF);
	lcd_writedata(x_end >> 8);		// X end
	lcd_writedata(x_end & 0xFF);

	lcd_writecommand(0x2B);			// Row address
	lcd_writedata(y >> 8);			// Y start
	lcd_writedata(y & 0xFF);
	lcd_writedata(y_end >> 8);		// Y end
	lcd_writedata(y_end & 0xFF);
}

void lcd_print(uint16_t x, uint16_t y, char * str, uint16_t color, uint8_t large) {
	uint8_t c, v, data;
	uint8_t * data_ptr;
	uint16_t gradient;

	while (*str) {
		if (!large)
			lcd_locate(x, y, 8, 8);
		else
			lcd_locate(x, y, 16, 16);

		lcd_writecommand(0x2C); 	// Write to RAM
		LPC_GPIO0->DATA |= (1<<7);	// D/C high
		LPC_GPIO0->DATA &= ~(1<<5);	// LCDCS low

		data_ptr = font + (((*str) - 32) << 3);
		if (!large) {
			for (c = 0; c < 8; c++) {
				data = *data_ptr;
				for (v = 0; v < 8; v++) {
					if (data & (1 << v)) {
						lcd_spifast_word(color);
					} else {
						lcd_spifast_word(COLOR_BLACK);
					}
				}
				data_ptr++;
			}
		} else {
			gradient = 0b1111111111111111;
			for (c = 0; c < 16; c++) {
				data = *data_ptr;
				for (v = 0; v < 8; v++) {
					if (data & (1 << v)) {
						lcd_spifast_word(color | gradient);
						lcd_spifast_word(color | gradient);
					} else {
						lcd_spifast_word(COLOR_BLACK);
						lcd_spifast_word(COLOR_BLACK);
					}
				}

				if (c & 1)
					data_ptr++;

				if (c < 7)
					gradient -= 0b0010000100000100;
			}
		}

		LPC_GPIO0->DATA |= (1<<5);	// LCDCS high

		if (!large)
			x += 8;
		else
			x += 16;

		str++;
	}
}

void lcd_hline(uint16_t x, uint16_t y, uint16_t l, uint16_t color) {
	uint16_t c;

	lcd_locate(x, y, l, 1);
	lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	for (c = 0; c < l; c++) {
		//xmit_spi(color >> 8);
		//xmit_spi(color & 0xFF);
		//lcd_spifast(color >> 8);
		//lcd_spifast(color & 0xFF);
		lcd_spifast_word(color);
	}

	LPC_GPIO0->DATA |= (1<<5);	// LCDCS high
}

void lcd_vline(uint16_t x, uint16_t y, uint16_t l, uint16_t color) {
	uint16_t c;

	lcd_locate(x, y, 1, l);
	lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	for (c = 0; c < l; c++) {
		//lcd_spifast(color >> 8);
		//lcd_spifast(color & 0xFF);
		lcd_spifast_word(color);
	}

	LPC_GPIO0->DATA |= (1<<5);	// LCDCS high
}

void lcd_paint(uint16_t x, uint16_t y, const uint8_t * bitmap, uint8_t large) {
	uint16_t palette[16];
	uint16_t c, w, h, line;
	uint8_t color;

	w = bitmap[0];
	h = bitmap[1];

	lcd_locate(x, y, w << large, h << large);
	lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	bitmap += 2;	// Skip width and height

	// Load and convert palette
	for (c = 0; c < 16; c++) {
		color = bitmap[c];
		palette[c] = ((color & 0xE7) << 8) + (color & 0x18);
	}

	bitmap += 16;	// Skip palette

	// Draw bitmap
	line = 0;
	for (y = 0; y < (h << large); y++) {
		for (x = 0; x < (w >> 1); x++) {
			color = bitmap[line + x];
			//if (color) {
				if (!large) {
					lcd_spifast_word(palette[color >> 4]);
					lcd_spifast_word(palette[color & 15]);
				} else {
					lcd_spifast_word(palette[color >> 4]);
					lcd_spifast_word(palette[color >> 4]);
					lcd_spifast_word(palette[color & 15]);
					lcd_spifast_word(palette[color & 15]);
				}
			//}
		}
		if ((y & 1) | !large)
			line += (w >> 1);
	}

	LPC_GPIO0->DATA |= (1<<5);	// LCDCS high
}

void lcd_clear(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	uint32_t size = w * h;
	uint32_t c;

	lcd_locate(x, y, w, h);
	lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	// Clear
	for (c = 0; c < size; c++)
		lcd_spifast_word(color);

	LPC_GPIO0->DATA |= (1<<5);		// LCDCS high
}

void lcd_preview(uint16_t x, uint16_t y) {
	uint8_t data_l, data_h, pixel;
	uint8_t xt, yt, xp;
	uint16_t addr, yto;

	lcd_locate(x, y, 128, 112);
	lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	for (yt = 0; yt < 112; yt++) {
		yto = (yt & 7) + ((yt & 0x78) * 16);
		for (xt = 0; xt < 128; xt++) {
			// See draft, complete mess: array of tiles to linear LCD format (x inverted)
			// (xt & 0x78): tile # x
			// + (yt & 7): pixel y in tile
			// + (yt & 0x78) * 16: tile # y
			// * 2: 2 bytes per pixel

			addr = ((xt & 0x78) + yto) * 2;

			data_l = picture_buffer[addr];
			data_h = picture_buffer[addr + 1];

			xp = 7 - (xt & 7);			// Invert x

			// Planar to linear
			// xp = 0				xp = 1				...
			// 76543210 76543210	76543210 76543210
			// 6543210- 76543210	7654321- -7654321
			// ------0- -------0	------1- -------1
			pixel = (((data_h >> xp) << 1) & 2) | ((data_l >> xp) & 1);

			// GameBoy 2bpp to RGB
			// TODO: Use LUT ?
			if (pixel == 3) {
				lcd_spifast_word(0xFFFF);		// 11111111 11111111
			} else if (pixel == 2) {
				lcd_spifast_word(0xBDF7);		// 10111101 11110111
			} else if (pixel == 1) {
				lcd_spifast_word(0x39E7);		// 00111001 11100111
			} else {
				lcd_spifast_word(0x18E3);		// 00011000 11100011
			}
		}
	}

	LPC_GPIO0->DATA |= (1<<5);		// LCDCS high
}

void fade_in() {
	while (backlight < 2200) {
		systick = 0;
		while (systick < 1);	// 10ms

		backlight += 100;

		LPC_TMR32B1->MR0 = 7200 - backlight;		// Inverted brightness
	}
}

void fade_out(void (*func)(void)) {
	while (backlight) {
		systick = 0;
		while (systick < 1);	// 10ms

		if (backlight >= 100)
			backlight -= 100;
		else
			backlight = 0;

		LPC_TMR32B1->MR0 = 7200 - backlight;		// Inverted brightness
	}

	func();
}
