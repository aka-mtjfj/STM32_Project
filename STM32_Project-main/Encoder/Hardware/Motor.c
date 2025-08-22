#include "stm32f4xx.h"                  // Device header
#include "Tim.h"

/**
  * 函    数：直流电机初始化
  * 参    数：无
  * 返 回 值：无
  */
void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin=GPIO_PIN_4|GPIO_PIN_3;
 	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
												
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
}

/**
  * 函    数：直流电机设置速度
  * 参    数：Speed 要设置的速度，范围：-100~100
  * 返 回 值：无
  */
void Motor_SetPWM(int8_t PWM)
{
	if (PWM >= 0)							//如果设置正转的速度值
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, PWM);				
	}
	else									//否则，即设置反转的速度值
	{
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);
		__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, -PWM);		
	}
}
