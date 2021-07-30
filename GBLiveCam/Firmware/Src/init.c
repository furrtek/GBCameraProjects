#include "main.h"
#include "init.h"

void MX_ADC1_Init(void) {
	LL_ADC_InitTypeDef ADC_InitStruct = {0};
	LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
	LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Peripheral clock enable */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	/**ADC1 GPIO Configuration
	PA0-WKUP   ------> ADC1_IN0*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/** Common config*/
	ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;
	ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
	ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;
	LL_ADC_Init(ADC1, &ADC_InitStruct);
	ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
	ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
	ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
	ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;
	ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;
	LL_ADC_REG_Init(ADC1, &ADC_REG_InitStruct);
	LL_ADC_REG_SetFlagEndOfConversion(ADC1, LL_ADC_REG_FLAG_EOC_UNITARY_CONV);
	LL_ADC_DisableIT_EOCS(ADC1);
	ADC_CommonInitStruct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV2;
	ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;
	LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &ADC_CommonInitStruct);
	/** Configure Regular Channel */
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_3CYCLES);
}

void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	// Configure the main internal regulator output voltage
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	// HSE: 8MHz crystal
	// Initializes the CPU, AHB and APB busses clocks
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	//RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	//RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 96;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;	// 8M/4*96/2=96M SYSCLK
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		Error_Handler();

	// Initializes the CPU, AHB and APB busses clocks
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)	// FLASH_LATENCY_3 ?
		Error_Handler();
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
	PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		Error_Handler();
}

void MX_GPIO_Init(void) {
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);

	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_5);
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_6);
	LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_7);
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_8);
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_14);
	LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_15);

	LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_14);

	LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_0);
	LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_3);

	// GB cart Phi
	GPIO_InitStruct.Pin = LL_GPIO_PIN_14;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;	// LL_GPIO_SPEED_FREQ_VERY_HIGH ?
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_9;	// TIM12_CH1
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// GB cart control
	GPIO_InitStruct.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7 | LL_GPIO_PIN_8;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;	// LL_GPIO_SPEED_FREQ_VERY_HIGH ?
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_3;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	// LEDs
	GPIO_InitStruct.Pin = LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// Bus
	GPIO_InitStruct.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 |
							LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;	// LL_GPIO_SPEED_FREQ_VERY_HIGH ?
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}
