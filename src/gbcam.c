/*
===============================================================================
 Name        : GBCamcorder
 Author      : furrtek
 Version     : 0.3
 Copyright   : CC Attribution-NonCommercial-ShareAlike 4.0
 Description : GameBoy Camcorder firmware
===============================================================================
*/

#include "LPC13xx.h"
#include "main.h"
#include "gbcam.h"
#include "io.h"

// Dithering layout taken from real GB Cam (see routine at ROMA:4427)
// Table of matrix offsets * 3, order is top-bottom, left-right
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
	uint32_t timeout = 200000;

	cart_put(0x4000, 0x10);			// ASIC registers
	while (!(cart_get_ram(0xA000) & 1)) {
		if (timeout)
			timeout--;
		else
			return 1;
	}
	return 0;
}

uint8_t gbcam_wait_idle() {
	uint32_t timeout = 200000;

	cart_put(0x4000, 0x10);			// ASIC registers
	while (cart_get_ram(0xA000) & 1) {
		if (timeout)
			timeout--;
		else
			return 1;
	}
	return 0;
}

void cart_set_address(const uint16_t address) {
	LPC_GPIO1->DATA &= ~((1<<2) | (1<<3));				// ALEs both low
	LPC_GPIO2->DATA |= ((1<<8) | (1<<9) | (1<<10));		// Make sure cart isn't asked anything
	LPC_GPIO2->DIR |= 0xFF;				// GB bus: Output

	// Set address low
	LPC_GPIO2->DATA = (LPC_GPIO2->DATA & 0xF00) | (address & 0x00FF);

	LPC_GPIO1->DATA |= (1<<2);			// Clock ALEL
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	LPC_GPIO1->DATA &= ~(1<<2);

	// Set address high
	LPC_GPIO2->DATA = (LPC_GPIO2->DATA & 0xF00) | (address >> 8);

	LPC_GPIO1->DATA |= (1<<3);			// Clock ALEH
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	LPC_GPIO1->DATA &= ~(1<<3);
}

uint8_t cart_get_ram(const uint16_t address) {
	uint8_t v;

	cart_set_address(address);

	LPC_GPIO2->DIR &= ~(0xFF);			// GB bus: Input

	LPC_GPIO2->DATA &= ~(1<<9);			// CS low
	delay_us(1);
	LPC_GPIO2->DATA &= ~(1<<10);		// RD low

	__asm("nop");						// TODO: Long enough ?
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

uint8_t cart_get_rom(const uint16_t address) {
	uint8_t v;

	cart_set_address(address);

	LPC_GPIO2->DIR &= ~(0xFF);			// GB bus: Input
	LPC_GPIO2->DATA |= (1<<9);			// CS high
	LPC_GPIO2->DATA &= ~(1<<10);		// RD low

	delay_us(20);						// TODO: ROM shouldn't be so slow :(

	v = LPC_GPIO2->DATA & 0xFF;			// Read data bus
	LPC_GPIO2->DATA |= (1<<10);			// RD high

	return v;
}

void cart_put(const uint16_t address, const uint8_t value) {
	cart_set_address(address);
	delay_us(1);						// TODO: Make shorter ?

	LPC_GPIO2->DIR |= 0xFF;				// GB bus: Output

	LPC_GPIO2->DATA = (LPC_GPIO2->DATA & 0xF00) | value;
	delay_us(1);						// TODO: Make shorter ?

	if (address >= 0xA000) {
		LPC_GPIO2->DATA &= ~(1<<9);		// CS low
		delay_us(1);					// TODO: Make shorter ?
	}

	LPC_GPIO2->DATA &= ~(1<<8);			// WR low (TODO: PHI must be high right now ?)
	delay_us(3);						// TODO: Make shorter ?
	LPC_GPIO2->DATA |= (1<<8);			// WR high

	if (address >= 0xA000) {
		delay_us(1);					// TODO: Make shorter ?
		LPC_GPIO2->DATA |= (1<<9);		// CS high
	}
}

// Returns 0 if GB Cam is detected
uint8_t gbcam_detect(void) {
	const char rom_name[14] = "GAMEBOYCAMERA";
	uint32_t c;

	cart_put(0x0000, 0x0A);			// Initialize MBC, allow writing to RAM
	delay_us(5000);

	// Check for ID string in ROM bank 0
	for (c = 0; c < 13; c++) {
		if (cart_get_rom(c + 0x0134) != rom_name[c])
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

// This is a hardcoded 16 entry LUT @ ROMA:7C20 in the GB Cam ROM
// Slope should be 0~31 here
void gbcam_setcontrast(const uint8_t slope, const uint8_t offset) {
	uint16_t c, v;

    for (c = 0; c < 4; c++) {
    	v = offset + (146 - (slope >> 1)) + (c * slope);
    	qlevels[c] = (v < 256) ? v : 255;
    }
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
	// The GB Cam ROM generates the whole matrix from 4 bytes: qlevels array (and threshold levels also),
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

    cart_put(0x4000, 0x10);			// ASIC registers
	delay_us(100);
	gbcam_wait_idle();

	for (c = 0; c < 48; c++)
		cart_put(c + 0xA006, gbcam_matrix[c]);
}

void gbcam_setexposure(const uint16_t exposure) {
	cart_put(0x4000, 0x10);			// ASIC registers
	delay_us(100);
	gbcam_wait_idle();

	cart_put(0xA002, exposure >> 8);
	cart_put(0xA003, exposure & 0xFF);
}
