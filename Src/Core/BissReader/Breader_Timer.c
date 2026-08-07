#include "BreaderConfig.h"
#include "BreaderTypes.h"


#define PRESCALE_FOR_100US    7999
#define PRESCALE_FOR_10US     799



TIM_HandleTypeDef htim1;
U32 u32Timer1Cnt;

// spi
extern SPI_HandleTypeDef hspi1;
extern structSpiTxRxBuf gstSpiTxRxBuf;


/* TIM1 init function */
void TIM1_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	// 10 ms = 100Hz 
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = PRESCALE_FOR_100US;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = (100-1);  // should be less than 2 bytes  => 100 Hz
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	U32 ulRet;

	U32 dbg_rxneie;
	U32 dbg_txeie;
	U32 dbg_errie;

	if (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_READY)
	{
#if 0
		__HAL_SPI_CLEAR_OVRFLAG(&hspi1); 
		while (__HAL_SPI_GET_FLAG(&hspi1, SPI_FLAG_RXNE))
		{
			volatile uint16_t dummy = hspi1.Instance->DR;
			(void)dummy;
		}		
#endif		
#if 1   // 8 bit		
		gstSpiTxRxBuf.aubTxBuf[0] = 0;
		gstSpiTxRxBuf.aubTxBuf[1] = 0;
		gstSpiTxRxBuf.aubTxBuf[2] = 0;
		gstSpiTxRxBuf.aubTxBuf[3] = 0;
		//ulRet = HAL_SPI_TransmitReceive_IT(&hspi1,gstSpiTxRxBuf.aubTxBuf,gstSpiTxRxBuf.aubRxBuf,4);
		ulRet = HAL_SPI_TransmitReceive_DMA(&hspi1,gstSpiTxRxBuf.aubTxBuf,gstSpiTxRxBuf.aubRxBuf,4);
#else   // 16 bit
		CLEAR_BIT(hspi1.Instance->CR2, SPI_CR2_FRXTH);

		gstSpiTxRxBuf.auwTxBuf[0] = 0xffaa;
		gstSpiTxRxBuf.auwTxBuf[1] = 0xee55;
		ulRet = HAL_SPI_TransmitReceive_DMA(&hspi1,(U8*)gstSpiTxRxBuf.auwTxBuf,(U8*)gstSpiTxRxBuf.auwRxBuf,2);
#endif 
		u32Timer1Cnt++;
	}

#if 0
	dbg_rxneie = hspi1.Instance->CR2 & SPI_CR2_RXNEIE;
	dbg_txeie  = hspi1.Instance->CR2 & SPI_CR2_TXEIE;
	dbg_errie  = hspi1.Instance->CR2 & SPI_CR2_ERRIE;
	UartPrint("\r\nrxnie [%d], txeie [%d] errie [%d]",dbg_rxneie,dbg_txeie,dbg_errie);
#endif	
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
	if(tim_baseHandle->Instance==TIM1)
	{
		__HAL_RCC_TIM1_CLK_ENABLE();

		HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
	}
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
	if(tim_baseHandle->Instance==TIM1)
	{
		__HAL_RCC_TIM1_CLK_DISABLE();

		HAL_NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
	}
}

