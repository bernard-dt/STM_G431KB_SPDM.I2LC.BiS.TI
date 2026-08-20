#include "BreaderConfig.h"
#include "BreaderTypes.h"

// Prescaleer : 10US 
#define PRESCALE_FOR_10US     800
// Prescaleer : 100US 
#define PRESCALE_FOR_100US    8000

#define PERIOD_ARR_10         10
#define PERIOD_ARR_100        100
#define PERIOD_ARR_1000       1000
#define PERIOD_ARR_10000      10000

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;
U32 u32Timer6Cnt;
U32 u32Timer7Cnt;

// spi
extern SPI_HandleTypeDef hspi1;
extern structSpiTxRxBuf gstSpiTxRxBuf;


/* TIM6 init function */
void TIM6_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	// 1 ms = 1 kHz 
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = (PRESCALE_FOR_10US-1);
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = (PERIOD_ARR_100-1);         // should be less than 2 bytes 
	htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim6.Init.RepetitionCounter = 0;
	//htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

	if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim6, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	//sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim6); 
}

void HAL_TIM6_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	u32Timer6Cnt++;
	//SendSpiClock();
}

void HAL_TIM6_Base_MspInit(TIM_HandleTypeDef* htim)
{
	if(htim->Instance==TIM6)
	{
		__HAL_RCC_TIM6_CLK_ENABLE();

		HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	}
}

void HAL_TIM6_Base_MspDeInit(TIM_HandleTypeDef* htim)
{
	if(htim->Instance==TIM6)
	{
		__HAL_RCC_TIM6_CLK_DISABLE();

		HAL_NVIC_DisableIRQ(TIM6_DAC_IRQn);
	}
}


/* TIM7 init function */
void TIM7_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	// 10 ms = 100Hz 
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = (PRESCALE_FOR_10US-1);
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim7.Init.Period = (PERIOD_ARR_100-1);         // should be less than 2 bytes  => 100 Hz
	htim7.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim7.Init.RepetitionCounter = 0;
	//htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

	if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim7, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	//sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

void TIM7_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim7); 
}

void HAL_TIM7_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	u32Timer7Cnt++;
	SendSpiClock();
}

void HAL_TIM7_Base_MspInit(TIM_HandleTypeDef* htim)
{
	if(htim->Instance==TIM7)
	{
		__HAL_RCC_TIM7_CLK_ENABLE();

		HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM7_IRQn);
	}
}

void HAL_TIM7_Base_MspDeInit(TIM_HandleTypeDef* htim)
{
	if(htim->Instance==TIM7)
	{
		__HAL_RCC_TIM7_CLK_DISABLE();

		HAL_NVIC_DisableIRQ(TIM7_IRQn);
	}
}
