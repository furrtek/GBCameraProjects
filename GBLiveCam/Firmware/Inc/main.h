#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f7xx_hal.h"
#include "stm32f7xx_ll_adc.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_system.h"
#include "stm32f7xx_ll_exti.h"
#include "stm32f7xx_ll_cortex.h"
#include "stm32f7xx_ll_utils.h"
#include "stm32f7xx_ll_pwr.h"
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx.h"
#include "stm32f7xx_ll_gpio.h"

#define PCBREV 'C'

#if PCBREV == 'B'
#define VERSION_STR "1.0B"
#else
#define VERSION_STR "1.0C"
#endif

#define ERROR_THRESHOLD 3

#define SPECIAL_VALUE_VERSION 123
#define SPECIAL_VALUE_DEBUG 999
#define SPECIAL_VALUE_DFU 1337

#define ADDR_SYSMEM (uint32_t)0x00100000	// RM0431 page 56

void Error_Handler(void);
void delay_us(uint32_t delay);

volatile enum {
	STATUS_STOPPED,
	STATUS_READY,
	STATUS_RUNNING
} cam_status;

volatile struct {
	uint32_t exposure;
	uint8_t auto_exposure;
	uint16_t hue;
	uint16_t brightness;
	uint16_t contrast;
	uint16_t gain;
	uint16_t debug;
} settings;

extern volatile uint8_t new_frame_trigger;
extern volatile uint8_t flag_update_matrix;
extern volatile uint8_t flag_update_exposure;
extern TIM_HandleTypeDef s_PhiTimer;
extern uint32_t OSD_timer;
extern uint32_t error_acc;
extern int16_t final_exposure;

extern volatile uint8_t debug_val;

#endif /* __MAIN_H */
