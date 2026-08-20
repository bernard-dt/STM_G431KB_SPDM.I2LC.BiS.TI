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


TIM_HandleTypeDef htim1;
U32 u32Timer1Cnt;

// spi
extern SPI_HandleTypeDef hspi1;
extern structSpiTxRxBuf gstSpiTxRxBuf;

extern TIM_HandleTypeDef htim6;

/* TIM1 init function */
void TIM1_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	// 10 ms = 100Hz 
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = (PRESCALE_FOR_10US-1);
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = (PERIOD_ARR_100-1);         // should be less than 2 bytes  => 100 Hz
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	//htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
}

void TIM1_UP_TIM16_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim1); 
}


void HAL_TIM1_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	u32Timer1Cnt++;
}

void HAL_TIM1_Base_MspInit(TIM_HandleTypeDef* htim)
{
	if(htim->Instance==TIM1)
	{
		__HAL_RCC_TIM1_CLK_ENABLE();

		HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);	
	}
}

void HAL_TIM1_Base_MspDeInit(TIM_HandleTypeDef* htim)
{
	if(htim->Instance==TIM1)
	{
		__HAL_RCC_TIM1_CLK_DISABLE();

		HAL_NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
	}
}
