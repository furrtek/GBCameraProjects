// GBLiveCam firmware for STM32F730R8T6
// furrtek 2019 - See LICENSE

// Changes from PCB rev. B:
// No more bus demux, direct data and address
// ALEL and ALEH are gone
// /GBCWR on PA8 instead of PA7
// GBCPHI on PA7 instead of PB14
// Address bus on port B

#include "main.h"
#include "usb_device.h"
#include "init.h"
#include "gbcam.h"
#include "osd.h"
#include "palettes.h"

TIM_HandleTypeDef s_TimerInstance;
uint8_t raw_buffer[GB_FRAME_SIZE];
uint8_t linear_buffer_a[128*112/4];
uint8_t linear_buffer_b[128*112/4];
volatile uint8_t new_frame_trigger = 0;
volatile uint8_t flag_update_exposure, flag_update_matrix;
volatile uint32_t buffer_flip = 0;
volatile uint8_t debug_val;
TIM_HandleTypeDef s_PhiTimer;
int32_t ae_last_error;
uint32_t OSD_timer = 0;
uint32_t error_acc;
int16_t final_exposure;

void blank_buffer(uint8_t* buffer) {
	for (uint32_t c = 0; c < GB_FRAME_SIZE; c++)
		buffer[c] = 0x55;	// 4 pixels with color #1
}

int main(void) {
	uint32_t c, error_flag;
	uint8_t gbcam_present = 0;
	uint32_t luma_hist[4];
	uint32_t luma_acc = 0;
	uint32_t luma_idx = 0;
	TIM_OC_InitTypeDef TIM_OCStruct;

	cam_status = STATUS_STOPPED;
	settings.auto_exposure = 1;
	settings.exposure = 50;
	settings.hue = 0;
	settings.brightness = 70;
	settings.contrast = 10;
	settings.gain = 5;
	settings.debug = 0;

	HAL_Init();

	SystemClock_Config();

	MX_GPIO_Init();
	MX_ADC1_Init();
	MX_USB_DEVICE_Init();

	// For delays
	__TIM2_CLK_ENABLE();
	s_TimerInstance.Instance = TIM2;
	s_TimerInstance.Init.Prescaler = 120;
	s_TimerInstance.Init.CounterMode = TIM_COUNTERMODE_UP;
	s_TimerInstance.Init.Period = 100000;
	s_TimerInstance.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	s_TimerInstance.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&s_TimerInstance);
	HAL_TIM_Base_Start(&s_TimerInstance);

	// GB Phi generator
	s_PhiTimer.Init.Prescaler = 1;
	s_PhiTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	s_PhiTimer.Init.Period = 30-1;	// 120M/30/2/2=1M
	s_PhiTimer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	s_PhiTimer.Init.RepetitionCounter = 0;
	s_PhiTimer.Channel = HAL_TIM_ACTIVE_CHANNEL_1;
	TIM_OCStruct.OCMode = TIM_OCMODE_TOGGLE;
	TIM_OCStruct.Pulse = 0;
#if PCBREV == 'B'
	__TIM12_CLK_ENABLE();
	s_PhiTimer.Instance = TIM12;
	TIM12->CR1 = TIM_COUNTERMODE_UP | TIM_CLOCKDIVISION_DIV1;
	TIM12->ARR = (uint32_t)12-1;
	TIM12->PSC = 1;
	TIM12->EGR = TIM_EGR_UG;
#else
	__TIM14_CLK_ENABLE();
	s_PhiTimer.Instance = TIM14;
	TIM14->CR1 = TIM_COUNTERMODE_UP | TIM_CLOCKDIVISION_DIV1;
	TIM14->ARR = (uint32_t)12-1;
	TIM14->PSC = 1;
	TIM14->EGR = TIM_EGR_UG;
