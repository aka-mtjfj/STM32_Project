#include "stm32f1xx.h"                  // Device header

#include "Delay.h"
/*
brief:Initializate buttons(B1,B111)
param:none
back:none
*/
void Key_Init(){
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.Mode=GPIO_MODE_INPUT  ;
	GPIO_InitStructure.Pull=GPIO_PULLDOWN;
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin=GPIO_PIN_0;
 	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
}
/*
brief:read an eight value of B1 or B11 button
param:none
back:uint8_t,an umber of which button
*/
uint8_t Key_GetNum()
{
	uint8_t keynum=0;
	if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1)
	{
		Delay_ms(20);
		while(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==1);
		Delay_ms(20);
		keynum =1;
	}
	return keynum;
}

