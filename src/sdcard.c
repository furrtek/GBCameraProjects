/*
 * sdcard.c
 *
 *  Created on: 26 sept. 2015
 *      Author: furrtek
 */


#include "LPC13xx.h"
#include "main.h"
#include "sdcard.h"
#include "diskio.h"

void spi_init(void) {
	uint8_t i, dummy;

    LPC_SYSCON->PRESETCTRL &= ~0x01;
	LPC_SYSCON->PRESETCTRL |= 0x01;	//SSP0 out of reset state
	LPC_SYSCON->SSPCLKDIV = 0x20;	//0x20
	LPC_SSP->CPSR = 0x2;
	LPC_SSP->CR0 = 0x0407;
	LPC_SSP->CR1 = 0x2;				//SSP0 on

	for (i = 0; i < 8; i++)
		dummy = LPC_SSP->DR;		//Clear RXFIFO

	(void)dummy;
}

/*
void spi_send(const uint8_t *buf, uint16_t length) {
	uint8_t dummy;
	if (!length) return;

	while (length) {
		while (!(LPC_SSP->SR & 0x02));
		LPC_SSP->DR = *buf;
		while ((LPC_SSP->SR & 0x10));
		dummy = LPC_SSP->DR;
		length--;
		buf++;
	}
	return;
}

void spi_receive(uint8_t *buf, uint16_t length) {
	uint16_t i;

	for (i=0; i<length; i++) {
		*buf = spi_receivebyte();
		buf++;
	}
	return;
}

uint8_t spi_receivebyte(void) {
	uint8_t data;

	LPC_SSP->DR = 0xFF;
	while (LPC_SSP->SR & 0x10);
	data = LPC_SSP->DR;
	return data;
}

int sd_init() {
	uint32_t i;

	// Initialise the MMC card into SPI mode by sending 80 clks
	for(i=0; i<5; i++)
		MMCCmd[i] = 0xFF;

	spi_send(MMCCmd, 5);
	spi_send(MMCCmd, 5);

	LPC_GPIO0->DATA &= ~(1<<4);

	// CMD0
	MMCCmd[0] = 0x40;
	MMCCmd[1] = 0x00;
	MMCCmd[2] = 0x00;
	MMCCmd[3] = 0x00;
	MMCCmd[4] = 0x00;
	MMCCmd[5] = 0x95;
	spi_send(MMCCmd, 6);

	if (sd_response(0x01) == 1) {
		LPC_GPIO0->DATA |= (1<<4);
		return IDLE_STATE_TIMEOUT;
	}
	LPC_GPIO0->DATA |= (1<<4);
	spi_receivebyte();
	LPC_GPIO0->DATA &= ~(1<<4);

	// CMD8
	MMCCmd[0] = 0x48;
	MMCCmd[1] = 0x00;
	MMCCmd[2] = 0x00;
	MMCCmd[3] = 0x01;
	MMCCmd[4] = 0xAA;
	MMCCmd[5] = 0xFF;
	spi_send(MMCCmd, 6);

	spi_receivebyte();
	if (sd_response(0x01) == 1) {
		//MMC
		// CMD1
		MMCCmd[0] = 0x41;
		MMCCmd[1] = 0x00;
		MMCCmd[2] = 0x00;
		MMCCmd[3] = 0x00;
		MMCCmd[4] = 0x00;
		MMCCmd[5] = 0xFF;
		spi_send(MMCCmd, 6);

		i = MAX_TIMEOUT;
		do {
			i--;
		} while (((spi_receivebyte() & 1) == 1) && (i>0));

		if (i == 0) {
			LPC_GPIO0->DATA |= (1<<4);
			return OP_COND_TIMEOUT;
		}
		LPC_GPIO0->DATA |= (1<<4);
		spi_receivebyte();
		LPC_GPIO0->DATA &= ~(1<<4);
	} else {
		// CMD55
		MMCCmd[0] = 0x77;
		MMCCmd[1] = 0x00;
		MMCCmd[2] = 0x00;
		MMCCmd[3] = 0x00;
		MMCCmd[4] = 0x00;
		MMCCmd[5] = 0xFF;
		spi_send(MMCCmd, 6);

		// ACMD42
		i = MAX_TIMEOUT;
		do {
			// CMD1
			MMCCmd[0] = 0x69;
			MMCCmd[1] = 0x40;
			MMCCmd[2] = 0x00;
			MMCCmd[3] = 0x00;
			MMCCmd[4] = 0x00;
			MMCCmd[5] = 0xFF;
			spi_send(MMCCmd, 6);
			i--;
		} while ((sd_response(0x00) != 1) && (i>0));

		if (i == 0) {
			LPC_GPIO0->DATA |= (1<<4);
			return OP_COND_TIMEOUT;
		}
		LPC_GPIO0->DATA |= (1<<4);
		spi_receivebyte();
		LPC_GPIO0->DATA &= ~(1<<4);
	}

	// CMD16
	MMCCmd[0] = 0x50;
	MMCCmd[1] = 0x00;
	MMCCmd[2] = 0x00;
	MMCCmd[3] = 0x02;
	MMCCmd[4] = 0x00;
	MMCCmd[5] = 0xFF;
	spi_send(MMCCmd, 6);

	if (sd_response(0x00) == 1) {
		LPC_GPIO0->DATA |= (1<<4);
		return SET_BLOCKLEN_TIMEOUT;
	}

	LPC_GPIO0->DATA |= (1<<4);

	LPC_SSP->CR1 = 0x0;				// SSP0 off
	LPC_SYSCON->SSPCLKDIV = 0x02;
	delay_us(10);
	LPC_SSP->CR0 = 0x0007;			// SPI full speed
	LPC_SSP->CR1 = 0x2;				// SSP0 on

	spi_receivebyte();
	return 0;
}

DRESULT sd_write_single_block(const uint8_t *buff, uint16_t block_number) {
	uint16_t varl, varh;

	for(;;) {};

	LPC_GPIO0->DATA &= ~(1<<4);

	varl = ((block_number&0x003F)<<9);
	varh = ((block_number&0xFFC0)>>7);

	// CMD24
	MMCCmd[0] = 0x58;
	MMCCmd[1] = varh >> 0x08;
	MMCCmd[2] = varh & 0xFF;
	MMCCmd[3] = varl >> 0x08;
	MMCCmd[4] = varl & 0xFF;
	MMCCmd[5] = 0xFF;
	spi_send(MMCCmd, 6);

	if(sd_response(0x00) == 1) {
		LPC_GPIO0->DATA |= (1<<4);
		return RES_ERROR;
	}

	MMCCmd[0] = 0xFE;
	spi_send(MMCCmd, 1);

	spi_send(buff, 512);

	MMCCmd[0] = 0xFF;
	MMCCmd[1] = 0xFF;
	spi_send(MMCCmd, 2);

	if ((spi_receivebyte() & 0x0F) != 0x05)	{
		LPC_GPIO0->DATA |= (1<<4);
		return RES_ERROR;
	}

	if(sd_wait_for_write_finish() == 1) {
		LPC_GPIO0->DATA |= (1<<4);
		return RES_ERROR;
	}

	LPC_GPIO0->DATA |= (1<<4);
	spi_receivebyte();

	return RES_OK;
}

DRESULT sd_read_single_block(const uint8_t *buff, uint16_t block_number) {
	uint16_t checksum;
	uint16_t varh, varl;

	LPC_GPIO0->DATA &= ~(1<<4);

	varl = ((block_number&0x003F)<<9);
	varh = ((block_number&0xFFC0)>>7);

	// CMD17
	MMCCmd[0] = 0x51;
	MMCCmd[1] = varh >> 0x08;
	MMCCmd[2] = varh & 0xFF;
	MMCCmd[3] = varl >> 0x08;
	MMCCmd[4] = varl & 0xFF;
	MMCCmd[5] = 0xFF;
	spi_send(MMCCmd, 6);

	if(sd_response(0x00) == 1)	{
		LPC_GPIO0->DATA |= (1<<4);
		return RES_ERROR;
	}

	if(sd_response(0xFE) == 1) {
		LPC_GPIO0->DATA |= (1<<4);
		return RES_ERROR;
	}
	spi_receive(buff, 512);

	// CRC bytes are not needed
	checksum = spi_receivebyte();
	checksum = (checksum << 0x08) | spi_receivebyte();

	LPC_GPIO0->DATA |= (1<<4);
	spi_receivebyte();

	return RES_OK;
}

int sd_response(uint8_t response) {
	uint8_t count = 0x7F;				// 0xFF for slow cards?
	uint8_t result;

	while (count) {
		result = spi_receivebyte();
		if (result == response) break;
		count--;
	}
	if (!count)
		return 1;
	else
		return 0;
}

int sd_wait_for_write_finish(void) {
	uint16_t count = 0xFFFF;
	uint8_t result = 0;

	while((result == 0) && count) {
		result = spi_receivebyte();
		count--;
	}
	if (!count)
		return 1;
	else
		return 0;
}
*/