#endif

	HAL_TIM_OC_ConfigChannel(&s_PhiTimer, &TIM_OCStruct, TIM_CHANNEL_1);
	//HAL_TIM_OC_Start(&s_Timer9, TIM_CHANNEL_1);
	HAL_TIM_OC_Start(&s_PhiTimer, TIM_CHANNEL_1);
	// GBCam doesn't like Phi with no sync: ROM reads are corrupt
	// No Phi means no RAM access though

	blank_buffer(linear_buffer_a);
	OSD_write_nocam(linear_buffer_a);
	blank_buffer(linear_buffer_b);
	OSD_write_nocam(linear_buffer_b);

    error_acc = 0;
    final_exposure = 2080;

	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_14);	// Red LED
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);

	while (1) {
		if (cam_status == STATUS_STOPPED) {
			if (settings.debug == SPECIAL_VALUE_DFU) {
				// Magic value to jump to DFU mode
				// +00000000: Stack pointer
				// +00000004: Reset vector
				uint32_t JumpAddress = *(__IO uint32_t*)(ADDR_SYSMEM + 4);
				void (*Jump)(void) = (void (*)(void))JumpAddress;

				HAL_RCC_DeInit();
				HAL_DeInit();

				SysTick->CTRL = 0;
				SysTick->LOAD = 0;
				SysTick->VAL = 0;

				__set_MSP(*(__IO uint32_t*)ADDR_SYSMEM);
				Jump();

				while(1);
			}
		} else {
			if (gbcam_present) {
				if (flag_update_matrix) {
					gbcam_setgain(settings.gain);
					gbcam_setmatrix(20 - settings.contrast, 200 - settings.brightness);
				}

				if (new_frame_trigger) {
					error_flag = 0;

					if (settings.auto_exposure) {
						// Min image brightness: 128*112*0=0
						// Max image brightness: 128*112*3=43008

						// Indoor: Brightness 100, Contrast 10, Gain 10
						// Outdoors: Brightness 160, Contrast 6, Gain 1

						int32_t luma_mean = 0;
						for (c = 0; c < 4; c++)
							luma_mean += luma_hist[c];

						luma_mean >>= 2;

						int32_t ae_error = ((43008 / 2) - luma_mean) / 32;
						int32_t ae_rate = ae_error - ae_last_error;
						ae_last_error = ae_error;

						final_exposure += (ae_error / 16 + ae_rate / 32);

						if (final_exposure < 32) {
							final_exposure = 32;
						} else if (final_exposure > 3000) {
							final_exposure = 3000;
						}

						gbcam_setexposure(final_exposure);
					} else {
						// Manual setting
						if (flag_update_exposure) {
							gbcam_setexposure(32 + (settings.exposure << 4));
							flag_update_exposure = 0;
						}
					}

					// Read scratchpad to raw_buffer
					cart_put(0x4000, 0x00);	// SRAM bank 0
					delay_us(2);
					for (c = 0; c < GB_FRAME_SIZE; c++)
						raw_buffer[c] = cart_get_ram(0xA100 + c) ^ 0xFF;

					// Ask ASIC for new capture
					cart_put(0x4000, 0x10);	// ASIC registers
					delay_us(2);
					cart_put(0xA000, 0x03);

					// Wait for capture to start
					error_flag |= gbcam_wait_busy();

					// Check if cart is still there
					error_flag |= gbcam_detect();

					uint8_t * linear_buffer_wr = buffer_flip ? linear_buffer_b : linear_buffer_a;

					// Convert planar GB tile data to linear array
					luma_acc = 0;
					uint32_t i = 0, sr = 0;
					for (uint32_t yt = 0; yt < 112; yt++) {

						uint32_t yto = (yt & 7) + ((yt & 0x78) << 4);

						for (uint32_t xt = 0; xt < 16; xt++) {
							// (xt << 3): tile # x * pixel rows per tile
							// + (yt & 7): pixel row # in tile
							// + ((yt & 0x78) << 4): tile # y * tiles per row
							// << 1: 2 bytes per 8x 2bpp pixel row

							uint32_t addr = ((xt << 3) + yto) << 1;

							// Get 8 pixels worth of data
							uint8_t data_l = raw_buffer[addr];
							uint8_t data_h = raw_buffer[addr + 1];

							for (uint32_t x = 0; x < 8; x++) {
								// Planar to linear
								uint8_t pixel = (data_h & 0x80) ? 2 : 0;
								if (data_l & 0x80)
									pixel |= 1;

								// Stack 2-bit pixels next to each other
								sr <<= 2;
								sr |= pixel;

								luma_acc += pixel;

								// Store byte (4 pixels)
								if ((x & 3) == 3)
									*(linear_buffer_wr + (i >> 2)) = sr;

								data_l <<= 1;
								data_h <<= 1;
								i++;
							}
						}
					}

					// Moving average over luma for auto-exposure
					luma_idx = (luma_idx + 1) & 3;
					luma_hist[luma_idx] = luma_acc;

					error_flag |= gbcam_wait_idle();

					if (error_flag) {
						if (error_acc < ERROR_THRESHOLD)
							error_acc++;
						else
							gbcam_present = 0;
					}

					if (!OSD_write_special(linear_buffer_wr)) {
						if (OSD_timer) {	// Only show palette name if nothing special to show
							OSD_write(2, 2, linear_buffer_wr, (char*)palettes[settings.hue & 7].name);
							OSD_timer--;
						}
					}

					new_frame_trigger = 0;
				}
			} else {
				// No GB Camera cart present
				if (new_frame_trigger) {
					uint8_t * linear_buffer_wr = buffer_flip ? linear_buffer_b : linear_buffer_a;

					blank_buffer(linear_buffer_wr);
					if (!OSD_write_special(linear_buffer_wr))
						OSD_write_nocam(linear_buffer_wr);	// Only show message if nothing special to show

					new_frame_trigger = 0;
				}

				if (!gbcam_detect()) {
					gbcam_init();
					flag_update_exposure = 1;
					gbcam_present = 1;
					error_acc = 0;
				}
			}
		}
	}
}

void delay_us(uint32_t delay) {
	__HAL_TIM_SET_COUNTER(&s_TimerInstance, 0);
	while (__HAL_TIM_GET_COUNTER(&s_TimerInstance) < delay) { };
}

void Error_Handler(void) {
}
