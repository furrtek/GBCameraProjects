#ifndef __GBCAM_H
#define __GBCAM_H

#define GB_FRAME_SIZE 	3584		// 128 * 112 * 2bpp / 8 bits
#define GBC_TIMEOUT		40000		// CPU speed dependent !

#define GBCCS_LOW		GPIOA->BSRR = (LL_GPIO_PIN_5 << 16);
#define GBCCS_HIGH		GPIOA->BSRR = LL_GPIO_PIN_5;
#define GBCRD_LOW		GPIOA->BSRR = (LL_GPIO_PIN_6 << 16);
#define GBCRD_HIGH		GPIOA->BSRR = LL_GPIO_PIN_6;
#define GBCWR_LOW		GPIOA->BSRR = (LL_GPIO_PIN_7 << 16);
#define GBCWR_HIGH		GPIOA->BSRR = LL_GPIO_PIN_7;
#define GBCRST_LOW		GPIOC->BSRR = (LL_GPIO_PIN_3 << 16);
#define GBCRST_HIGH		GPIOC->BSRR = LL_GPIO_PIN_3;
#define GBBUS_OUTPUT	GPIOC->MODER = (GPIOC->MODER & 0x0000FFFF) | 0x55550000;	// 0b0101010101010101<<16
#define GBBUS_INPUT		GPIOC->MODER = (GPIOC->MODER & 0x0000FFFF);
#define GBBUS_SET(v)	GPIOC->ODR = ((GPIOC->ODR & 0x00FF) | ((v) << 8));
#define GBBUS_READ		((GPIOC->IDR & 0xFF00) >> 8);

#define ALEL_LOW		GPIOA->BSRR = (LL_GPIO_PIN_8 << 16);
#define ALEL_HIGH		GPIOA->BSRR = LL_GPIO_PIN_8;
#define ALEH_LOW		GPIOC->BSRR = (LL_GPIO_PIN_0 << 16);
#define ALEH_HIGH		GPIOC->BSRR = LL_GPIO_PIN_0;

void cart_set_address(const uint16_t address);
uint8_t cart_get_ram(const uint16_t address);
uint8_t cart_get_rom(const uint16_t address);
void cart_put(const uint16_t address, const uint8_t value);
void gbcam_reset();
void gbcam_init();
uint32_t gbcam_detect();
uint8_t gbcam_wait_busy();
uint8_t gbcam_wait_idle();
void gbcam_setmatrix(const uint8_t slope, const uint8_t offset);
void gbcam_setgain(const uint32_t gain);
void gbcam_setexposure(const uint16_t exposure);

#endif  /* __GBCAM_H */
