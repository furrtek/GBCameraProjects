/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.3
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/

#ifndef GBCAM_H_
#define GBCAM_H_

uint8_t gbcam_wait_busy();
uint8_t gbcam_wait_idle();
void cart_set_address(const uint16_t address);
uint8_t cart_get_rom(const uint16_t address);
uint8_t cart_get_ram(const uint16_t address);
void cart_put(const uint16_t address, const uint8_t value);
uint8_t gbcam_detect(void);
void gbcam_reset(void);
void gbcam_setcontrast(const uint8_t slope, const uint8_t offset);
void gbcam_setmatrix();
void gbcam_setexposure(const uint16_t exposure);

#endif /* GBCAM_H_ */
