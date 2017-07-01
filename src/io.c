/*
 * io.c
 *
 *  Created on: 12 oct. 2015
 *      Author: furrtek
 */

#include "LPC13xx.h"
#include "main.h"

// Microsecond wait using TMR16B0 (1us tick)
void delay_us(uint16_t delay) {
    LPC_TMR16B0->TC = 0;
    while (LPC_TMR16B0->TC < delay);
}

void read_inputs() {
	// 00054321: PB5 (11) and PB1~4 (0~3)
	inputs_prev = inputs_current;
	inputs_current = (((LPC_GPIO2->DATA >> 7) & 0x10) | (LPC_GPIO3->DATA & 0x0F)) ^ 0x1F;
	inputs_active = (inputs_current ^ inputs_prev) & inputs_current;
}

void init_io(void) {
	LPC_IOCON->RESET_PIO0_0 |= 0x10;		//Pull-up
	LPC_IOCON->PIO0_2 &= ~(0x07);
	LPC_IOCON->PIO0_3 |= 1;
	LPC_IOCON->PIO0_4 = 0;
	LPC_IOCON->PIO0_5 = 0;
	LPC_IOCON->PIO0_6 |= 1;
	LPC_IOCON->PIO0_7 &= ~(0x3F);
	LPC_IOCON->PIO0_8 &= ~(0x3F);
	LPC_IOCON->PIO0_8 |= 1;					// MISO
	LPC_IOCON->PIO0_9 &= ~(0x3F);
	LPC_IOCON->PIO0_9 |= 1;					// MOSI
	LPC_IOCON->JTAG_TCK_PIO0_10 &= ~(0x3F);
	LPC_IOCON->JTAG_TCK_PIO0_10 |= 2;		// SCK0
	LPC_IOCON->JTAG_TDI_PIO0_11 &= ~(0x8F);
	LPC_IOCON->JTAG_TDI_PIO0_11 |= 2;		// AD0 Mic input

	LPC_IOCON->JTAG_TMS_PIO1_0 = 0xC1;
	LPC_IOCON->JTAG_TDO_PIO1_1 = 0xC3;		// Backlight PWM
	LPC_IOCON->JTAG_nTRST_PIO1_2 = 0xC1;
	LPC_IOCON->ARM_SWDIO_PIO1_3 = 0xC1;
	LPC_IOCON->PIO1_4 &= ~(0x3F);
	LPC_IOCON->PIO1_5 &= ~(0x3F);
	LPC_IOCON->PIO1_6 |= 2;					// PWM audio out
	LPC_IOCON->PIO1_7 |= 1;					// TXD
	LPC_IOCON->PIO1_8 &= ~(0x3F);
	LPC_IOCON->PIO1_9 &= ~(0x3F);
	LPC_IOCON->PIO1_9 |= 1;					// GB Phi
	LPC_IOCON->PIO1_10 &= ~(0x3F);
	LPC_IOCON->PIO1_11 &= ~(0x3F);

	LPC_IOCON->PIO2_0 &= ~(0x3F);
	LPC_IOCON->PIO2_1 &= ~(0x3F);
	LPC_IOCON->PIO2_2 &= ~(0x3F);
	LPC_IOCON->PIO2_3 &= ~(0x3F);
	LPC_IOCON->PIO2_4 &= ~(0x3F);
	LPC_IOCON->PIO2_5 &= ~(0x3F);
	LPC_IOCON->PIO2_6 &= ~(0x3F);
	LPC_IOCON->PIO2_7 &= ~(0x3F);
	LPC_IOCON->PIO2_8 &= ~(0x3F);
	LPC_IOCON->PIO2_9 &= ~(0x3F);
	LPC_IOCON->PIO2_10 &= ~(0x3F);
	LPC_IOCON->PIO2_11 |= 0x10;				// Pull-up

	LPC_IOCON->PIO3_0 |= 0x10;				// Pull-up
	LPC_IOCON->PIO3_1 |= 0x10;				// Pull-up
	LPC_IOCON->PIO3_2 |= 0x10;				// Pull-up
	LPC_IOCON->PIO3_3 |= 0x10;				// Pull-up

	LPC_IOCON->SCKLOC = 0;					// SSP SCK on SCK0 pin

	LPC_GPIO0->DIR = 0b011011110100;
	LPC_GPIO1->DIR = 0b101101101110;
	LPC_GPIO2->DIR = 0b011100000000;
	LPC_GPIO3->DIR = 0b0000;

	LPC_GPIO0->DATA = 0b001001110100;		// CSes and resets inactive
	LPC_GPIO1->DATA = 0b100100100000;
	LPC_GPIO2->DATA = 0b011100000000;
	LPC_GPIO3->DATA = 0b0000;
}
