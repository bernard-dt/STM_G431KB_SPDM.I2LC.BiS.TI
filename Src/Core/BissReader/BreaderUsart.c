#include <stdarg.h>
#include <stdio.h>
#include "BreaderConfig.h"
#include "BreaderTypes.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef uartDev;

U8 gUartBuffer2[UART_RX_BUFFER_MAX];

S32 idxWrToBuf2;
S32 idxRdFromBuf2;
U8 rxByte2;

const U8 HOST_HEADER            = 'H';

const U8 HOST_VER[]             = "VR";



void HandleCmdFromHostPc(U8 u8Msb, U8 u8Lsb);

void USART2_Init(void)
{
  uartDev.Instance = USART2;
  uartDev.Init.BaudRate = 115200;
  uartDev.Init.WordLength = UART_WORDLENGTH_8B;
  uartDev.Init.StopBits = UART_STOPBITS_1;
  uartDev.Init.Parity = UART_PARITY_NONE;
  uartDev.Init.Mode = UART_MODE_TX_RX;
  uartDev.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartDev.Init.OverSampling = UART_OVERSAMPLING_16;
  uartDev.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uartDev.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  uartDev.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&uartDev) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&uartDev, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&uartDev, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&uartDev) != HAL_OK)
  {
    Error_Handler();
  }
}

void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&uartDev);
}

void HAL_USART2_RxCpltCallback(UART_HandleTypeDef *huart)
{
	gUartBuffer2[idxWrToBuf2] = rxByte2;
	idxWrToBuf2 = (idxWrToBuf2 >= UART_RX_BUFFER_MAX - 1) ? 0 : idxWrToBuf2 + 1;
	HAL_UART_Receive_IT(&uartDev, &rxByte2, 1);
}

static U32 GetByteFromHostPc(U8* pubData)
{
	U32 Count = 0;
	U32 ulRet = 1;

	while(idxRdFromBuf2 == idxWrToBuf2)
	{
		Count++;
		if(Count > 3)
		{
			ulRet = 0;		
			break;	
		}
		else
		{
			HAL_Delay(1);	
		}		
	}
	if(1 == ulRet)
	{
		*pubData = gUartBuffer2[idxRdFromBuf2];
		idxRdFromBuf2++; 		  
		idxRdFromBuf2 %= UART_RX_BUFFER_MAX;  
	}
	else {
		*pubData = 0xff;
	}	
	return ulRet;
}

void ParseCmdFromHostPC(void)
{
	U32 u32Read =0, u32Result =0;
	U8 u8Byte, u8Msb, u8Lsb;

	if(idxRdFromBuf2 != idxWrToBuf2)
    {		
		u32Read = GetByteFromHostPc(&u8Byte);
		if((1 == u32Read) && (HOST_HEADER == u8Byte))
		{
			u32Read = GetByteFromHostPc(&u8Msb);		
			if(1 == u32Read)
			{
				u32Read = GetByteFromHostPc(&u8Lsb);
				if(1 == u32Read)
				{
					u32Result = 1;
					HandleCmdFromHostPc(u8Msb,u8Lsb);
				}
			}
		}
		if(0 == u32Result)
		{
			idxRdFromBuf2 = idxWrToBuf2;
		}
    }
}

void UartSendData(U8 * pu8Buf, U32 u32Size)
{
	HAL_UART_Transmit(&uartDev,pu8Buf,u32Size, 1000);    
}

void HandleCmdFromHostPc(U8 u8Msb, U8 u8Lsb)
{

	if((u8Msb== HOST_VER[0]) && (u8Lsb == HOST_VER[1]))
	{
		UartPrint("\r\nVersion is Not Read !!");
	}
	else
	{
		;
	}
}

void UartPrint(const char *format, ...)
{
//#if defined(DBG_UART_SUPPORT)	
#if 1
	U8 aubTxBuf1[UART_TX_BUFFER_MAX] ={0,};	
    va_list args;
    U32 u32Len;

	va_start(args, format);    
    u32Len = vsnprintf(aubTxBuf1, UART_TX_BUFFER_MAX, format, args);
    va_end(args);
	
    if (u32Len > 0)
    {
        HAL_UART_Transmit(&uartDev, (uint8_t *)aubTxBuf1, u32Len, HAL_MAX_DELAY);
    }
#endif
}


void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	if(uartHandle->Instance==USART2)
	{
		PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
		PeriphClkInit.Usart1ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		{
			Error_Handler();
		}

		__HAL_RCC_USART2_CLK_ENABLE();

		//__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
	if(uartHandle->Instance==USART2)
	{
		__HAL_RCC_USART2_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

		HAL_NVIC_DisableIRQ(USART2_IRQn);
	}
}