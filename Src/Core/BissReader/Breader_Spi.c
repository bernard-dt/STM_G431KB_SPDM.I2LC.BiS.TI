#include "BreaderConfig.h"
#include "BreaderTypes.h"

const U32 auCheckLength[5][2] =
{
	{0x7FFF,0x7},
	{0x3FFF,0x3},
	{0x1FFF,0x1},
	{0xFFF,0},
	{0x7FF,0}
};

const U32 auShiftLen[5][2] =
{
	{15,3},
	{14,2},
	{13,1},
	{12,0},
	{11,0}
};

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_spi1_rx;

structSpiTxRxBuf gstSpiTxRxBuf;

U32 u32Spi1Cnt;
U32 u32Spi2Cnt;
U32 u32SpiErrCnt;
U32 u32SpiTxDmaCnt;
U32 u32SpiRxDmaCnt;

extern structBreaderData gstBreaderData;

void SPI1_Init(void)
{
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	//hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	//hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	//hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	//hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;              // 80M ->10M  , 40M ->5M  (NG)
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;               // 80M ->5M   , 40M ->2.5M  (ok)
	//hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;             // 80M ->2.5M , 40M ->1.25M
	//hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;          
	//hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;         
	//hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;            // 80M ->312.5K , 40M ->156.2K  (ok)

	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	//hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;   // removing the gap between 16bits 

	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
}

void SPI1_IRQHandler(void)
{
    uint32_t sr = hspi1.Instance->SR;
	static U32 u32TxeIrqCnt=0;
	static U32 u32RxneIrqCnt=0;
	static U32 u32ErrIrqCnt=0;
#if 0
    if (sr & SPI_SR_TXE)  u32TxeIrqCnt++;
    if (sr & SPI_SR_RXNE) u32RxneIrqCnt++;
    if (sr & (SPI_SR_OVR | SPI_SR_MODF | SPI_SR_FRE)) u32ErrIrqCnt++;
	//UartPrint("\r\nTxe [%d], Rxe [%d], Err [%d]",u32TxeIrqCnt,u32RxneIrqCnt,u32ErrIrqCnt);		
#endif

	HAL_SPI_IRQHandler(&hspi1);
	u32Spi1Cnt++;
}

void ManagingReceivedData(void)
{
	U32 u32Temp =0;
	gstBreaderData.u32BitData = gstBreaderData.u32ReceivedData >> auShiftLen[gstBreaderData.u32Pbit][gstBreaderData.u32Ptype];

	if(gstBreaderData.u32Ptype == PROTOCOL_BISS)
	{
		gstBreaderData.u32AngleE5 = GetAngle_FromBiSS(gstBreaderData.u32BitData,gstBreaderData.u32Pbit);
	}
	else
	{
		gstBreaderData.u32AngleE5 = GetAngle_FromSSI(gstBreaderData.u32BitData,gstBreaderData.u32Pbit);
	}

	u32Temp = (gstBreaderData.u32AngleE5 - gstBreaderData.u32AngleE5Prev);
	gstBreaderData.s32AngleDiffE5 = (gstBreaderData.u32AngleE5 - gstBreaderData.u32AngleE5Prev);

	gstBreaderData.u32AngleE5Prev = gstBreaderData.u32AngleE5;
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	static U32 u32Raw;

	if (hspi->Instance == SPI1)
    {
		u32Spi2Cnt++;
		u32Raw = (	(U32)gstSpiTxRxBuf.aubRxBuf[0]<< 24 |
					(U32)gstSpiTxRxBuf.aubRxBuf[1]<< 16 |		
					(U32)gstSpiTxRxBuf.aubRxBuf[2]<< 8 |					
					(U32)gstSpiTxRxBuf.aubRxBuf[3]			);
					
		// Considering Sub system's response
#if defined(SHIFT_DUMMY_1_BIT)		
		gstBreaderData.u32ReceivedData = u32Raw << 1;
#else
		gstBreaderData.u32ReceivedData = u32Raw;
#endif
		ManagingReceivedData();

	}
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	u32SpiErrCnt++;
}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(spiHandle->Instance==SPI1)
	{
		__HAL_RCC_SPI1_CLK_ENABLE();

		//__HAL_RCC_GPIOA_CLK_ENABLE();
		//__HAL_RCC_GPIOB_CLK_ENABLE();
		/**SPI1 GPIO Configuration
		PA7     ------> SPI1_MOSI
		PB3     ------> SPI1_SCK
		PB4     ------> SPI1_MISO
		*/
		
		// MOSI is not used 	
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		// PB3 = Clk
		// PB4 = MISO
		GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


		/* SPI1 DMA Init */
		/* SPI1_TX Init */
		hdma_spi1_tx.Instance = DMA1_Channel1;
		hdma_spi1_tx.Init.Request = DMA_REQUEST_SPI1_TX;
		hdma_spi1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_spi1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi1_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_spi1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		//hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		//hdma_spi1_tx.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
		//hdma_spi1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		//hdma_spi1_tx.Init.MemDataAlignment = DMA_PDATAALIGN_WORD;
		hdma_spi1_tx.Init.Mode = DMA_NORMAL;
		//hdma_spi1_tx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_spi1_tx.Init.Priority = DMA_PRIORITY_HIGH;

		if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
		{
		Error_Handler();
		}

		__HAL_LINKDMA(spiHandle,hdmatx,hdma_spi1_tx);

		/* SPI1_RX Init */
		hdma_spi1_rx.Instance = DMA1_Channel2;
		hdma_spi1_rx.Init.Request = DMA_REQUEST_SPI1_RX;
		hdma_spi1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_spi1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_spi1_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_spi1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		//hdma_spi1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		//hdma_spi1_rx.Init.MemDataAlignment = DMA_PDATAALIGN_HALFWORD;
		hdma_spi1_rx.Init.Mode = DMA_NORMAL;
		//hdma_spi1_rx.Init.Priority = DMA_PRIORITY_LOW;		
		hdma_spi1_rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;		

		if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_LINKDMA(spiHandle,hdmarx,hdma_spi1_rx);

		// SPI1 interrupt
		HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(SPI1_IRQn);
		
	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

	if(spiHandle->Instance==SPI1)
	{
		__HAL_RCC_SPI1_CLK_DISABLE();

		/**SPI1 GPIO Configuration
		PA7     ------> SPI1_MOSI
		PB3     ------> SPI1_SCK
		PB4     ------> SPI1_MISO
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7);

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3|GPIO_PIN_4);

	    /* SPI1 DMA DeInit */
    	HAL_DMA_DeInit(spiHandle->hdmatx);
    	HAL_DMA_DeInit(spiHandle->hdmarx);


		/* SPI1 interrupt Deinit */
		HAL_NVIC_DisableIRQ(SPI1_IRQn);
	}
}
