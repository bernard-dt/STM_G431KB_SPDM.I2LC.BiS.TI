#include "BreaderConfig.h"
#include "BreaderTypes.h"

// Led
#define LED2_PIN         GPIO_PIN_8
#define LED2_GPIO_PORT   GPIOB

// Protocol config
#define PROTO_TYPE_PIN         GPIO_PIN_0
#define PROTO_BIT1_PIN         GPIO_PIN_1
#define PROTO_BIT2_PIN         GPIO_PIN_4
#define PROTO_GPIO_PORT        GPIOA

#define LCD_MAX             17
#define STR_RUNNING         2
#define STR_ALARM           2
#define STR_ARROW           5
#define STR_PRT_ERR         2

const U8 * auStringOfProto[5][2] =
{
	{"S17","B17"},
	{"S18","B18"},
	{"S19","B19"},
	{"S20","B20"},
	{"S21","B21"}
};

const U8 * strClockSpeed = "2.5";

const U8 * auStrRunning[STR_RUNNING] =
{
	"\x7c",
	":",
	//"\x7e",	
};

const U8 * auStrAlarm[STR_ALARM] =
{
	"FAIL!!",
	"      ",
};

const U8 * auStrArrow[STR_ARROW] =
{
	"\x7e   ",
	"\x7e\x7e  ",	
	" \x7e\x7e ",	
	"  \x7e\x7e",
	"   \x7e"	
};

const U8 * auStrProtErr[STR_PRT_ERR] =
{
	"INVALID DATA",
	"BiSS CRC ERR"	
};


structBreaderData gstBreaderData;
structUserData gstUserData;
U8 aubRow1Str[17];
U8 aubRow2Str[17];
U32 gu32DispCnt;


// uart
extern UART_HandleTypeDef uartDev; 
extern U8 rxByte2;

// i2c
extern I2C_HandleTypeDef hi2c1;

// lcd
extern I2C_LCD_HandleTypeDef hlcd;

// timer
extern TIM_HandleTypeDef htim1;
extern U32 u32Timer1Cnt;
extern TIM_HandleTypeDef htim6;
extern U32 u32Timer6Cnt;
extern TIM_HandleTypeDef htim7;
extern U32 u32Timer7Cnt;

// spi
extern structSpiTxRxBuf gstSpiTxRxBuf;
extern SPI_HandleTypeDef hspi1;
extern U32 u32Spi1Cnt;
extern U32 u32Spi2Cnt;
extern U32 u32SpiErrCnt;
extern U32 u32SpiTxDmaCnt;
extern U32 u32SpiRxDmaCnt;

// Function
void ChangeReadTmFrequency(U32 u32Tick, TIM_HandleTypeDef *htim);




int main(void)
{
	U32 u32ModeTimer = 0;
	U32 u32LcdTimer =0;
	U32 u32FreqTimer =0;

	HAL_Init();

	SystemClock_Config();
	HAL_Delay(100);	
	GPIO_Init();

  	DMA_Init();	
	LED2_Init();
	LED2_On();	

	USART2_Init();	
	HAL_Delay(100);	
	HAL_UART_Receive_IT(&uartDev, &rxByte2, 1);

	I2C1_Init();
	LCD16x2_Init();
	
	//UartPrint("\r\nA");

	TIM1_Init();
	TIM6_Init();
	TIM7_Init();
	SPI1_Init();	


#if 0
    UartPrint("\r\nSYSCLK   : %d Hz", HAL_RCC_GetSysClockFreq()); // HSI PLL
	HAL_Delay(10);	
    UartPrint("\r\nHCLK     : %d Hz", HAL_RCC_GetHCLKFreq());     // HCLK (AHB, CPU  )
	HAL_Delay(10);	
    UartPrint("\r\nPCLK1    : %d Hz", HAL_RCC_GetPCLK1Freq());    // USART2/3, I2C, SPI2/3, TIM2~7
	HAL_Delay(10);	
    UartPrint("\r\nPCLK2    : %d Hz", HAL_RCC_GetPCLK2Freq());    // USART1, SPI1, TIM1/8, ADC
	HAL_Delay(10);	
#endif

	ProtocolConfigCheck();

	//UartPrint("\r\nB");

	//HAL_TIM_Base_Start_IT(&htim1);
	//HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_Base_Start_IT(&htim7);

	ReadDataPage63();

	//UartPrint("\r\nC");

    u32ModeTimer = u32Tick;
	u32LcdTimer = u32Tick;
	//gstBreaderData.u32CommError = COMM_ERROR;
	while (1)
	{
		ParseCmdFromHostPC();
		if(gstBreaderData.u32CommError == NO_ERROR )
		{
			if(((u32Tick - u32LcdTimer) >= 1000) || (u32Tick < u32LcdTimer))
			{
				u32LcdTimer = u32Tick;
				LED2_Toggle();
				LcdDisplayUpdate();		
			}
			if(((u32Tick - u32FreqTimer) >= 5000) || (u32Tick < u32FreqTimer))
			{
				u32FreqTimer = u32Tick;
				ChangeReadTmFrequency(u32Tick,&htim7);
				
			}
		}
		else
		{
			if(((u32Tick - u32LcdTimer) >= 500) || (u32Tick < u32LcdTimer))
			{
				u32LcdTimer = u32Tick;
				LED2_Toggle();
				LcdDisplayUpdate();		
			}
		}
#if 1
		if(((u32Tick - u32ModeTimer) >= 2000) || (u32Tick < u32ModeTimer))
		{
			u32ModeTimer = u32Tick;
			UartPrint("\r\nT1=%d, T6=%d CRC_Error[%d]",u32Timer1Cnt,u32Timer6Cnt,gstBreaderData.u32CrcErrCnt);
		}
#endif		
	}
}


