/*
 * lcd.h
 *
 *  Created on: 12 oct. 2015
 *      Author: furrtek
 */

#ifndef LCD_H_
#define LCD_H_

#define COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

uint8_t hexify(uint8_t d);
void lcd_init();
void lcd_fill(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const uint16_t color);
void lcd_clear();
void lcd_locate(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h);
void lcd_print(uint32_t x, const uint32_t y, char * str, const uint16_t color, const uint32_t large);
void lcd_print_time(const uint32_t x, const uint32_t y);
void lcd_preview(const uint32_t x, const uint32_t y);
void lcd_hline(const uint32_t x, const uint32_t y, const uint32_t l, const uint32_t color);
void lcd_vline(const uint32_t x, const uint32_t y, const uint32_t l, const uint32_t color);
void lcd_paint(uint32_t x, uint32_t y, const uint8_t * icon, const uint32_t large);
void fade_in();
void fade_out(void (*func)(void));

#endif /* LCD_H_ */
