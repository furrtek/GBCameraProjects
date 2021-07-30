/*
 * io.h
 *
 *  Created on: 12 oct. 2015
 *      Author: furrtek
 */

#ifndef IO_H_
#define IO_H_

void delay_us(uint16_t delay);
void read_inputs();
void init_io();

uint8_t inputs_prev, inputs_current, inputs_active;

#endif /* IO_H_ */
