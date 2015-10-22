/*
 * sdcard.h
 *
 *  Created on: 26 sept. 2015
 *      Author: furrtek
 */

#include "LPC13xx.h"
#include "diskio.h"

#ifndef SDCARD_H_
#define SDCARD_H_

#define MMC_DATA_SIZE 512

#define MAX_TIMEOUT 0xFF
#define IDLE_STATE_TIMEOUT 1

#define OP_COND_TIMEOUT 2
#define SET_BLOCKLEN_TIMEOUT 3
#define WRITE_BLOCK_TIMEOUT 4
#define WRITE_BLOCK_FAIL 5
#define READ_BLOCK_TIMEOUT 6
#define READ_BLOCK_DATA_TOKEN_MISSING 7
#define DATA_TOKEN_TIMEOUT 8
#define SELECT_CARD_TIMEOUT 9
#define SET_RELATIVE_ADDR_TIMEOUT 10

uint8_t MMCCmd[6];

void FCLK_LCD();
void FCLK_FAST();
void FCLK_SLOW();
void xmit_spi(BYTE dat);

void spi_init(void);
void spi_send(const uint8_t *buf, uint16_t length);
void spi_receive(uint8_t *buf, uint16_t length);
uint8_t spi_receivebyte(void);
int sd_init(void);
int sd_response(uint8_t response);
DRESULT sd_read_single_block(const uint8_t *buff, uint16_t block_number);
DRESULT sd_write_single_block(const uint8_t *buff, uint16_t block_number);
int sd_wait_for_write_finish(void);

#endif /* SDCARD_H_ */
