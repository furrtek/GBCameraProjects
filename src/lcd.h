/*
 * lcd.h
 *
 *  Created on: 12 oct. 2015
 *      Author: furrtek
 */

#ifndef LCD_H_
#define LCD_H_

void lcd_spifast(uint8_t v);
void lcd_writecommand(uint8_t c);
void lcd_writedata(uint8_t c);
void lcd_init(void);
void lcd_locate(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void lcd_print(uint16_t x, uint16_t y, const char * str, uint16_t color);
void lcd_preview(uint16_t x, uint16_t y);
void lcd_hline(uint16_t x, uint16_t y, uint16_t l, uint16_t color);
void lcd_vline(uint16_t x, uint16_t y, uint16_t l, uint16_t color);
void lcd_paint(uint16_t x, uint16_t y, const uint8_t * icon);

#endif /* LCD_H_ */
