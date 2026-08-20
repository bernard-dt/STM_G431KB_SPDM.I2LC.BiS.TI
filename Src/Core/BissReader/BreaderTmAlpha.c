#include "BreaderConfig.h"
#include "BreaderTypes.h"


#define PERIOD_ARR_100        100

//Timer1 : Pwm
//Timer6 : not fixed
//Timer7 : Biss Reader

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	U32 ulRet;

	if(htim->Instance == TIM1)
	{
		HAL_TIM1_PeriodElapsedCallback(htim);
	}
	
	if(htim->Instance == TIM6)
	{
		HAL_TIM6_PeriodElapsedCallback(htim);
	}
	
	if(htim->Instance == TIM7)
	{
		HAL_TIM7_PeriodElapsedCallback(htim);
	}
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim)
{
	if(htim->Instance == TIM1)
	{
		HAL_TIM1_Base_MspInit(htim);
	}
	else if(htim->Instance == TIM6)
	{
		HAL_TIM6_Base_MspInit(htim);
	}
	else if(htim->Instance == TIM7)
	{
		HAL_TIM7_Base_MspInit(htim);
	}
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim)
{
	if(htim->Instance == TIM1)
	{
		HAL_TIM1_Base_MspDeInit(htim);		
	}	
	else if(htim->Instance == TIM6)
	{
		HAL_TIM6_Base_MspDeInit(htim);
	}
	else if(htim->Instance == TIM7)
	{
		HAL_TIM7_Base_MspDeInit(htim);
	}

}

void ChangeReadTmFrequency(U32 u32Tick, TIM_HandleTypeDef *htim)
{
	U32 u32Var;
	static U32 u32FreqVar = 0;
	static U32 u32Dir = 0;

	if(PERIOD_ARR_100/2 < u32FreqVar)
	{
		u32Dir = 1;
	}
	else if( u32FreqVar < 1)
	{
		u32Dir = 0;
	}

	(u32Dir == 0) ? u32FreqVar++:u32FreqVar--;


	u32Var = (PERIOD_ARR_100 -1) - u32FreqVar;

	__HAL_TIM_SET_AUTORELOAD(htim, (u32Var));
}
