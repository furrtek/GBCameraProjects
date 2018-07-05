/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.2
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/
#include "main.h"
#include "font.h"
#include "colors.h"
#include "sdcard.h"
#include "capture.h"
#include "lcd.h"
#include "io.h"

void lcd_write_byte(const uint8_t v) {
	uint32_t dummy;

	LPC_SSP->CR0 = 0x0007;			// 8bit transfer

	while (!(LPC_SSP->SR & 0x02));
	LPC_SSP->DR = v;
	while ((LPC_SSP->SR & 0x10));
	dummy = LPC_SSP->DR;

	(void)dummy;
}

void lcd_write_word(const uint16_t v) {
	uint32_t dummy;

	LPC_SSP->CR0 = 0x000F;			// 16bit transfer

	while (!(LPC_SSP->SR & 0x02));
	LPC_SSP->DR = v;
	while ((LPC_SSP->SR & 0x10));
	dummy = LPC_SSP->DR;

	(void)dummy;
}

void lcd_writecommand(const uint8_t c) {
	LPC_GPIO0->DATA &= ~(1<<7 | 1<<10);	// D/C and CLK low
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	lcd_write_byte(c);

	LPC_GPIO0->DATA |= (1<<5);		// LCDCS high
}

void lcd_writedata(const uint8_t c) {
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<10);	// CLK low
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	lcd_write_byte(c);

	LPC_GPIO0->DATA |= (1<<5);		// LCDCS high
}

uint8_t hexify(uint8_t d) {
	if (d > 9)
		d += 7;
	return '0' + d;
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

void lcd_init() {
	uint32_t c, args;
	uint8_t cmd;

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
		while (args--)
			lcd_writedata(lcd_init_data[c++]);
	}
}

void lcd_locate(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h) {
	uint32_t x_end = x + w - 1;
	uint32_t y_end = y + h - 1;

	lcd_writecommand(0x2A); 		// Column address
	lcd_writedata(x >> 8);			// X start
	lcd_writedata(x);
	lcd_writedata(x_end >> 8);		// X end
	lcd_writedata(x_end);

	lcd_writecommand(0x2B);			// Row address
	lcd_writedata(y >> 8);			// Y start
	lcd_writedata(y);
	lcd_writedata(y_end >> 8);		// Y end
	lcd_writedata(y_end);
}

void lcd_print(uint32_t x, const uint32_t y, char * str, const uint16_t color, const uint32_t large) {
	uint32_t c, v, gradient;
	uint8_t data;
	const unsigned char * data_ptr;

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
				data = *data_ptr++;
				for (v = 0; v < 8; v++) {
					if (data & (1 << v))
						lcd_write_word(color);
					else
						lcd_write_word(COLOR_BLACK);
				}
			}
		} else {
			gradient = 0b1111111111111111;
			for (c = 0; c < 16; c++) {
				data = *data_ptr;
				for (v = 0; v < 8; v++) {
					if (data & (1 << v)) {
						lcd_write_word(color | gradient);
						lcd_write_word(color | gradient);
					} else {
						lcd_write_word(COLOR_BLACK);
						lcd_write_word(COLOR_BLACK);
					}
				}

				if (c & 1)
					data_ptr++;

				if (c < 7)
					gradient -= 0b0010000100000100;
			}
		}

		LPC_GPIO0->DATA |= (1<<5);	// LCDCS high

		x += (8 << large);
		str++;
	}
}

void lcd_print_time(const uint32_t x, const uint32_t y) {
	str_buffer[0] = 0x30 + (hours / 10);
	str_buffer[1] = 0x30 + (hours % 10);
	str_buffer[2] = ':';
	str_buffer[3] = 0x30 + (minutes / 10);
	str_buffer[4] = 0x30 + (minutes % 10);
	str_buffer[5] = ':';
	str_buffer[6] = 0x30 + (seconds / 10);
	str_buffer[7] = 0x30 + (seconds % 10);

	lcd_print(x, y, str_buffer, COLOR_WHITE, 0);
}

void lcd_fill_common(const uint32_t l, const uint16_t color) {
	uint32_t c;

	lcd_writecommand(0x2C); 		// Write to RAM
	LPC_GPIO0->DATA |= (1<<7);		// D/C high
	LPC_GPIO0->DATA &= ~(1<<5);		// LCDCS low

	for (c = 0; c < l; c++)
		lcd_write_word(color);

	LPC_GPIO0->DATA |= (1<<5);		// LCDCS high
}

void lcd_hline(const uint32_t x, const uint32_t y, const uint32_t l, const uint32_t color) {
	lcd_locate(x, y, l, 1);
	lcd_fill_common(l, color);
}

void lcd_vline(const uint32_t x, const uint32_t y, const uint32_t l, const uint32_t color) {
	lcd_locate(x, y, 1, l);
	lcd_fill_common(l, color);
}

void lcd_paint(uint32_t x, uint32_t y, const uint8_t * bitmap, const uint32_t large) {
	uint16_t palette[16];
	uint32_t c, w, h, line;
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
		color = *bitmap++;
		palette[c] = ((color & 0xE7) << 8) + (color & 0x18);
	}

	// Draw bitmap
	w >>= 1;
	line = 0;
	for (y = 0; y < (h << large); y++) {
		for (x = 0; x < w; x++) {
			color = bitmap[line + x];
			if (!large) {
				lcd_write_word(palette[color >> 4]);
				lcd_write_word(palette[color & 15]);
			} else {
				lcd_write_word(palette[color >> 4]);
				lcd_write_word(palette[color >> 4]);
				lcd_write_word(palette[color & 15]);
				lcd_write_word(palette[color & 15]);
			}
		}
		if ((y & 1) | !large)
			line += w;
	}

	LPC_GPIO0->DATA |= (1<<5);	// LCDCS high
}

void lcd_fill(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const uint16_t color) {
	lcd_locate(x, y, w, h);
	lcd_fill_common(w * h, color);
}

void lcd_preview(const uint32_t x, const uint32_t y) {
	uint32_t data_l, data_h, pixel;
	uint32_t xt, yt, xp, p;
	uint32_t addr, yto;

	luma_acc = 0;
	for (p = 0; p < 4; p++)
		histogram[p] = 0;

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

			histogram[pixel]++;
			luma_acc += pixel;

			// 2bpp to RGB
			lcd_write_word(lut_2bpp[pixel]);
		}
	}

	LPC_GPIO0->DATA |= (1<<5);		// LCDCS high
}

void lcd_clear() {
	lcd_fill(0, 35, 240, 320 - 35, COLOR_BLACK);
}

void fade_in() {
	while (backlight < 300) {	// 2200
		systick_wait(1);	// 10ms

		backlight += 100;

		LPC_TMR32B1->MR0 = 7200 - backlight;		// Inverted brightness
	}
}

void fade_out(void (*func)(void)) {
	while (backlight) {
		systick_wait(1);	// 10ms

		if (backlight >= 100)
			backlight -= 100;
		else
			backlight = 0;

		LPC_TMR32B1->MR0 = 7200 - backlight;		// Inverted brightness
	}

	func();
}