void LcdDisplayUpdate(void)
{
	//lcd_clear(&hlcd);

#if 1
	if(gstBreaderData.u32CommError == NO_ERROR )
	{
		snprintf(aubRow1Str,LCD_MAX,"[%s%s%s] %4dHz ",auStringOfProto[gstBreaderData.ProtoBit][gstBreaderData.ProtoType], \
		                                               auStrRunning[gu32DispCnt%STR_RUNNING],strClockSpeed,gstBreaderData.u32DispStackCnt);
		if((gstBreaderData.u32AngleDiffE5 >= 100000))
		{
			snprintf(aubRow2Str,LCD_MAX,"%08.4f -OVER 1  ",(F32)gstBreaderData.u32AngleE5*0.00001f);
		}
		else
		{	
			snprintf(aubRow2Str,LCD_MAX,"%08.4f -0.%04d  ",(F32)gstBreaderData.u32AngleE5*0.00001f,(U32)(gstBreaderData.u32AngleDiffE5*0.1f) );												
		}		
	}
	else
	{
		snprintf(aubRow1Str,LCD_MAX,"[%s%s%s] %s ",auStringOfProto[gstBreaderData.ProtoBit][gstBreaderData.ProtoType],auStrRunning[gu32DispCnt%STR_RUNNING],strClockSpeed,auStrAlarm[gu32DispCnt%STR_ALARM],auStrAlarm[gu32DispCnt%STR_ALARM]);		
		snprintf(aubRow2Str,LCD_MAX,"%s%s        ",auStrArrow[gu32DispCnt%STR_ARROW],auStrProtErr[gstBreaderData.ProtoType]);			
	}
#else
	if(gstBreaderData.u32CommError == NO_ERROR )
	{
		snprintf(aubRow1Str,LCD_MAX,"%s[%s] %4dHz  %s",auStrToggle[gu32DispCnt%STR_TOGGLE],auStringOfProto[gstBreaderData.ProtoBit][gstBreaderData.ProtoType], \
		                          gstBreaderData.u32DispStackCnt,auStrToggle[gu32DispCnt%STR_TOGGLE]);
		if((gstBreaderData.u32AngleDiffE5 >= 100000))
		{
			snprintf(aubRow2Str,LCD_MAX," %7.3f {>.999}",(F32)gstBreaderData.u32AngleE5*0.00001f);
		}
		else
		{					
			snprintf(aubRow2Str,LCD_MAX," %7.3f {.%04d}",(F32)gstBreaderData.u32AngleE5*0.00001f,(U32)(gstBreaderData.u32AngleDiffE5*0.1f) );					
		}		
	}
	else
	{
		snprintf(aubRow1Str,LCD_MAX,"%s[%s]  FAIL   %s   ",auStrToggle[gu32DispCnt%STR_TOGGLE],auStringOfProto[gstBreaderData.ProtoBit][gstBreaderData.ProtoType],auStrToggle[gu32DispCnt%STR_TOGGLE]);
		snprintf(aubRow2Str,LCD_MAX," %s%s        ",auStrArrow[gu32DispCnt%STR_ARROW],auStrProtErr[gstBreaderData.ProtoType]);			
	}
#endif
	gstBreaderData.u32DispStackCnt = 0;

	lcd_gotoxy(&hlcd,0,0);
	lcd_puts(&hlcd,&aubRow1Str[0]);

	lcd_gotoxy(&hlcd,0,1);
	lcd_puts(&hlcd,&aubRow2Str[0]);	

	gu32DispCnt++;	
}



