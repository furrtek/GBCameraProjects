/*
 * gbcam.h
 *
 *  Created on: 19 sept. 2015
 *      Author: furrtek
 */

#ifndef GBCAM_H_
#define GBCAM_H_

uint8_t gbcam_wait_busy();
uint8_t gbcam_wait_idle();
void gbcam_address(const uint16_t address);
uint8_t gbcam_get_ram(const uint16_t address);
void gbcam_put(const uint16_t address, const uint8_t value);
uint8_t gbcam_detect(void);
void gbcam_reset(void);
void gbcam_setcontrast(const uint8_t slope, const uint8_t offset);
void gbcam_setmatrix();
void gbcam_setexposure(const uint16_t exposure);

#endif /* GBCAM_H_ */
