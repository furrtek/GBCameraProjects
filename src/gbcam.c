/*
 * gbcam.c
 *
 *  Created on: 07 oct. 2015
 *      Author: furrtek
 */

#include "LPC13xx.h"
#include "io.h"

void gbcam_address(uint16_t address) {
	LPC_GPIO1->DATA &= ~((1<<2) | (1<<3));	// ALEx low
	LPC_GPIO2->DIR |= 0xFF;					// Output

	// Set address low
	LPC_GPIO2->DATA = (LPC_GPIO2->DATA & 0xF00) | (address & 0xFF);

	LPC_GPIO1->DATA |= (1<<2);				// ALEL high
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	LPC_GPIO1->DATA &= ~(1<<2);				// ALEL low

	// Set address high
	LPC_GPIO2->DATA = (LPC_GPIO2->DATA & 0xF00) | (address >> 8);

	LPC_GPIO1->DATA |= (1<<3);				// ALEH high
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	LPC_GPIO1->DATA &= ~(1<<3);				// ALEH low
}

uint8_t gbcam_get(uint16_t address) {
	uint8_t v;

	gbcam_address(address);

	LPC_GPIO2->DIR &= ~(0xFF);				// Hi-z

	LPC_GPIO2->DATA &= ~(1<<9);				// CS and RD low
	LPC_GPIO2->DATA &= ~(1<<10);

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");

	v = LPC_GPIO2->DATA & 0xFF;				// Read data bus

	LPC_GPIO2->DATA |= (1<<10);				// RD and CS high
	LPC_GPIO2->DATA |= (1<<9);

	return v;
}

void gbcam_set(uint16_t address, uint8_t value) {
	gbcam_address(address);
	delay_us(1);
	//Output
	LPC_GPIO2->DATA = (LPC_GPIO2->DATA & 0xF00) | value;
	delay_us(1);

	if (address >= 0xA000) {
		LPC_GPIO2->DATA &= ~(1<<9);		// CS low
		delay_us(1);
	}
	LPC_GPIO2->DATA &= ~(1<<8);			// WR low
	delay_us(2);
	LPC_GPIO2->DATA |= (1<<8);			// WR high
	if (address >= 0xA000) {
		delay_us(1);
		LPC_GPIO2->DATA |= (1<<9);		// CS high
	}
}

// Returns 1 if GB Cam is detected
uint8_t gbcam_detect(void) {
	uint8_t c;
	const char romname[14] = "GAMEBOYCAMERA";

	gbcam_set(0x0000, 0x0A);			// Initialize MBC
	delay_us(2000);

	// Check for ID string in ROM bank 0
	for (c=0; c<13; c++) {
		if (gbcam_get(c + 0x0134) != romname[c]) return 0;
	}

	// Check if A000 bit 0 is = 0
	gbcam_set(0x4000, 0x10);
	delay_us(1000);
	if (gbcam_get(0xA000) & 1) return 0;

	return 1;
}

void gbcam_reset(void) {
	LPC_GPIO1->DATA &= ~(1<<11);		// GBCRST low
	delay_us(20000);
	LPC_GPIO1->DATA |= (1<<11);			// GBCRST high
	delay_us(10000);
}

void gbcam_setmatrix(uint8_t * matrix) {
	uint8_t c;

	gbcam_set(0x4000, 0x10);			// ASIC registers
	delay_us(10);
	while(gbcam_get(0xA000) & 1);		// Could freeze !

	for (c=0; c<48; c++)
		gbcam_set(c + 0xA006, matrix[c]);
}

void gbcam_setexposure(uint16_t exposure) {
	gbcam_set(0x4000, 0x10);			// ASIC registers
	delay_us(10);
	while(gbcam_get(0xA000) & 1);	// Could freeze !

	gbcam_set(0xA002, exposure >> 8);
	gbcam_set(0xA003, exposure & 0xFF);
}
