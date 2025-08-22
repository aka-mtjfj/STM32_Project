#include "stm32f4xx.h"                  // Device header

void Led_Init()
{
	
		__HAL_RCC_GPIOC_CLK_ENABLE();
	  
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.Mode= GPIO_MODE_OUTPUT_PP ;
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin=GPIO_PIN_6;
 	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
	
}


void Led_On()
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
}	

	void Led_Off()
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
}	

void Led_Turn()
{
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_6)==0)
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET);
	else 
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET);
}

