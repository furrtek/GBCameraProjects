/*
 * gbcam.c
 *
 *  Created on: 07 oct. 2015
 *      Author: furrtek
 */

#include "LPC13xx.h"
#include "main.h"
#include "gbcam.h"
#include "io.h"

// Dithering layout taken from real GB Cam (see routine at ROMA:4427)
// Table of matrix offsets * 3
//     0   1   2   3
//   ---------------
// 0 | A   M   D   P
// 1 | I   E   L   H
// 2 | C   O   B   N
// 3 | K   G   J   F
const uint8_t matrix_layout[16] = { 0, 30, 24, 6,
									15, 45, 39, 21,
									12, 42, 36, 18,
									3, 33, 27, 9 };

// Contrast values for computing dithering matrix
uint8_t qlevels[4];

uint8_t gbcam_wait_busy() {
	uint32_t timeout = 100000;

	gbcam_set(0x4000, 0x10);			// ASIC registers
	while (!(gbcam_get(0xA000) & 1)) {
		if (timeout)
			timeout--;
		else
			return 1;
	}
	return 0;
}

uint8_t gbcam_wait_idle() {
	uint32_t timeout = 100000;

	gbcam_set(0x4000, 0x10);			// ASIC registers
	while (gbcam_get(0xA000) & 1) {
		if (timeout)
			timeout--;
		else
			return 1;
	}
	return 0;
}

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

	LPC_GPIO2->DIR &= ~(0xFF);			// Hi-z

	LPC_GPIO2->DATA &= ~(1<<9);			// CS low
	delay_us(1);
	LPC_GPIO2->DATA &= ~(1<<10);		// RD low

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

	v = LPC_GPIO2->DATA & 0xFF;			// Read data bus

	LPC_GPIO2->DATA |= (1<<10);			// RD high
	//delay_us(1);
	LPC_GPIO2->DATA |= (1<<9);			// CS high

	return v;
}

uint8_t gbcam_get_rom(uint16_t address) {
	uint8_t v;

	gbcam_address(address);

	LPC_GPIO2->DATA |= (1<<9);			// CS high
	LPC_GPIO2->DIR &= ~(0xFF);			// Hi-z
	LPC_GPIO2->DATA &= ~(1<<10);		// RD low

	delay_us(15);						// ROM is slow :(

	v = LPC_GPIO2->DATA & 0xFF;			// Read data bus
	LPC_GPIO2->DATA |= (1<<10);			// RD high

	return v;
}

void gbcam_set(uint16_t address, uint8_t value) {
	gbcam_address(address);
	delay_us(1);

	LPC_GPIO2->DIR |= 0xFF;				// Output

	LPC_GPIO2->DATA = (LPC_GPIO2->DATA & 0xF00) | value;
	delay_us(1);

	if (address >= 0xA000) {
		LPC_GPIO2->DATA &= ~(1<<9);		// CS low
		delay_us(1);
	}

	LPC_GPIO2->DATA &= ~(1<<8);			// WR low (PHI must always be high right now ?)
	delay_us(3);
	LPC_GPIO2->DATA |= (1<<8);			// WR high

	if (address >= 0xA000) {
		delay_us(1);
		LPC_GPIO2->DATA |= (1<<9);		// CS high
	}
}

// Returns 0 if GB Cam is detected
uint8_t gbcam_detect(void) {
	const char rom_name[14] = "GAMEBOYCAMERA";
	uint8_t c;

	gbcam_set(0x0000, 0x0A);			// Initialize MBC, allow writing to RAM
	delay_us(5000);

	// Check for ID string in ROM bank 0
	for (c = 0; c < 13; c++) {
		if (gbcam_get_rom(c + 0x0134) != rom_name[c])
			return 1;
	}

	// Check if A000 bit 0 is = 0 (idle)
	return gbcam_wait_idle();
}

void gbcam_reset(void) {
	LPC_GPIO1->DATA &= ~(1<<11);		// GBCRST low
	delay_us(20000);
	LPC_GPIO1->DATA |= (1<<11);			// GBCRST high
	delay_us(10000);
}

// TESTING - This is a hardcoded 16 entry LUT @ ROMA:7C20 in the GB Cam ROM
void gbcam_setcontrast(uint8_t slope) {
	uint16_t c;

    for (c = 0; c < 4; c++)
    	qlevels[c] = (146 - (slope >> 1)) + (c * slope);
}

// Compute GB Cam dithering matrix values (see GB Cam ROM)
void gbcam_setmatrix() {
	uint16_t c, acc, inc;
	uint8_t pixel;

	// Each entry in the 4x4 matrix is 3 bytes indicating the threshold levels for each shade of grey
	// Pixel voltage < byte A -> black
	// byte A < Pv < byte B -> dark grey
	// byte B < Pv < byte C -> light grey
	// byte C < Pv -> white
	// Those are used to generate the voltages for the 3 comparators in the GB Cam ASIC
	// The GB Cam ROM generates the whole matrix from 4 bytes: qlevels array (threshold levels also),
	// which are "spread out" by interpolation on the 16 pixels.
	// Bytes A for each pixel are qlevels[0] -> qlevels[1]
	// Bytes B for each pixel are qlevels[1] -> qlevels[2]
	// Bytes C for each pixel are qlevels[2] -> qlevels[3]
    for (c = 0; c < 3; c++) {
    	acc = qlevels[c];
    	inc = qlevels[c + 1] - acc;
    	acc <<= 4;
		for (pixel = 0; pixel < 16; pixel++) {
			gbcam_matrix[c + matrix_layout[pixel]] = acc >> 4;
			acc += inc;
		}
    }

	gbcam_set(0x4000, 0x10);			// ASIC registers
	delay_us(100);
	gbcam_wait_idle();

	for (c = 0; c < 48; c++)
		gbcam_set(c + 0xA006, gbcam_matrix[c]);
}

void gbcam_setexposure(uint16_t exposure) {
	gbcam_set(0x4000, 0x10);			// ASIC registers
	delay_us(100);
	gbcam_wait_idle();

	gbcam_set(0xA002, exposure >> 8);
	gbcam_set(0xA003, exposure & 0xFF);
}