void HAL_MspInit(void)
{
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	HAL_PWREx_DisableUCPDDeadBattery();
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

#if 0
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
#else   
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
#endif	

	/** Initializes the RCC Oscillators according to the specified parameters
	* in the RCC_OscInitTypeDef structure.
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
#if 0
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
#else	
 	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;  // 16M / 4 = 4M
   	RCC_OscInitStruct.PLL.PLLN = 80;             // 4M * 80 = 320M 

#if 0
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;  // 320MHz /2 = 160MHz 
#else
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;  // 320MHz /4 = 80MHz
#endif	
#endif	
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
#if 0
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
#else
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;     // sys clk
#endif 
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;           // 80MHz
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;           // 80MHz  spi 
	//RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;           // 40MHz  spi 

#if 0	
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
#else
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
#endif
	{
		Error_Handler();
	}
}

void GPIO_Init(void)
{
	GPIO_InitTypeDef  gpio_init_structure ={0};	  

    gpio_init_structure.Mode  = GPIO_MODE_ANALOG;
	gpio_init_structure.Pull  = GPIO_NOPULL;
	gpio_init_structure.Pin   = GPIO_PIN_ALL;		

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_Init(GPIOA, &gpio_init_structure);
	HAL_GPIO_Init(GPIOB, &gpio_init_structure);


	// PA0, PA1, PA3 : Input, Pull-down
	// this is for Function Config
	// PA2 & PA3 are occupied by VCP	
	gpio_init_structure.Pin  = PROTO_TYPE_PIN | PROTO_BIT1_PIN | PROTO_BIT2_PIN;
	gpio_init_structure.Mode = GPIO_MODE_INPUT;
	gpio_init_structure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &gpio_init_structure);
	
}

void LED2_Init(void)
{
	int32_t ret = 0;
	GPIO_InitTypeDef  gpio_init_structure ={0};

	gpio_init_structure.Pin   = LED2_PIN;
	gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull  = GPIO_NOPULL;
	gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

	HAL_GPIO_Init(LED2_GPIO_PORT, &gpio_init_structure);
}
void LED2_On(void)
{
	HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);
}
void LED2_Off(void)
{
	HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_RESET);
}
void LED2_Toggle(void)
{
	HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
}

void ProtocolConfigCheck(void)
{
	U32 u32Type =0, u32Bit1 =0, u32Bit2 =0;
	U32 u32BitResult =0;

	u32Type = HAL_GPIO_ReadPin(PROTO_GPIO_PORT,PROTO_TYPE_PIN);
	u32Bit1 = HAL_GPIO_ReadPin(PROTO_GPIO_PORT,PROTO_BIT1_PIN);
	u32Bit2 = HAL_GPIO_ReadPin(PROTO_GPIO_PORT,PROTO_BIT2_PIN);

	gstBreaderData.ProtoType = u32Type;
	gstBreaderData.ProtoBit = u32Bit1*2 + u32Bit2;	

	if(u32Bit1)
	{
		gstBreaderData.ProtoBit = (u32Bit2)? PROTOCOL_BIT_21:PROTOCOL_BIT_20;
	}
	else
	{
		gstBreaderData.ProtoBit = (u32Bit2)? PROTOCOL_BIT_19:PROTOCOL_BIT_17;
	}
	//UartPrint("\r\nType =%d , Bit =%d",gstBreaderData.ProtoType,gstBreaderData.ProtoBit);

	//Dbg
	gstBreaderData.ProtoType = PROTOCOL_BISS;
	gstBreaderData.ProtoBit = PROTOCOL_BIT_19;		
}


/* Debug Handler */
void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
		ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
