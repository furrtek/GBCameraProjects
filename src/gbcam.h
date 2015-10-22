/*
 * gbcam.h
 *
 *  Created on: 19 sept. 2015
 *      Author: furrtek
 */

#ifndef GBCAM_H_
#define GBCAM_H_

void gbcam_address(uint16_t address);
uint8_t gbcam_get(uint16_t address);
void gbcam_set(uint16_t address, uint8_t value);
uint8_t gbcam_detect(void);
void gbcam_reset(void);
void gbcam_setmatrix(uint8_t * matrix);
void gbcam_setexposure(uint16_t exposure);

#endif /* GBCAM_H_ */